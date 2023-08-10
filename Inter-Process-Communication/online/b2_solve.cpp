#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

sem_t mutex_1;
sem_t mutex_2;

void* foo1(void* arg){
    int n = (intptr_t)arg;

    for(int i = 2 * n - 1; i >= n; i--){
        sem_wait(&mutex_1);
        for(int j = 1; j <= i; j++){
            cout << "_";
        }
        sem_post(&mutex_2);
    }

    pthread_exit(NULL);
}

void* foo2(void* arg){
    int n = (intptr_t)arg;

    for(int i = 1; i <= n; i++){
        sem_wait(&mutex_2);
        for(int j = 1; j <= i; j++){
            cout << "+";
        }
        cout << endl;
        sem_post(&mutex_1);
    }

    pthread_exit(NULL);
}

int main(){
    sem_init(&mutex_1, 0, 1);
    sem_init(&mutex_2, 0, 0);

    int n;
    cin >> n;
    pthread_t thr1, thr2;
    pthread_create(&thr1, NULL, foo1, (intptr_t*)n);
    pthread_create(&thr2, NULL, foo2, (intptr_t*)n);

    // without joining the two threads, defective output is shown
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

    return 0;
}