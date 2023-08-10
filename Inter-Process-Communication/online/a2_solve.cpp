#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <vector>
#define N_t 3
#define next_thread(i) ((i+1) % N_t)

using namespace std;

int k = 0;
pthread_mutex_t mutex_iter;
sem_t S[N_t];

void* foo(void* arg) {
    char ch = *((char*)arg);
    delete (char*)arg;
    arg = nullptr;
    int i = (int) (ch - 'p');
    for(int j = 0; j < 20; j++){
        sem_wait(&S[i]);
        cout << ch << ", ";
        pthread_mutex_lock(&mutex_iter);
        k++;
        if(!(k%3)){
            cout << "[iteration " << (k/3) << "]\n";
        }
        pthread_mutex_unlock(&mutex_iter);
        sem_post(&S[next_thread(i)]);
    }
    pthread_exit(NULL);
}

int main(){
    freopen("output.txt", "w", stdout);
    pthread_mutex_init(&mutex_iter, NULL);
    sem_init(&S[0], 0, 1);
    for(int i = 1; i < N_t; i++){
        sem_init(&S[i], 0, 0);
    }
    pthread_t mythreads[N_t];
    char c = 'p';
    for (int i = 0; i < N_t; i++) {
        char* ch = new char(c);
        pthread_create(&mythreads[i], NULL, foo, (void*)ch);
        c++;
    }

    // without joining all the threads, defective output is shown
    for(int i = 0; i < N_t; i++){
        pthread_join(mythreads[i], NULL);
    }

    return 0;
}