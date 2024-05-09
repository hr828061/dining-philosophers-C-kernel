#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 10
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define MAX_WAIT 3
#define RUNTIME_SECONDS 60

pthread_mutex_t mutex;
int state[N];
int wait_counts[N];
pthread_t philosophers[N];
sem_t forks[N];
time_t start_time;

int left(int philosopher) {
    return (philosopher + N - 1) % N;
}

int right(int philosopher) {
    return (philosopher + 1) % N;
}

void check(int philosopher) {
    if (state[philosopher] == HUNGRY && state[left(philosopher)] != EATING && state[right(philosopher)] != EATING && wait_counts[philosopher] < MAX_WAIT) {
        state[philosopher] = EATING;
        printf("Philosopher %d acquired forks %d and %d\n", philosopher, left(philosopher), philosopher);
        printf("Philosopher %d is eating\n", philosopher);
        //sem_post(&forks[philosopher]);
    }
}

void take_forks(int philosopher) {
    sem_wait(&forks[philosopher]);
    state[philosopher] = HUNGRY;
    check(philosopher);
    sem_post(&forks[philosopher]);
}

void put_forks(int philosopher) {
    state[philosopher] = THINKING;
    printf("Philosopher %d finished eating, now thinking\n", philosopher);
    wait_counts[left(philosopher)] = 0;
    wait_counts[right(philosopher)] = 0;
    check(left(philosopher));
    check(right(philosopher));
}

void *philosopher(void *arg) {
    int philosopher = *(int *)arg;

    while (time(NULL) < start_time + RUNTIME_SECONDS) {
        printf("Philosopher %d is thinking\n", philosopher);
        sleep(rand() % 5 + 1);

        take_forks(philosopher);
        wait_counts[philosopher]++;
        sleep(rand() % 5 + 1);
        put_forks(philosopher);
    }
}

int main() {
    // srand(time(NULL));


    for (int i = 0; i < N; i++) {
        sem_init(&forks[i], 0, 1);
        state[i] = THINKING;
        wait_counts[i] = 0;
    }

    time(&start_time);

    for (int i = 0; i < N; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, (void *)&i);
        sleep(1);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }


    for (int i = 0; i < N; i++) {
        sem_destroy(&forks[i]);
    }

    return 0;
}