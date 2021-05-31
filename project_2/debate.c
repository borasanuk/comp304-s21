#include <stdio.h>
#include <pthread.h>
#include "queue.c"
#include <time.h>
#include <stdlib.h>

#define P 1
#define T 1
#define Q 5
#define N 1
#define SEED 0
 
float p;
double t;
int q;
int n;
pthread_t mod;
pthread_t coms[N];
pthread_mutex_t lock;
Queue answerQueue;

int main() {
    run();
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
        return 1;
    }

    // create mod thread
    pthread_create(&mod, NULL, moderate, NULL);

    // create com thread
    for(int i = 0; i < n; i++) {
        pthread_create(&coms[i], NULL, run, NULL);
    }

}

/* main loop */
void moderate () {
    while (q != 0) {
        askQuestion();
        while (!isEmpty(&answerQueue));
            // give lock to first com.
    }
    for(int i = 0; i < n; i++) {
        pthread_join(&coms[i], NULL);
    }
    pthread_join(&mod, NULL);
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
void participate() {
    pthread_t com = pop(answerQueue);
    // lock
    pthread_sleep(t);
    // unlock    
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

// how to global queue
// how to threads
// how to locks