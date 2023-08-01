#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <chrono>
#include <random>
#include <unistd.h>

#define timestamp chrono::high_resolution_clock::time_point
#define current_time() chrono::high_resolution_clock::now()
#define get_seconds(t2, t1) duration_cast<seconds>(t2 - t1).count()

using namespace std;
using namespace std::chrono;

const int N = 15;  // Total number of students
const int M = 5;   // Group size
const int NUM_GROUPS = N / M;
const int NUM_PRINTERS = 4;
const int w = 10;
const int x = 8;
const int y = 3;


pthread_mutex_t printMutex[NUM_PRINTERS] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
                                            PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
sem_t printerAvailable[NUM_PRINTERS];
vector<sem_t> groupSemaphores(NUM_GROUPS);
pthread_mutex_t outputMutex = PTHREAD_MUTEX_INITIALIZER;
timestamp initialTime;

std::random_device rd;          // obtain a random seed from the hardware
std::mt19937 generator(rd());   // Mersenne Twister 19937 engine
double mean_delay = 1.6;        // adjust to control expected delay
poisson_distribution<int> poisson(mean_delay);

void* arrivePS(void* arg) {
    int id = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    int time = get_seconds(current_time(), initialTime);
    timestamp lastTime = current_time();
    int pid = (id - 1) % NUM_PRINTERS;
    int gid = (id - 1) / M;         // Calculate the group number for the student

    pthread_mutex_lock(&outputMutex);
    cout << "Student " << id << " has arrived at PS " << (pid+1) << " at time " << time << endl;
    pthread_mutex_unlock(&outputMutex);

    // Wait for the assigned printer to be available
    sem_wait(&printerAvailable[pid]);

    pthread_mutex_lock(&printMutex[pid]);

    // Simulate printing
    // usleep(500000); // Sleep for 0.5 seconds
    time += get_seconds(current_time(), lastTime);

    pthread_mutex_lock(&outputMutex);
    cout << "Student " << id << " has started printing at PS " << (pid + 1) << " at time " << time << endl;
    pthread_mutex_unlock(&outputMutex);

    sleep(w);
    time += w;

    pthread_mutex_lock(&outputMutex);
    cout << "Student " << id << " has finished printing at PS "<< (pid + 1) << " at time " << time << endl;
    pthread_mutex_unlock(&outputMutex);

    // pthread_mutex_unlock(&printMutex[pid]);

    // Release the assigned printer
    sem_post(&printerAvailable[pid]);

    // If it's the last student in the group, signal the group leader (SM)
    if (id % M == 0) {
        sem_post(&groupSemaphores[gid]);
    }

    pthread_exit(NULL);
}

void* groupLeader(void* arg) {
    int groupId = *static_cast<int*>(arg);
    delete static_cast<int*>(arg);

    // Wait for all students in the group to finish printing
    for (int i = 0; i < M; ++i) {
        sem_wait(&groupSemaphores[groupId]);
    }

    pthread_mutex_lock(&outputMutex);
    cout << "Group " << groupId + 1 << " (Group Leader - SM) has received all printed copies.\n";
    pthread_mutex_unlock(&outputMutex);

    pthread_exit(NULL);
}

int main() {

    pthread_t students[N];
    pthread_t groupLeaders[NUM_GROUPS];

    // Initialize the printer semaphores to 1 (all printers are available)
    for (int i = 0; i < NUM_PRINTERS; ++i) {
        sem_init(&printerAvailable[i], 0, 1);
    }

    // Initialize the group semaphores to 0 (none of the students in a group have finished printing)
    for (int i = 0; i < NUM_GROUPS; ++i) {
        sem_init(&groupSemaphores[i], 0, 0);
    }

    initialTime = current_time();   //  Initial timestamp to determine program startup time (GLOBAL)

    // Start student threads
    for (int i = 0; i < N; ++i) {
        int* studentId = new int(i + 1);
        pthread_create(&students[i], NULL, arrivePS, static_cast<void*>(studentId));
    }

    // Start group leader threads
    for (int i = 0; i < NUM_GROUPS; ++i) {
        int* groupId = new int(i);
        pthread_create(&groupLeaders[i], NULL, groupLeader, static_cast<void*>(groupId));
    }

    // Join all student threads
    for (int i = 0; i < N; ++i) {
        pthread_join(students[i], NULL);
    }

    // Join all group leader threads
    for (int i = 0; i < NUM_GROUPS; ++i) {
        pthread_join(groupLeaders[i], NULL);
    }

    // Destroy the semaphores
    for (int i = 0; i < NUM_PRINTERS; ++i) {
        sem_destroy(&printerAvailable[i]);
    }
    for (int i = 0; i < NUM_GROUPS; ++i) {
        sem_destroy(&groupSemaphores[i]);
    }

    return 0;
}
