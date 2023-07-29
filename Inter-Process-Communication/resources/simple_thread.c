#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include <unistd.h>

void * threadFunc1(void * arg)
{
	int i;
	for(i=1;i<=5;i++)
	{
		printf("%s\n",(char*)arg);
		sleep(1);		//sleeps for 1sec
	}
}

void * threadFunc2(void * arg)
{
	int i;
	for(i=1;i<=5;i++)
	{
		printf("%s\n",(char*)arg);
		sleep(1);
	}
}

int main(void)
{	
	pthread_t thread1;
	pthread_t thread2;
	
	char * message1 = "i am thread 1";
	char * message2 = "i am thread 2";	
	
	pthread_create(&thread1,NULL,threadFunc1,(void*)message1 );
	pthread_create(&thread2,NULL,threadFunc2,(void*)message2 );

	pthread_exit(NULL);		//exits from main() ONLY AFTER finishing both the threads
	// while(1);	//doesn't exit from main()
	return 0;
}
