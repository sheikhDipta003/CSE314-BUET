// PS -> printing station, BS -> binding station

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <random>

#define N_PS 4
#define N_BS 2
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
int N, M;       // N -> number of students, M -> size of each group
int w, x, y;    // relative time units for the operations- printing, binding and reading/writing respectively
int readersCount = 0;
int submissionCount = 0;
bool* psIsFree;
bool* bsIsFree;
pthread_mutex_t mutex_ps;
pthread_mutex_t mutex_bs;
pthread_mutex_t entry_book;
pthread_mutex_t reader_count;
pthread_mutex_t sub_count;
sem_t sem_availablePS;
sem_t sem_availableBS;

void printOut(int id, string agent, string description, int time) {
    cout << agent << " " << id << " " << description << " at time " << time << endl;
}

// Function for readers
void readEntryBook(int id, int time, timestamp lastTime) {
    pthread_mutex_lock(&reader_count);
    ++readersCount;
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
    --readersCount;
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

        int delay = poisson(generator);   // delay -> amount of time interval between successive reading of 2 staffs
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

void* writehelp(void* arg) {
    int id = *((int*)arg);      // group-id
    delete (int*)arg;
    arg = nullptr;

    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    writeEntryBook(id, time, current_time());

    return NULL;
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

void* arriveBS(void* arg) {
    int id = *((int*)arg);      // group-id
    delete (int*)arg;
    arg = nullptr;

    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    useBS(id, time, current_time());

    return NULL;
}

void usePS(int id, int time, timestamp lastTime) {
    sem_wait(&sem_availablePS);
    
    pthread_mutex_lock(&mutex_ps);
    int i = (id % N_PS);         // student no. 'id' is assigned to the print station no. (id % N_PS) + 1; but array index starts from 0
    if(psIsFree[i]) psIsFree[i] = false;
    pthread_mutex_unlock(&mutex_ps);

    time += get_seconds(current_time(), lastTime);

    printOut(id, "Student", "has started printing at PS " + to_string(i+1), time);
    sleep(w);
    time += w;
    printOut(id, "Student", "has finished printing at PS " + to_string(i+1), time);

    pthread_mutex_lock(&mutex_ps);
    psIsFree[i] = true;
    pthread_mutex_unlock(&mutex_ps);

    sem_post(&sem_availablePS);
}

void* arrivePS(void* arg) {
    int id = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    int pid = (id % N_PS);     // student no. 'id' is assigned to the print station no. (id % N_PS) + 1; but array index starts from 0
    printOut(id, "Student", "has arrived at PS " + to_string(pid+1), time);

    usePS(id, time, current_time());

    return NULL;
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    cin >> N >> M;
    cin >> w >> x >> y;

    psIsFree = new bool[N_PS];
    fill(psIsFree, psIsFree + N_PS, true);
    bsIsFree = new bool[N_BS];
    fill(bsIsFree, bsIsFree + N_BS, true);

    pthread_mutex_init(&mutex_ps, NULL);
    sem_init(&sem_availablePS, 0, N_PS);
    pthread_mutex_init(&mutex_bs, NULL);
    sem_init(&sem_availableBS, 0, N_BS);
    pthread_mutex_init(&entry_book, NULL);
    pthread_mutex_init(&reader_count, NULL);
    pthread_mutex_init(&sub_count, NULL);

    pthread_t reader;
    int r = rand() % N_r;
    int* id = new int(r + 1);
    pthread_create(&reader, NULL, readhelp, (void*)id);

    // printing phase
    int sid = 0;                    //  Total count of students for uniquely identifying
    initialTime = current_time();   //  Initial timestamp to determine program startup time (GLOBAL)
    pthread_t student[N];

    for (int i=0; i < N; i++) {
        int* id = new int(sid + 1);
        pthread_create(&student[i], NULL, arrivePS, (void*)id);
        sid++;

        int delay = poisson(generator);   // delay -> amount of time interval between successive arrival of 2 students
        sleep(delay);
    }
    //

    // binding phase
    int c = N / M;
    pthread_t group[c];

    for(int i = 0; i < c; i++){
        for (int j = i*M; j < (i+1)*M ; j++) {
            pthread_join(student[j], NULL);
        }

        int* id = new int(i+1);
        pthread_create(&group[i], NULL, arriveBS, (void*)id);
    }
    //

    // submission phase
    for(int i = 0; i < c; i++){
        pthread_join(group[i], NULL);
        int* id = new int(i+1);
        pthread_create(&group[i], NULL, writehelp, (void*)id);
    }
    //

    for (int i = 0; i < c; i++) {
        pthread_join(group[i], NULL);
    }

    pthread_cancel(reader);

    return 0;
}