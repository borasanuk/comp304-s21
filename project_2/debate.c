#include <stdio.h>
#include <pthread.h>
#include "queue.c"
#include "pthread_sleep.c"
#include <time.h>
#include <stdlib.h>

#define P 1
#define T 1
#define Q 5
#define N 1
#define SEED 0
 
void init ();
void *moderate (void *dummy);
void askQuestion();
void *participate(void *dummy);
void chooseCommentatorsToAnswer();

float p;
double t;
int q;
int n;
pthread_t mod;
pthread_t coms[N];
pthread_mutex_t lock;
Queue answerQueue;

int main() {
    init();
}

/* initialize
- create threads
- initialize lock
*/
void init () {
    p = P;
    t = T;
    q = Q;
    n = N;
    srand(SEED);
    answerQueue = *createQueue(n);
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
    }
    
    pthread_mutex_lock(&lock);

    // create mod thread
    pthread_create(&mod, NULL, moderate, NULL);

    // create com thread
    for(int i = 0; i < n; i++) {
        pthread_create(&coms[i], NULL, participate, NULL);
    }

    pthread_mutex_unlock(&lock);
}

/* main loop */
void *moderate (void *dummy) {
    pthread_mutex_lock(&lock);
    while (q != 0) {
        askQuestion();
        pthread_mutex_unlock(&lock);
        while (!isEmpty(&answerQueue)) printf("mod waiting\n");
        pthread_mutex_lock(&lock);
    }
    
    //join threads when the questions end
    for(int i = 0; i < n; i++) {
        pthread_join(coms[i], NULL);
    }
    pthread_join(mod, NULL);
    pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
}

/* ask a question:
- decrement q
*/
void askQuestion() {
    q--;
    chooseCommentatorsToAnswer();
}

/* answer a question
- make com speak (sleep)
*/
void *participate(void *dummy) {
    pthread_mutex_lock(&lock);
    pthread_sleep(t);
    pthread_mutex_unlock(&lock);
}

/*
- add some coms to queue
*/
void chooseCommentatorsToAnswer() {
    for (int i = 0; i < n; i++) {
        if (rand() % 99 < p * 100) {
            enqueue(&answerQueue, coms[i]);
        }
    }
}