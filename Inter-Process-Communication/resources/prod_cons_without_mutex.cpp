#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#define N 5
using namespace std;


//semaphore to control sleep and wake up
sem_t blank;
sem_t full;
queue<int> q;


void init_semaphore()
{
	sem_init(&blank,0,N);
	sem_init(&full,0,0);
}

void * ProducerFunc(void * arg)
{	
	printf("%s\n",(char*)arg);
	for(int i=1;i<=10;i++)
	{
		sem_wait(&blank);

		sleep(1);
		
		q.push(i);
		printf("producer produced item %d\n",i);
	
		sem_post(&full);
	}
}

void * ConsumerFunc(void * arg)
{
	printf("%s\n",(char*)arg);
	for(int i=1;i<=10;i++)
	{	
		sem_wait(&full);
 		
		sleep(1);

		int item = q.front();
		q.pop();
		printf("consumer consumed item %d\n",item);	
			
		sem_post(&blank);
	}
}

int main(void)
{	
	pthread_t thread1;
	pthread_t thread2;
	
	init_semaphore();
	
	char * message1 = "i am producer";
	char * message2 = "i am consumer";	
	
	pthread_create(&thread1,NULL,ProducerFunc,(void*)message1 );
	pthread_create(&thread2,NULL,ConsumerFunc,(void*)message2 );

	pthread_exit(NULL);		//exits from main() ONLY AFTER finishing both the threads
	// while(1);	//doesn't exit from main()

	return 0;
}
