
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h> 
#include <assert.h>
#include <pthread.h>


#include "cache.h"

#ifdef DEBUG_VERSION
#define DEBUG(_X_,_ARGS_...) printf("[Cache]"_X_,## _ARGS_)
#else
#define DEBUG(_X_,_ARGS_...) do {} while (0)
#endif


static data_buffer_t * bufferHead = NULL;
static pthread_mutex_t lock;

void cache_add(char *data, data_type_t type)
{
    
    data_buffer_t * pCurr = bufferHead;
    data_buffer_t * pPre = bufferHead;
    int len = strlen(data);

    pthread_mutex_lock(&lock);
    DEBUG("%s\n", __func__);

    //remove existing data
    while (NULL != pCurr)
    {
        if (pCurr->type == type)
        {
            if (pCurr == pPre)
            {
                bufferHead = bufferHead->pNext;
            }
            else
            {
                pPre->pNext = pCurr->pNext;
            }
            free(pCurr->data);
            free(pCurr);
            break;
        }
        pPre = pCurr;
        pCurr = pCurr->pNext;
    }
    
    //add new data
    data_buffer_t *buffer = (data_buffer_t *)malloc(sizeof(data_buffer_t));       
    assert(buffer!=NULL);
    memset(buffer, 0, sizeof(data_buffer_t));
    
    buffer->data = (char*) malloc(len+1);
    assert(buffer->data!=NULL);
    memset(buffer->data, 0, len+1);
    
    buffer->pNext = NULL;
    strcpy(buffer->data, data);
    buffer->data[len]='\0';
    buffer->timestamp = time(NULL);
    buffer->type = type;

    DEBUG("cache data %s\n", data);

    if (bufferHead==NULL)
    {
        bufferHead = buffer;        
    }
    else
    {
        data_buffer_t * pCurr  = bufferHead;
        bufferHead = buffer;
        bufferHead->pNext = pCurr;
    }
    
    pthread_mutex_unlock(&lock);
}


void cache_remove(data_type_t type)
{
    
    data_buffer_t * pCurr = bufferHead;
    data_buffer_t * pPre = bufferHead;
    
    pthread_mutex_lock(&lock);
    DEBUG("%s\n", __func__);

    //remove existing data
    while (NULL != pCurr)
    {
        if (pCurr->type == type)
        {
            if (pCurr == pPre)
            {
                bufferHead = bufferHead->pNext;
            }
            else
            {
                pPre->pNext = pCurr->pNext;
            }
            free(pCurr->data);
            free(pCurr);
            break;
        }
        pPre = pCurr;
        pCurr = pCurr->pNext;
    }
    pthread_mutex_unlock(&lock);
}


int cache_get(data_type_t type, data_buffer_t *buffer)
{
    data_buffer_t * pCurr = bufferHead;
    int ret = FAILURE;

    pthread_mutex_lock(&lock);

    DEBUG("%s\n", __func__);
    
    //remove existing data
    while (NULL != pCurr)
    {        
        if (pCurr->type == type)
        {
            int len = strlen(pCurr->data);
            buffer->data = (char*) malloc(len+1);
            assert(buffer->data!=NULL);
            memset(buffer->data, 0, len+1);
            
            DEBUG("retrieve data %s\n", pCurr->data);
            strcpy(buffer->data, pCurr->data);
            buffer->data[len]='\0';
             buffer->timestamp = pCurr->timestamp;
            ret = SUCCESS;
            break;
        }        
        pCurr = pCurr->pNext;
    }    
    pthread_mutex_unlock(&lock);
    return ret;
}


void cache_destroy(void)
{
    data_buffer_t * pCurr = bufferHead;
    pthread_mutex_lock(&lock);
    DEBUG("%s\n", __func__);

    //remove existing data
    while (NULL != pCurr)
    {        
        data_buffer_t *pPre = pCurr;
        pCurr = pCurr->pNext;

        free(pPre->data);
        free(pPre);
    }
    pthread_mutex_unlock(&lock);
}
