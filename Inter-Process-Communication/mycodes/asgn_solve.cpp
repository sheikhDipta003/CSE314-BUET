// PS -> printing station, BS -> binding station

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <random>

#define N_PS 3
#define N_BS 2
#define INACTIVE 0
#define WAITING 1
#define USING 2

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
int N, M;   // N -> number of students, M -> size of each group
int w, x, y;    // relative time units for the operations printing, binding and reading/writing respectively
bool* psIsFree;
pthread_mutex_t mutex_ps;
sem_t* sem_ps;
int* state;

void printOut(int id, string description, int time) {
    string msg;
    msg = "Student " + to_string(id) + " " + description + " at time " + to_string(time) + "\n";
    cout << msg;
}

void test(int id, int pid){
    if(state[id]==WAITING && psIsFree[pid]){
        state[id] = USING;
        psIsFree[pid] = false;
        sem_post(&sem_ps[id]);
    }
}

void take_PS(int id){
    pthread_mutex_lock(&mutex_ps);

    state[id] = WAITING;
    int pid = (id % N_PS) + 1;         // student no. 'id' is assigned to the print station no. (id % N_PS) + 1
    test(id, pid);

    pthread_mutex_unlock(&mutex_ps);
    sem_wait(&sem_ps[id]);
}

void put_PS(int id){
    pthread_mutex_lock(&mutex_ps);

    state[id] = INACTIVE;
    psIsFree[id] = true;

    //notify all teammates and then rest of the students who are waiting for this PS
    int g = (id-1) / M + 1;      // 1 <= g <= (N/M)
    int s = 1 + (g-1) * M;       // starting student-id of group no. 'g'
    int pid = (id % N_PS) + 1;
    for(int i = s; i < s+M; i++){
        if(i != id) test(i, pid);
    }
    for(int i = 1; i <= N; i++){
        if(i < s || i >= s+M) test(i, pid);
    }

    pthread_mutex_unlock(&mutex_ps);
}

void usePS(int id, int time, timestamp lastTime) {    
    take_PS(id);

    time += get_seconds(current_time(), lastTime);
    int i = (id % N_PS) + 1;
    printOut(id, "has started printing at print station " + to_string(i), time);
    sleep(w);
    time += w;
    printOut(id, "has finished printing at print station " + to_string(i), time);

    put_PS(id);
}

void* arrivePS(void* arg) {
    int id = *((int*)arg);

    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    printOut(id, "has arrived at the print station", time);

    state[id] = WAITING;
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

    pthread_mutex_init(&mutex_ps, NULL);
    state = new int[N];
    fill(state, state + N, INACTIVE);
    sem_ps = new sem_t[N];
    for (int i = 0; i < N; i++) {       // Initialize each semaphore in the array
        if (sem_init(&sem_ps[i], 0, 1) != 0) {
            cout << "Semaphore initialization failed for index " << i << std::endl;
            free(sem_ps);
            return 1;
        }
    }

    int ret;                        //  To read the return value from each `pthread` methods
    int sid = 0;                    //  Total count of students for uniquely identifying
    initialTime = current_time();   //  Initial timestamp to determine program startup time (GLOBAL)
    pthread_t student[N];

    for (int i=0; i < N; i++) {
        int* id = new int(sid + 1);
        ret = pthread_create(&student[i], NULL, arrivePS, (void*)id);
        if (ret){
            cout << "Error: pthread_create() failed\n";
            exit(-1);
        }
        sid++;

        int delay = poisson(generator);   // delay -> amount of time interval between successive arrival of 2 students
        sleep(delay);
    }

    for (int i=0; i < N; i++) {
        ret = pthread_join(student[i], NULL);
        if (ret){
            cout << "Error: pthread_join() failed\n";
            exit(-1);
        }
    }

    for (int i = 0; i < N; ++i) {
        sem_destroy(&sem_ps[i]);
    }

    free(sem_ps);
    free(state);

    return 0;
}