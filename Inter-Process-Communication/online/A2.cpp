// Solve from 1905008 - Shattik Islam
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

sem_t semp, semq, semr, mutex, line, linep, lineq, liner;

void *threadp(void *)
{
    while(true){
    sem_wait(&line);
    sem_wait(&mutex);
    cout << "p, ";
    sem_post(&mutex);
    sem_post(&semp);
    sem_post(&semp);
    sem_post(&linep);
    sem_wait(&semq);
    sem_wait(&semr);
    }
}

void *threadq(void *)
{
    while(true){
    sem_wait(&line);
    sem_wait(&mutex);
    cout << "q, ";
    sem_post(&mutex);
    sem_post(&semq);
    sem_post(&semq);
    sem_post(&lineq);
    sem_wait(&semp);
    sem_wait(&semr);
    }
}

void *threadr(void *)
{
    while(true){
    sem_wait(&line);
    sem_wait(&mutex);
    cout << "r, ";
    sem_post(&mutex);
    sem_post(&semr);
    sem_post(&semr);
    sem_post(&liner);
    sem_wait(&semp);
    sem_wait(&semq);
    }
}

void *threadLine(void *)
{
    while(true){
    sem_wait(&linep);
    sem_wait(&lineq);
    sem_wait(&liner);
    sem_wait(&mutex);
    cout << endl;
    sem_post(&mutex);
    sem_post(&line);
    sem_post(&line);    
    sem_post(&line);
    }
}

int main()
{
    freopen("A2.text", "w", stdout);

    sem_init(&semp, 0, 0);
    sem_init(&semq, 0, 0);
    sem_init(&semr, 0, 0);
    sem_init(&mutex, 0, 1);
    sem_init(&line, 0, 3);
    sem_init(&linep, 0, 0);
    sem_init(&lineq, 0, 0);
    sem_init(&liner, 0, 0);
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, threadp, NULL);
    pthread_create(&t2, NULL, threadq, NULL);
    pthread_create(&t3, NULL, threadr, NULL);
    pthread_create(&t4, NULL, threadLine, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    return 0;
}