#include <stdio.h>
#include <pthread.h>
#include "queue.c"
#include "pthread_sleep.c"
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
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
const char* getTimestamp();

pthread_t mod;
pthread_t *coms;
pthread_t currentCom;
pthread_mutex_t mutex;
pthread_cond_t cond;

int n;
int q;
double t;
double p;
int currentQ = 0;
int questionActive = false;
struct timeval start;

Queue answerQueue;

int main(int argc, char *argv[]) {

    printf("argc: %d\n", argc);

    for (int i = 0; i < argc; i++)
    {
        printf("%s |", argv[i]);
    }
    printf("\n");
    
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-n") == 0) {
            n = atoi(argv[i + 1]);
        } 
        else if (strcmp(argv[i], "-q") == 0) {
            q = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-t") == 0) {
            t = atof(argv[i + 1]);
        } 
        else if (strcmp(argv[i], "-p") == 0) {
            p = atof(argv[i + 1]);
        }
    }
    
    init();
}

void init () {  
    coms = (pthread_t*) malloc(n * sizeof(pthread_t));  
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

    gettimeofday(&start, NULL);

    for(int i = 0; i < n; i++) {
        pthread_join(coms[i], NULL);
    }

    pthread_join(mod, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void *moderate(void *arg) {
    while (q >= 0) {
        pthread_mutex_lock(&mutex);
        if (isEmpty(&answerQueue) && q == 0) {
            q = -1;
            questionActive = false;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
            break;
        }
        pthread_mutex_unlock(&mutex);
        pthread_sleep(1);
        
        pthread_mutex_lock(&mutex);
        if (!questionActive) {
            currentQ++;
            askQuestion();
            questionActive = true;
        } else
            currentCom = dequeue(&answerQueue);
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
        pthread_sleep(1);
        pthread_mutex_lock(&mutex);
        if (isEmpty(&answerQueue) && currentCom == NULL) {
            questionActive = false;
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void *commentate(void *arg) {
    int didAnswerActiveQuestion = false;
    while (q >= 0) {
        // this block is for checking if we're done
        if (isEmpty(&answerQueue) && q < 0) {
            pthread_cond_broadcast(&cond);
            break;
        }
        pthread_mutex_lock(&mutex);
        // wait
        while (!questionActive || didAnswerActiveQuestion && currentCom != pthread_self()) {
            pthread_cond_wait(&cond,&mutex);
            // reset
            if (!questionActive && didAnswerActiveQuestion)
                didAnswerActiveQuestion = false;
            if (q < 0)
                break;
        }

        if (!didAnswerActiveQuestion && ((double) rand() / (double) RAND_MAX) <= p && q >= 0) {
            printf("[%s] Commentator #%d generates answer, position in queue: %d\n", getTimestamp(), indexOf(pthread_self()), (&answerQueue)->size + 1);
            enqueue(&answerQueue, pthread_self());
        }

        didAnswerActiveQuestion = true;

        if(currentCom == pthread_self()) {
            printf("[%s] Commentator #%d's turn to speak for %f seconds. \n", getTimestamp(), indexOf(pthread_self()),t_speak());
            pthread_sleep(t_speak());
            
            printf("[%s] Commentator #%d finished speaking.\n", getTimestamp(), indexOf(pthread_self()));
            currentCom = NULL;
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void askQuestion() {
    printf("[%s] Moderator asks question #%d.\n", getTimestamp(), currentQ);
    q--;
}

int indexOf(pthread_t thread) {
    for(int i=0; i<n; i++)
        if(thread == coms[i]) return i;
    return -1;
}

double t_speak() {
    return (t - 1) * ((double) rand() / (double) RAND_MAX) + 1;
}

const char* getTimestamp() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    
    long mins = (currentTime.tv_sec - start.tv_sec) / 60;
    long secs = (currentTime.tv_sec - start.tv_sec) % 60;
    uint64_t millis = ((currentTime.tv_sec - start.tv_sec) * (uint64_t) 100000) + ((currentTime.tv_usec - start.tv_usec) / 1000);
    char *time = (char*) malloc(10 * sizeof(char));
    sprintf(time, "%02ld:%02ld.%llu", mins, secs, millis);
    return time;
}