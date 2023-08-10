// Solve from 1905008 - Shattik Islam
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

sem_t sem1, sem2;

void *thread1(void *arg)
{
    int n = (intptr_t)arg;
    for(int i=0; i<n; i++){
        sem_wait(&sem1);
        for(int j=0; j<(2*n-i-1); j++){
            cout << "_";
        }
        sem_post(&sem2);
    }
}

void *thread2(void *arg)
{
    int n = (intptr_t)arg;
    for(int i=0; i<n; i++){
        sem_wait(&sem2);
        for(int j=0; j<(i+1); j++){
            cout << "*";
        }
        cout<<endl;
        sem_post(&sem1);
    }
}

int main()
{
    int n;
    cin >> n;
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, (void *)(intptr_t)n);
    pthread_create(&t2, NULL, thread2, (void *)(intptr_t)n);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    cout << endl;
    return 0;
}