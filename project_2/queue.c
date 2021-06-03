// C program for array implementation of queue
// DISCLAIMER: Retrieved from https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// A structure to represent a queue
struct Queue {
	int front, rear, size;
	unsigned capacity;
	pthread_t* array;
};

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
	struct Queue* queue = (struct Queue*)malloc(
		sizeof(struct Queue));
	queue->capacity = capacity;
	queue->front = queue->size = 0;

	// This is important, see the enqueue
	queue->rear = capacity - 1;
	queue->array = (pthread_t*)malloc(
		queue->capacity * sizeof(pthread_t));
	return queue;
}

typedef struct Queue Queue;

// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
	return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, pthread_t item)
{
	if (isFull(queue))
		return;
	queue->rear = (queue->rear + 1)
				% queue->capacity;
	queue->array[queue->rear] = item;
	queue->size = queue->size + 1;
}

// Function to remove an item from queue.
// It changes front and size
pthread_t dequeue(struct Queue* queue)
{
	if (isEmpty(queue))
		return NULL;
	pthread_t item = queue->array[queue->front];
	queue->front = (queue->front + 1)
				% queue->capacity;
	queue->size = queue->size - 1;
	return item;
}

int contains(struct Queue* queue, pthread_t item)
{
	for (int i = 0; i < queue->size; i++)
		if (queue->array[i] == item) return 1;
	return 0;
}