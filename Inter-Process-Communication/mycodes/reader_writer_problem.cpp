#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <random>

pthread_mutex_t entry_book;
pthread_mutex_t reader_count;

int readersCount = 0;

// Function for readers
void* readEntryBook(void* arg) {
    int readerId = *static_cast<int*>(arg);
    delete static_cast<int*>(arg);

    pthread_mutex_lock(&reader_count);
    ++readersCount;
    if (readersCount == 1) {
        pthread_mutex_lock(&entry_book);
    }
    pthread_mutex_unlock(&reader_count);

    // Reading the entry book
    sleep(1);
    std::cout << "Reader " << readerId << " reads the entry book.\n";

    pthread_mutex_lock(&reader_count);
    --readersCount;
    if (readersCount == 0) {
        pthread_mutex_unlock(&entry_book);
    }
    pthread_mutex_unlock(&reader_count);

    return NULL;
}

// Function for writers
void* writeEntryBook(void* arg) {
    int writerId = *static_cast<int*>(arg);
    delete static_cast<int*>(arg);

    pthread_mutex_lock(&entry_book);

    // Writing to the entry book
    sleep(2);
    std::cout << "Writer " << writerId << " writes to the entry book.\n";

    pthread_mutex_unlock(&entry_book);

    return NULL;
}

int main() {
    const int numReaders = 5;
    const int numWriters = 2;

    pthread_mutex_init(&entry_book, NULL);
    pthread_mutex_init(&reader_count, NULL);

    pthread_t readers[numReaders];
    pthread_t writers[numWriters];

    // Start readers and writers
    int r = 0, w = 0;
    while((r + w) < numReaders + numWriters){
        int p = rand() % 2;
        if(p && r < numReaders){
            int* id = new int(r + 1);
            pthread_create(&readers[r], NULL, readEntryBook, static_cast<void*>(id));
            r++;
        }
        else if(!p && w < numWriters){
            int* id = new int(w + 1);
            pthread_create(&writers[w], NULL, writeEntryBook, static_cast<void*>(id));
            w++;
        }
    }

    // Join all reader threads
    for (int i = 0; i < numReaders; ++i) {
        pthread_join(readers[i], NULL);
    }

    // Join all writer threads
    for (int i = 0; i < numWriters; ++i) {
        pthread_join(writers[i], NULL);
    }

    return 0;
}
