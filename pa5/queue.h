#ifndef QUEUE_H
#define QUEUE_H

#include "node.h"
#include <pthread.h>

typedef struct queueT {
 
    pthread_cond_t open;
    pthread_mutex_t mut;
    node *end;
} queue;

void enqueue(queue *, void *);
queue *create_queue(void);

void destroy_queue(queue *, void (*)(void *));

void *dequeue(queue *);


const void *peek(queue *);

int isempty(queue *);
#endif

