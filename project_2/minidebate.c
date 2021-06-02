#include <stdio.h>
#include <pthread.h>
#include "queue.c"
#include "pthread_sleep.c"
#include <time.h>
#include <stdlib.h>
#include <string.h>

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
double t_speak();

pthread_t mod;
pthread_t coms[N];
pthread_t currentCom;
pthread_mutex_t mutex;
pthread_cond_t cond;

int n;
int q;
double t;
double p;

Queue answerQueue;

int main(int argc, char *argv[]) {
    n = N;
    q = Q;
    t = T;
    p = P;

    printf("argc: %d\n", argc);

    for (int i = 0; i < argc; i++)
    {
        printf("%s|", argv[i]);
    }
    printf("\n");

    printf("entering loop\n");
    for (int i = 1; i < argc; i++)
    {
        printf("in loop\n");
        if (strcmp(argv[i], "-n") == 0) {
            printf("comparing\n ");
            n = atoi(argv[i + 1]);
            i++;
        }
/* 
        else if (strcmp(argv[i], "-q") == 0) {
            printf("%s: %s", argv[i], argv[i + 1]);
            if (argv[++i] != NULL)
                q = atoi(argv[i]);
        }

        else if (strcmp(argv[i], "-t") == 0) {
            printf("%s: %s", argv[i], argv[i + 1]);
            if (argv[++i] != NULL)
                t = strtod(argv[i], NULL);
        }
        
        else if (strcmp(argv[i], "-p") == 0) {
            printf("%s: %s", argv[i], argv[i + 1]);
            if (argv[++i] != NULL)
                p = strtod(argv[i], NULL);
        }    */
    }
    
    init();
}

void init () {    
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
            pthread_sleep(t_speak());
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
        if (p * ((double) rand() / (double) RAND_MAX) <= 100) {
            printf("Commentator #%d will answer.\n", i);
            enqueue(&answerQueue, coms[i]);
        }
    }
}

int indexOf(pthread_t thread) {
    for(int i=0; i<n; i++)
        if(thread == coms[i]) return i;
    return -1;
}

double t_speak() {
    return t * ((double) rand() / (double) RAND_MAX);
}