#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#define N 5
int life_array[N+2][N+2];
int sv[2];
void Networker()
{
	sleep(1);
	printf("Hello, i'm child\n");
	FILE* buf = fopen("buffer","rb");
        mmap(NULL,N*N,PROT_READ+PROT_WRITE,MAP_SHARED,fileno(buf),0);
	//printf("%c",fgetc(buf));
	fclose(buf);
	int e = 7;
	send(sv[0], &e, sizeof(int),0);

}
void Lifer()
{
	//FILE* buf = fopen("buffer","wb");
        //mmap(NULL,N*N,PROT_READ+PROT_WRITE,MAP_SHARED,fileno(buf),0);
        //fputc('?',buf);
        //fclose(buf);
	int e;
	recv(sv[1],&e,sizeof(int),0);
	printf("%d",e);
	/*
        FILE* in = fopen ("input.txt", "rb");
        for ( int i = 1; i<N+1; i++)
                for (int j = 1; j< N+1; j++)
                {
                        int c = fgetc(in);
			if (c == EOF)
                        {
                                i=N+1;
                                break;
                        }
                        life_array[i][j] = c - '0';
                }
        fclose(in);
        while(1)
	{
	int temp_array[N+2][N+2];
        for (int i = 1; i < N+1; i++)
                for (int j = 1; j < N+1; j++)
                {
                        int a = -life_array[i][j];
                        for(int x = - 1; x < 2; x++)
                                for (int y = -1; y < 2; y++)
                                        a+=life_array[i+x][j+y];
                        if (life_array[i][j]==0)
                        {
                                if (a == 3)
                                        temp_array[i][j] = 1;
                                else
                                        temp_array[i][j] = 0;
                        }
                        else
                                if (a != 2 && a != 3)
                                        temp_array[i][j]=0;
                                else
                                        temp_array[i][j]=1;
                }
        FILE* out = fopen ("buffer", "wb");
        for ( int i = 1; i<N+1; i++)
                for (int j = 1; j< N+1; j++)
                        fputc(temp_array[i][j] +'0', out);
                 fclose(out);
	for(int i = 1;i<N+1;i++)
		for(int j=1;j<N+1;j++)
			life_array[i][j]=temp_array[i][j];
	sleep(1);
	}*/
        //return 0;

}
int main()
{
	//FILE* buf = fopen("buffer","wb+");
	//mmap(NULL,N*N,PROT_READ+PROT_WRITE,MAP_SHARED,fileno(buf),0);
	//fputs("123\n",buf);
	//fprintf(buf,"%c",'?');
	//printf("1\n");
	//fclose(buf);
	//int sv[2];
	socketpair(AF_LOCAL,SOCK_STREAM,0,sv);
	pid_t networker = fork();
	
	//printf("2\n");
	if(networker==0) 
	{
		Networker();
		return 0;
	}
	else
	{
		Lifer();
		return 0;
	}
return 0;
}
