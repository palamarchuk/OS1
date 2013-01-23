#include <stdio.h>
#include <pthread.h>
#include <errno.h>

int lock = 0;
int testvar = 0;
int thread_num = 100;
int top_sum = 100000;
int cas (int *m, int old, int new)
{
	int ret;
	asm volatile("lock\n\tcmpxchgl %1,%2"
		: "=a"(ret)
		: "r"(new), "m"(*m), "0"(old)
		: "memory"
	);
	return ret;
}

void enter(void)
{
	while(cas(&lock, 0, 1) == 1);
	return;
}

void leave(void)
{
	lock = 0;
	return;
}

void * test(void * arg)
{
	enter();
	int j;
	for (j=0; j < top_sum; j++)
		testvar++;
	leave();
}

void main(void)
{
	int i, j;
	int err_n;
	pthread_t thread[thread_num];
	for (i = 0; i < thread_num; i++)
		if (pthread_create(&thread[i], NULL, test, NULL) != 0)
			break;
	for (j = 0; j < i; j++)
		pthread_join(thread[j], NULL);
	printf("%d == %d\n", thread_num * top_sum, testvar);
	return;
}