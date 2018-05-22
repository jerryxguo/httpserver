

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#include "log.h"

static log_level_t current_log_no = DEBUG;
static char  log_path[] = "/var/log/";
static char  log_name[PATH_MAX] = "httpserver.log";

static pthread_mutex_t lock;


void set_log_name(const char * name)
{
    if (strlen(name) < PATH_MAX)
    {
        strcpy(log_name,name);
    }
}

void set_log_level(const char * log_level)
{
    if (strstr(log_level,"ERROR") || strstr(log_level,"error"))   
    {
        current_log_no = ERROR;
    }
    else if (strstr(log_level,"WARNING") || strstr(log_level,"warning")) 
    {
        current_log_no = WARNING;
    }
    else if (strstr(log_level,"INFO") || strstr(log_level,"info"))
    {
        current_log_no = INFO;
    }
    else if (strstr(log_level,"DEBUG") || strstr(log_level,"debug"))
    {
        current_log_no = DEBUG;
    }
    else
    {
        current_log_no = DEBUG; 
    }
}

void logger(log_level_t log_level,char * fmt, ...)
{
    FILE *file;
    char pathFile[PATH_MAX];
    time_t now;
    char *time_string = NULL;
    char *nl = NULL;

    if (log_level > current_log_no)
    {        
        return;
    }
    
    pthread_mutex_lock(&lock);

    time ( &now );
    time_string = ctime(&now);
    nl = strrchr(time_string, '\n');
    if (nl)
    {
        *nl = '\0';
    }

    strcpy(pathFile, log_path);
    strcat(pathFile, log_name);
    
    file = fopen(pathFile, "a+"); 
    if (file != NULL) 
    {
        if (current_log_no == DEBUG)
        {
            fprintf(file, "%s - DEBUG:", time_string);
        }
        else if (current_log_no == INFO)
        {
            fprintf(file, "%s - INFO:", time_string); 
        }
        else if (current_log_no == WARNING)
        {
            fprintf(file, "%s WARNING:", time_string); 
        }
        else if (current_log_no == ERROR)
        {
            fprintf(file, "%s ERROR:", time_string); 
        }
    
        va_list myargs;
        va_start(myargs,fmt);
        vfprintf(file,fmt,myargs);
        va_end(myargs);
    }
    fclose(file);
    pthread_mutex_unlock(&lock);
}
