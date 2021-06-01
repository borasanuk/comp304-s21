#include <stdio.h>
#include <pthread.h>
#include "queue.c"
#include "pthread_sleep.c"
#include <time.h>
#include <stdlib.h>

#define true 1
#define P 1
#define T 1
#define Q 3
#define N 3
#define SEED 1000

void init ();
void *moderate(void *arg);
void *commentate(void *arg);
void askQuestion();
void chooseCommentatorsToAnswer();
int indexOf(pthread_t thread);

pthread_t mod;
pthread_t coms[N];
pthread_t currentCom;
pthread_mutex_t mutex;
pthread_cond_t cond;


float p;
double t;
int q;
int n;

Queue answerQueue;

int main() {
    init();
}

void init () {    
    p = P;
    t = T;
    q = Q;
    n = N;

    srand(SEED);
    answerQueue = *createQueue(n);

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("\nmutex init has failed\n");
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("\ncondvar init has failed\n");
    }
    if (pthread_create(&mod, NULL, moderate, NULL) != 0){
        perror("\nmod thread init has failed\n");
    }

    for(int i = 0; i < n; i++) {
        pthread_create(&coms[i], NULL, commentate, NULL);
    }

    for(int i = 0; i < n; i++) {
        pthread_join(coms[i], NULL);
    }

    pthread_join(mod, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void *moderate(void *arg) {
    while (q >= 0) {
        if (isEmpty(&answerQueue) && q == 0) {
            q = -1;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
            break;
        } 
        pthread_sleep(t);
        pthread_mutex_lock(&mutex);
        if (isEmpty(&answerQueue) && q > 0)
            askQuestion();
        else
            currentCom = dequeue(&answerQueue);
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
    }
}

void *commentate(void *arg) {
    while (q >= 0) {
        pthread_mutex_lock(&mutex);
        if (isEmpty(&answerQueue) && q <= 0) {
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
            break;
        } 
        while (isEmpty(&answerQueue)) {
            pthread_cond_wait(&cond,&mutex);
        }
        if(currentCom == pthread_self()) {
            pthread_sleep(t);
            printf("Commentator #%d answers question #%d.\n", indexOf(pthread_self()), q + 1);
            currentCom = NULL;
        }
        pthread_mutex_unlock(&mutex);
    }
    
}

void askQuestion() {
    printf("Moderator asks question #%d.\n", q);
    q--;
    chooseCommentatorsToAnswer();
}

void chooseCommentatorsToAnswer() {
    for (int i = 0; i < n; i++) {
        if (rand() % 101 <= p * 100) {
            enqueue(&answerQueue, coms[i]);
        }
    }
}

int indexOf(pthread_t thread) {
    for(int i=0; i<n; i++)
        if(thread == coms[i]) return i;
    return -1;
}