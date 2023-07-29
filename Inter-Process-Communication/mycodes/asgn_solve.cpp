// PS -> printing station, BS -> binding station

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <random>

#define N_PS 4
#define N_BS 2
#define timestamp std::chrono::high_resolution_clock::time_point
#define current_time() std::chrono::high_resolution_clock::now()
#define get_seconds(t2, t1) duration_cast<seconds>(t2 - t1).count()

using namespace std;
using namespace std::chrono;

std::random_device rd;          // obtain a random seed from the hardware
std::mt19937 generator(rd());   // Mersenne Twister 19937 engine
double mean_delay = 1.0;        // adjust to control expected delay
poisson_distribution<int> poisson(mean_delay);

timestamp initialTime;
int N, M;   // N -> number of students, M -> size of each group
int w, x, y;    // relative time units for the operations printing, binding and reading/writing respectively
bool* psIsFree;
pthread_mutex_t mutex_ps;
sem_t sem_availablePS;

void printOut(int id, string description, int time) {
    string msg;
    msg = "Student " + to_string(id) + " " + description + " at time " + to_string(time) + "\n";
    cout << msg;
}

void usePS(int id, int time, timestamp lastTime) {
    sem_wait(&sem_availablePS);
    
    pthread_mutex_lock(&mutex_ps);
    int i = (id % 4) + 1;         // student no. 'id' is assigned to the print station no. (id % 4) + 1
    if(psIsFree[i]) psIsFree[i] = false;
    pthread_mutex_unlock(&mutex_ps);

    time += get_seconds(current_time(), lastTime);

    printOut(id, "has started printing at print station " + to_string(i), time);
    sleep(w);
    time += w;
    printOut(id, "has finished printing at print station " + to_string(i), time);

    pthread_mutex_lock(&mutex_ps);
    psIsFree[i] = true;
    pthread_mutex_unlock(&mutex_ps);

    sem_post(&sem_availablePS);

    // checkSecurity(id, isVip, time, current_time());
}

void* arrivePS(void* arg) {
    int id = *((int*)arg);
    delete (int*)arg;
    arg = nullptr;

    timestamp t = current_time();
    int time = get_seconds(t, initialTime);
    int pid = (id % 4) + 1;     //The rule to assign a student to a printer is (student ID mod 4 + 1)
    printOut(id, "has arrived at the print station", time);

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
    sem_init(&sem_availablePS, 0, N_PS);

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

    return 0;
}