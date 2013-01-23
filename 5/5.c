#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <sys/queue.h>
#include <time.h>

#define thread_count 15

ucontext_t main_context, schedule_context;
char schedule_stack[SIGSTKSZ];
int current_id;

ucontext_t context[thread_count];
char stack[thread_count][SIGSTKSZ];
int sleep_sec[thread_count];

int cur_priority = 0, rem_times = 3;

struct foo 
{
    int id;
    TAILQ_ENTRY(foo) pointers;
};
TAILQ_HEAD(foo_list, foo) head2, head1, head0;

int kb_thread(void * func)
{
	getcontext(&context[current_id]);
	context[current_id].uc_link = &schedule_context;
    context[current_id].uc_stack.ss_sp = stack[current_id];
    context[current_id].uc_stack.ss_size = sizeof(stack[current_id]);
	makecontext(&context[current_id], func, 0);
	return current_id;
}

void kb_exit()
{
	printf("task %d ends\n", current_id);
	sleep_sec[current_id] = -1; //значит никогда не проснется
	//setcontext(&schedule_context);
}

int kb_sleep(int seconds)
{
	if (seconds > 0)
	{
		printf("task %d sleeps for %d sec\n", current_id, seconds);
		sleep_sec[current_id] = seconds;
		swapcontext(&context[current_id], &schedule_context);
	}
	return 0;
}

void AlarmHandler(int SigNumber, siginfo_t * SI, void * icontext)
{
	struct foo *item;
	TAILQ_FOREACH(item, &head2, pointers)
	   if (sleep_sec[item->id] > 0)
			sleep_sec[item->id]--;
	TAILQ_FOREACH(item, &head1, pointers)
	   if (sleep_sec[item->id] > 0)
			sleep_sec[item->id]--;
	TAILQ_FOREACH(item, &head0, pointers)
	   if (sleep_sec[item->id] > 0)
			sleep_sec[item->id]--;
	alarm(1);
	if (current_id != -1)
		context[current_id] = *(ucontext_t*)icontext;
	current_id = -1;
	setcontext(&schedule_context);
}

void schedule(int sig)
{
	current_id = -1;
	struct foo *item, *ff;
	TAILQ_HEAD(foo_list, foo)* head;
	int tmpid;
	if (rem_times == 0)
	{
		cur_priority++;
		if (cur_priority == 3)
			cur_priority = 0;
		rem_times = 3 - cur_priority;
	}
	switch(cur_priority)
	{
		case 0: head = &head0; break;
		case 1: head = &head1; break;
		case 2: head = &head2; break;
	}
	ff = TAILQ_FIRST(head);
	if (ff != NULL)
	{
		do
		{
			item = TAILQ_FIRST(head);
			tmpid = item->id;
			TAILQ_REMOVE(head, item, pointers);
			TAILQ_INSERT_TAIL(head, item, pointers);
		}
		while (sleep_sec[tmpid] != 0 && ff->id != head->tqh_first->id);
	}
	if (ff == head->tqh_first)
	{
		if (cur_priority < 2)
			cur_priority++;
		else
			while(ff == head->tqh_first)
		rem_times = 3 - cur_priority;
		setcontext(&schedule_context);
	}
	printf("wake up task %d\n", tmpid);
	rem_times--;
	current_id = tmpid;
	setcontext(&context[current_id]);
}

void IOBound()
{
	while(1)
	{
		printf("my name is %d\n", current_id);
		kb_sleep(rand() % 16);
	}
	kb_exit();
}

void SemiCPUBound()
{
	int i, j, f;
	for (i = 2; i < 2000000; i++)
	{
		f = 0;
		for (j = 2; j <= sqrt(i); j++)
			if (i % j == 0)
			{
				f = 1;
				break;
			}
		if (f == 0)
		{
			printf("", current_id, i);
			kb_sleep(2);
		}
	}
	kb_exit();
}

void CPUBound()
{
	int i, j, f;
	for (i = 2; i < 2000000000; i++)
	{
		f = 0;
		for (j = 2; j <= sqrt(i); j++)
			if (i % j == 0)
			{
				f = 1;
				break;
			}
		if (f == 0)
			printf("", current_id, i);
	}
	kb_exit();
}

int main()
{
	srand(time(NULL));
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = AlarmHandler;
	sigaction(SIGALRM, &sa, NULL);
	
	getcontext(&schedule_context);
	schedule_context.uc_link = &main_context;
    schedule_context.uc_stack.ss_sp = schedule_stack;
    schedule_context.uc_stack.ss_size = sizeof(schedule_stack);
	makecontext(&schedule_context,(void (*)(void))schedule, 1, SIGUSR1);
	
    TAILQ_INIT(&head2);
    TAILQ_INIT(&head1);
    TAILQ_INIT(&head0);
	memset(&sleep_sec, 0, sizeof(sleep_sec));
	struct foo *item;
	for (current_id = 0; current_id < thread_count / 3; current_id++)
	{
		item = malloc(sizeof(struct foo));
		item->id = kb_thread(CPUBound);
		TAILQ_INSERT_TAIL(&head2, item, pointers);
	}
	for (; current_id < thread_count * 2 / 3; current_id++)
	{
		item = malloc(sizeof(struct foo));
		item->id = kb_thread(SemiCPUBound);
		TAILQ_INSERT_TAIL(&head1, item, pointers);
	}
	for (; current_id < thread_count; current_id++)
	{
		item = malloc(sizeof(struct foo));
		item->id = kb_thread(IOBound);
		TAILQ_INSERT_TAIL(&head0, item, pointers);
	}
	alarm(1);
	swapcontext(&main_context, &schedule_context);
	
    while (!TAILQ_EMPTY(&head2))
    {
        item = TAILQ_FIRST(&head2);
        TAILQ_REMOVE(&head2, item, pointers);
        free(item);
    }
	while (!TAILQ_EMPTY(&head1))
    {
        item = TAILQ_FIRST(&head1);
        TAILQ_REMOVE(&head1, item, pointers);
        free(item);
    }
	while (!TAILQ_EMPTY(&head0))
    {
        item = TAILQ_FIRST(&head0);
        TAILQ_REMOVE(&head0, item, pointers);
        free(item);
    }
}