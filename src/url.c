
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "constant.h"
#include "http.h"
#include "cache.h"

#ifdef DEBUG_VERSION
#define DEBUG(_X_,_ARGS_...) printf("[Url]"_X_,## _ARGS_)
#else
#define DEBUG(_X_,_ARGS_...) do {} while (0)
#endif


/**
  url structure definition
*/
typedef struct 
{
    char url[MAX_URL_LENGTH+1];
    data_type_t  type;
    int cache_flag;
} url_t;

static url_t * url_list = NULL;
static int url_count = 0;

void initialize_urls(void)
{

    int i = 0;
    
    url_list = (url_t *)malloc(sizeof(url_t) *MAX_URLS );
    assert(url_list!=NULL );

    //external interface
    url_list[i].type   = TELEMETRY;
    url_list[i].cache_flag   = 1;
    sprintf(url_list[i].url, "/api/site-telemetry");
    i++;

    url_list[i].type   = CHARGING_MODE;    
    url_list[i].cache_flag   = 0;
    sprintf(url_list[i].url, "/api/charging-mode");
    i++;

    url_list[i].type   = REFRESH_MODE;    
    url_list[i].cache_flag   = 0;
    sprintf(url_list[i].url, "/api/battery");
    i++;

    url_list[i].type   = SYSTEM;
    url_list[i].cache_flag   = 0;
    sprintf(url_list[i].url, "/api/firmwareUpdate");
    i++; 

    assert(i<MAX_URLS);
    url_count = i;
}

void free_urls(void)
{
    if (url_list != NULL)
    {
        free(url_list);    
    }
}

int get_data_type(const char *url, int url_length)
{
    int i;
    DEBUG("%s\n", __func__);
    if (url_length <= MAX_URL_LENGTH ){
        for (i=0; i< url_count; i++)
        {
            if (strncmp(url_list[i].url, url, url_length)==0)
            {
                return url_list[i].type;
            }            
        }
    }
    return INVALID;
}

int is_cached_data(const char *url, int url_length)
{
    int i;
    DEBUG("%s\n", __func__);
    if (url_length <= MAX_URL_LENGTH ){
        for (i=0; i< url_count; i++)
        {
            if (strncmp(url_list[i].url, url, url_length)==0)
            {
                return url_list[i].cache_flag;
            }            
        }
    }
    return 0;
}

