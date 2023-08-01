#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <random>

#define N_PS 4  // number of printing station
#define N_BS 2  // number of binding station
#define N_r 2   // number of readers
#define timestamp std::chrono::high_resolution_clock::time_point
#define current_time() std::chrono::high_resolution_clock::now()
#define get_seconds(t2, t1) duration_cast<seconds>(t2 - t1).count()

using namespace std;
using namespace std::chrono;

std::random_device rd;          // obtain a random seed from the hardware
std::mt19937 generator(rd());   // Mersenne Twister 19937 engine
double mean_delay = 1.6;        // adjust to control expected delay
poisson_distribution<int> poisson(mean_delay);

timestamp initialTime;

int N, M;                       // N -> number of students, M -> size of each group
int w, x, y;                    // relative time units for the operations- printing, binding and reading/writing respectively
int readersCount = 0;
int submissionCount = 0;
bool* bsIsFree;

pthread_mutex_t mutex_bs;
pthread_mutex_t entry_book;
pthread_mutex_t reader_count;
pthread_mutex_t sub_count;
pthread_mutex_t outputMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printMutex[N_PS] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
                                            PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
sem_t printerAvailable[N_PS];
sem_t sem_availablePS;
sem_t sem_availableBS;

void printOut(int id, string agent, string description, int time) {
    cout << agent << " " << id << " " << description << " at time " << time << endl;
}

// Function for readers
void readEntryBook(int id, int time, timestamp lastTime) {
    pthread_mutex_lock(&reader_count);
    readersCount++;
    if (readersCount == 1) {
        pthread_mutex_lock(&entry_book);
    }
    pthread_mutex_unlock(&reader_count);

    // Reading the entry book
    time += get_seconds(current_time(), lastTime);

    pthread_mutex_lock(&sub_count);
    cout << "Staff" << " " << id << " has started reading the entry book at time " << time << ". No. of submission = " << submissionCount << endl;
    sleep(y);
    time += y;
    pthread_mutex_unlock(&sub_count);

    pthread_mutex_lock(&reader_count);
    readersCount--;
    if (readersCount == 0) {
        pthread_mutex_unlock(&entry_book);
    }
    pthread_mutex_unlock(&reader_count);
}

void* readhelp(void* arg) {
    int id = *((int*)arg);      // staff-id
    delete (int*)arg;
    arg = nullptr;

    while(true){
        timestamp t = current_time();
        int time = get_seconds(t, initialTime);
        readEntryBook(id, time, current_time());

        int delay = poisson(generator);   // delay -> amount of time interval between successive reading of 'N_r' staffs
        sleep(delay);

        id = rand() % N_r + 1;
    }

    return NULL;
}

// Function for writers
void writeEntryBook(int id, int time, timestamp lastTime) {
    pthread_mutex_lock(&entry_book);

    // Writing to the entry book
    time += get_seconds(current_time(), lastTime);

    pthread_mutex_lock(&sub_count);
    sleep(y);
    time += y;
    cout << "Group" << " " << id << " has submitted the report at time " << time << endl;
    submissionCount++;
    pthread_mutex_unlock(&sub_count);

    pthread_mutex_unlock(&entry_book);
}

int findFreeBS() {
    for (int i = 0; i < N_BS; i++) {
        if (bsIsFree[i]) {
            return i;
        }
    }
    return -1;
}

void useBS(int id, int time, timestamp lastTime) {  // id->group-id
    sem_wait(&sem_availableBS);
    
    pthread_mutex_lock(&mutex_bs);
    int i = findFreeBS();
    bsIsFree[i] = false;
    pthread_mutex_unlock(&mutex_bs);

    time += get_seconds(current_time(), lastTime);

    printOut(id, "Group", "has started binding at BS " + to_string(i+1), time);
    sleep(x);
    time += x;
    printOut(id, "Group", "has finished binding at BS " + to_string(i+1), time);

    pthread_mutex_lock(&mutex_bs);
    bsIsFree[i] = true;
    pthread_mutex_unlock(&mutex_bs);

    sem_post(&sem_availableBS);
}

void* arrivePS(void* arg) {
    int id = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    int time = get_seconds(current_time(), initialTime);
    timestamp lastTime = current_time();
    int pid = (id - 1) % N_PS;
    int gid = (id - 1) / M;         // Calculate the group number for the student

    pthread_mutex_lock(&outputMutex);
    cout << "Student " << id << " has arrived at PS " << (pid+1) << " at time " << time << endl;
    pthread_mutex_unlock(&outputMutex);

    // Wait for the assigned printer to be available
    sem_wait(&printerAvailable[pid]);

    pthread_mutex_lock(&printMutex[pid]);

    time += get_seconds(current_time(), lastTime);

    pthread_mutex_lock(&outputMutex);
    cout << "Student " << id << " has started printing at PS " << (pid + 1) << " at time " << time << endl;
    pthread_mutex_unlock(&outputMutex);

    // Simulate printing
    sleep(w);
    time += w;

    pthread_mutex_lock(&outputMutex);
    cout << "Student " << id << " has finished printing at PS "<< (pid + 1) << " at time " << time << endl;
    pthread_mutex_unlock(&outputMutex);

    pthread_mutex_unlock(&printMutex[pid]);

    // Release the assigned printer
    sem_post(&printerAvailable[pid]);

    pthread_exit(NULL);
}

void* foo(void* arg) {
    int j = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    // printing
    pthread_t student[M];
    int sid = (j-1) * M;
    for (int i = 0; i < M; i++) {
        int* id = new int(sid + 1);
        pthread_create(&student[i], NULL, arrivePS, (void*)id);
        sid++;

        int delay = poisson(generator);   // delay -> amount of time interval between successive arrival of 2 students
        sleep(delay);
    }

    for (int i = 0; i < M; i++) {           //wait until every team member finishes printing
        pthread_join(student[i], NULL);
    }
    //

    // binding
    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    useBS(j, time, current_time());
    //

    // submission
    t = current_time();
    time = get_seconds(t, initialTime);
    writeEntryBook(j, time, current_time());
    //

    return NULL;
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    cin >> N >> M;
    cin >> w >> x >> y;

    bsIsFree = new bool[N_BS];
    fill(bsIsFree, bsIsFree + N_BS, true);

    // Initialize the printer semaphores to 1 (all printers are available)
    for (int i = 0; i < N_PS; ++i) {
        sem_init(&printerAvailable[i], 0, 1);
    }

    pthread_mutex_init(&mutex_bs, NULL);
    pthread_mutex_init(&entry_book, NULL);
    pthread_mutex_init(&reader_count, NULL);
    pthread_mutex_init(&sub_count, NULL);

    sem_init(&sem_availablePS, 0, N_PS);
    sem_init(&sem_availableBS, 0, N_BS);

    initialTime = current_time();   //  Initial timestamp to determine program startup time (GLOBAL)

    pthread_t reader;
    int r = rand() % N_r;
    int* id = new int(r + 1);
    pthread_create(&reader, NULL, readhelp, (void*)id);

    int c = N / M;
    pthread_t group[c];

    for(int i = 0; i < c; i++){
        int* id = new int(i+1);
        pthread_create(&group[i], NULL, foo, (void*)id);
    }

    for (int i = 0; i < c; i++) {
        pthread_join(group[i], NULL);
    }

    pthread_cancel(reader);

    return 0;
}