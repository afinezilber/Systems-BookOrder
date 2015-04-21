#include <stdlib.h>
#include <pthread.h>
#include "node.h"
#include "queue.h"

void enqueue(queue *queueThread, void *data) 
{
    node *nodeThread;
    if (!queueThread)
        return;

    nodeThread = create_node(data, NULL);
    if (queueThread->end == NULL) {

        queueThread->end = nodeThread;
        queueThread->end->next = queueThread->end;
    }
    else {
   
        nodeThread->next = queueThread->end->next;
        queueThread->end->next = nodeThread;
        queueThread->end = nodeThread;
    }
}

queue *create_queue(void) 
{
    queue *t = (queue*) malloc(sizeof(queue));
    if (t) {
        t->end = NULL;
        if (pthread_mutex_init(&t->mut, NULL) != 0) {
            free(t);
            t = NULL;
        }
        else if (pthread_cond_init(&t->open, NULL) != 0) {
            pthread_mutex_destroy(&t->mut);
            free(t);
            t = NULL;
        }
    }
    return t;
}


void destroy_queue(queue *queueThread, void (*destroy_func)(void *))
{
    node *nodeThread, *next;
    if (queueThread) 
    {
		pthread_cond_destroy(&queueThread->open);
        pthread_mutex_destroy(&queueThread->mut);

        if (!queueThread->end) 
        {
          
        }
        else if (queueThread->end == queueThread->end->next) 
        {
           
            destroy_node(queueThread->end);
        }
        else 
        {
            
            nodeThread = queueThread->end;
            while(nodeThread) {
                next = nodeThread->next;
                if (destroy_func) {
                    destroy_func(nodeThread->data);
                }
                destroy_node(nodeThread);
                nodeThread = next;
            }
        }
        free(queueThread);
    }
}

void *dequeue(queue *queueThread) 
{
    void *data;
    node *destroy;

    if (queueThread == NULL || queueThread->end == NULL) 
    {
        return NULL;
    }        


    if (!queueThread->end->next || queueThread->end == queueThread->end->next) 
    {

        data = queueThread->end->data;
        destroy_node(queueThread->end);
        queueThread->end = NULL;
    }
    else 
    {
    
        destroy = queueThread->end->next;
        data = queueThread->end->next->data;
        queueThread->end->next = queueThread->end->next->next;
        destroy_node(destroy);
    }
    return data;
}



const void *peek(queue *queueThread)
{
    if (queueThread && queueThread->end && queueThread->end->next) 
    {
        return queueThread->end->next->data;
    }
    else
    {
        return NULL;
    }
}

int isempty(queue *queueThread)
{
    return queueThread == NULL || queueThread->end == NULL;
}
