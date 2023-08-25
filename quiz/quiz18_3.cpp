#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

sem_t sema, semb;

void *printA(void*){
    for(int i = 0; i < 20; i++){
        sem_wait(&sema);  // Wait until allowed to print A
        printf("A ");
        sem_post(&semb);  // Allow B to print
    }
}

void *printB(void*){
    for(int i = 0; i < 20; i++){
        sem_wait(&semb);  // Wait until allowed to print B
        printf("B ");
        sem_post(&sema);  // Allow A to print
    }
}

int main()
{
    sem_init(&sema, 0, 1);  // Start with A being allowed to print
    sem_init(&semb, 0, 0);
    
    pthread_t t1, t2;
    printf("\n");
    
    pthread_create(&t1, NULL, printA, NULL);
    pthread_create(&t2, NULL, printB, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("\n");
    return 0;
}
