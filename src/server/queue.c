#include "server/queue.h"
#include <stdlib.h>
#include <stdio.h>

int enQueue(queue_t *pQueue,int fd){
    node_t *pNew = (node_t *)calloc(1,sizeof(node_t));
    if(pNew==NULL){
        return -1;
    }
    pNew->fd=fd;
    if(pQueue->size==0){
        pQueue->head=pNew;
        pQueue->end=pNew;
    }
    else{
        pQueue->end->pNext=pNew;
        pQueue->end=pNew;
    }
    pQueue->size++;
    return 0;
}

int deQueue(queue_t *pQueue){
    if(pQueue->size==0){
        return -1;
    }
    node_t *p=pQueue->head;
    pQueue->head=p->pNext;
    if(pQueue->size==1){
        pQueue->end=NULL;
    }

    pQueue->size--;
    free(p);
    return 0;
}



















