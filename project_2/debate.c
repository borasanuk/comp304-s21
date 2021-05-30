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

// mutex lock;
 
float p;
int t;
int q;
int n;
pthread_t mod;
pthread_t com;

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
    // create mod thread

    // create com thread
    answerQueue = *createQueue(n);
}

/* main loop */
void run () {
    while (q != 0) {
        askQuestion();
        while (!isEmpty(&answerQueue))
            answerQuestion();
    }
}

/* ask a question:
- decrement q
*/
void askQuestion() {
    q--;
    chooseCommentatorsToAnswer();
}

/* answer a question
- pop a com from queue
- make com speak (sleep)
- 
*/
void answerQuestion() {
    
}

/*
- add some coms to queue
*/
void chooseCommentatorsToAnswer() {
    for (int i = 0; i < n; i++) {
        if (rand() % 99 < p * 100) {
            // add com[i] to queue
        }
    }
}

// how to global queue
// how to threads
// how to locks