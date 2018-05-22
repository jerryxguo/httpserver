#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <limits.h>
#include <assert.h>


#include "constant.h"
#include "log.h"

#include "http.h"
#include "cache.h"
#include "url.h"

#ifdef DEBUG_VERSION
#define DEBUG(_X_,_ARGS_...) printf("[Http Server]"_X_,## _ARGS_)
#else
#define DEBUG(_X_,_ARGS_...) do{}while (0)
#endif

extern http_status_map_t HTTP_STATUS_LOOKUP[];


static sig_atomic_t  status_on = 1;

/**
 @brief     set signal mask ( which signal can be received)
 @param     none
 @return    void
*/
static void set_signal_mask(void)
{
    static sigset_t   signal_mask;
    sigemptyset(&signal_mask);
    sigaddset (&signal_mask, SIGINT);
    sigaddset (&signal_mask, SIGTERM);
    pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
}

/**
 @brief     construct and send the response
 @param     fd:  socket file descriptor
 @param     request:  the http request
 @return    void
*/
static void send_response(int fd, const http_request_t *request)
{
    char buffer[MAX_BUFFER_LENGTH+1];
    time_t now=0;
    struct tm *t;
    FILE   *fp = fdopen(dup(fd),"w");
    DEBUG("%s\n", __func__);
    memset(buffer, 0, MAX_BUFFER_LENGTH+1);

    now = time(NULL);
    t = gmtime(&now);
    strftime(buffer,30,"%a, %d %b %Y %H:%M:%S %Z",t);

    fprintf(fp,"HTTP/%d.%d %d %s\r\n",request->major_version,request->minor_version, HTTP_STATUS_LOOKUP[request->status].code, HTTP_STATUS_LOOKUP[request->status].reason);        

    fprintf(fp,"%s: %s\r\n", "Date", buffer);
    fprintf(fp,"%s: %s\r\n", "Server", "ESS Controller HTTP");
    fprintf(fp,"%s: %s\r\n", "Content-Type", "application/json; charset=UTF-8");
    
    if (HTTP_STATUS_OK == request->status && INVALID != request->type)
    {

        if (HTTP_METHOD_GET == request->method)
        {
            data_buffer_t data_buffer;
            memset(&data_buffer, 0, sizeof(data_buffer_t));
            
            if (SUCCESS == cache_get(request->type, &data_buffer))
            {
                DEBUG("cache_get SUCCESS\n");
                if (!is_cached_data(request->url, strlen(request->url)))
                {
                    cache_remove(request->type);   
                }
                fprintf(fp,"%s: %d\r\n", "Content-Length", (int)strlen(data_buffer.data));
                fprintf(fp,"\r\n");
                fprintf(fp, data_buffer.data);
                fprintf(fp,"\n");
            }
            else
            {
                DEBUG("cache_get FAILURE\n");
                char result[] = "{\"result\":\"no data available\"}";
                fprintf(fp,"%s: %d\r\n", "Content-Length", (int)sizeof(result));
                fprintf(fp,"\r\n");
                fprintf(fp, result);
                fprintf(fp,"\n");
            }
            if (NULL != data_buffer.data)
            {
                DEBUG("data_buffer.data = %s\n", data_buffer.data);
                free(data_buffer.data);
            }
                
        }
        else
        {
            char result[] = "{\"result\":\"success\"}";
            fprintf(fp,"%s: %d\r\n", "Content-Length", (int)sizeof(result));
            fprintf(fp,"\r\n");
            fprintf(fp, result);
            fprintf(fp,"\n");
        }
    }
    else
    {
        char result[] = "{\"result\":\"bad request\"}";        
        fprintf(fp,"%s: %d\r\n", "Content-Length", (int)sizeof(result));
        fprintf(fp,"\r\n");
        fprintf(fp, result);
        fprintf(fp,"\n");
    }
    DEBUG("status:%d, reason:%s\n", request->status, HTTP_STATUS_LOOKUP[request->status].reason);
    fclose(fp);
}

/**
 @brief     parse the request
 @param     fd:  socket file descriptor
 @param     request: http request
 @return    void
*/
static void parse_request(int sockfd, http_request_t *request)
{
    char   method  [MAX_METHODS+1];
    char   location[MAX_URL_LENGTH+1];
    int    minor_version;
    int    major_version;
    char *data = NULL;
    int data_length = 0;
    struct pollfd fd;
    
    memset(method, 0, MAX_METHODS+1);
    memset(location, 0, MAX_URL_LENGTH+1);
    DEBUG("%s\n", __func__);
    fd.fd = sockfd;
    fd.events = POLLIN;
    while (1)
    {
        char   buffer  [MAX_BUFFER_LENGTH+1];
        int ret = poll(&fd, 1, 1000); // 1 second for timeout
        if (ret > 0) 
        {
            int size = recv(sockfd, buffer, MAX_BUFFER_LENGTH, 0);
            if (size>0)
            {
                int pre_size = data_length;
                buffer[size] = '\0';            
                data_length += size;    
                data = (char *) realloc(data, data_length+1);
                data[pre_size] = '\0';
                strcat(data, buffer);
                data[data_length] = '\0';
                
                DEBUG("\nsize = %d\n", size);
                DEBUG("data_length = %d\n", data_length);
                DEBUG("buffer = %s\n", buffer);
            }
            else
            {
                logger(DEBUG, "size = %d\n", size);
                break;
            }
        }              
        else
        {
            logger(DEBUG, "poll ret = %d\n", ret);
            break;
        }
    }

    if (data!=NULL)
    {
        DEBUG("data is %s\n",data);
        sscanf(data, "%s %s HTTP/%d.%d",method, location, &major_version, &minor_version);
                
        //remove hash part
        if (strstr(location,"#")) 
        {
            strcpy(strpbrk(location,"#"),"");
        }
        //remove domain part
        if (strstr(location,"?")) 
        {
            strcpy(strpbrk(location,"?"),"");
        }
        //save the url
        strcpy(request->url, location);
        request->major_version = major_version;
        request->minor_version = minor_version;
        request->type = get_data_type(request->url, strlen(request->url));

        DEBUG("method %s\n", method);
        DEBUG("location %s\n", location);
        DEBUG("minor_version %d\n", minor_version);        
        DEBUG("type %d\n", request->type);     
        
        if (strcmp(method,"GET")==0)
        {
            request->method = HTTP_METHOD_GET;
            request->status = HTTP_STATUS_OK;
            
        }
        else if (strcmp(method,"PUT")==0)
        {
            char * payload = strstr(data, "\r\n\r\n");
            request->method = HTTP_METHOD_PUT;
            request->status = HTTP_STATUS_OK;    
            if (NULL != payload)
            {
                cache_add(payload + 4, request->type);
            }
        }
        else if (strcmp(method,"POST")==0)
        {
            char * payload = strstr(data, "\r\n\r\n");
            DEBUG("HTTP_METHOD_POST\n");
            request->method = HTTP_METHOD_POST;
            request->status = HTTP_STATUS_OK;
            if (NULL != payload)
            {
                cache_add(payload + 4, request->type);
            }
        }
        else
        {
            DEBUG("HTTP_METHOD_NOT_SUPPORTED\n");
            request->method = HTTP_METHOD_NOT_SUPPORTED;
            request->status = HTTP_STATUS_NOT_IMPLEMENTED;
        }        
        
        logger(DEBUG, "method = %s\n", method);
        logger(DEBUG, "url = %s\n", request->url);
    
        free(data);
    }
    else
    {
        logger(ERROR,"sockfd(%d) retrieve data fails\n", sockfd);
        request->method = HTTP_INVALID;
        request->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
    }
}


/**
 @brief     shut down the app
 @param     peer_sfd:  the pointer to socket file descriptor
 @return    void
*/
static void* manage_request_response_thread(void *peer_sfd)
{
    http_request_t  *request  = (http_request_t*)malloc(sizeof(http_request_t));

    DEBUG("%s\n", __func__);

    set_signal_mask();

    parse_request(*(int*)peer_sfd, request);
    send_response(*(int*)peer_sfd, request);

    free(request);    
    close(*(int*)peer_sfd);
    free((int*)peer_sfd);
    pthread_exit(pthread_self);
}

/**
 @brief     shut down the app
 @param     sig: signal id
 @return    void
*/
void graceful_shutdown(int sig)
{
    status_on = 0;
}


/**
 @brief     config the signal process handlers
 @return    void
*/
static void configure_signal_handle(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags   = 0;

    sa.sa_handler = graceful_shutdown;
    if (sigaction(SIGINT,  &sa, NULL) == -1)  
    {
        logger(ERROR,"SIGINT sigaction\n");
        exit(1);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        logger(ERROR,"SIGTERM sigaction\n");
        exit(1);
    }
    if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
        logger(ERROR,"SIGCHLD sigaction\n");
        exit(1);
    }
}


/**
 @brief     set up the http server
 @param     port:  port number to listen
 @return    socket file descriptor (int type)
*/ 
static int initialize_server(int port)
{
    
    struct sockaddr_in myaddr;
    int  sfd; 
    int  optval = 1;

    DEBUG("%s\n", __func__);

    sfd = socket(AF_INET,SOCK_STREAM,0);    //creating socket

    if (sfd == -1)
    {
        logger(ERROR,"socket\n");
        exit(0);
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {        
        logger(WARNING,"setsockopt\n");
    }

    memset(&myaddr,0,sizeof(myaddr));
    myaddr.sin_family      = AF_INET;
    myaddr.sin_port        = htons(port);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd, (struct sockaddr*) &myaddr, sizeof(myaddr)) == -1)  
    {    
        logger(ERROR,"bind failed\n");
        exit(0);
    }
    if (listen(sfd,BACKLOG)==-1)
    {                        
        logger(WARNING,"listen FAILED\n");
    }
    return sfd;
}

void http_server_start(int port)
{
    int sfd ;
    pthread_t tid;
    int *peer_sfd = NULL;
    logger (DEBUG, "Server start....\n" );

    configure_signal_handle();
    initialize_urls();
    
    //start server    
    sfd  = initialize_server(port);

    while (status_on)                                                        
    {
        peer_sfd = (int *)malloc(sizeof(int));
        *peer_sfd = accept(sfd,NULL,NULL);
        if (*peer_sfd == -1)
        {
            logger(WARNING,"Interrupt\n");
            continue;
        }
        DEBUG("---------------------------------------------------------- accept(%d)\n",*peer_sfd);

        
        pthread_create(&tid, NULL, manage_request_response_thread, peer_sfd);
        pthread_detach(tid);
    }    

    //close server
    if (close(sfd)==-1)
    {
        logger(WARNING,"Error while closing\n");
    }
    if (peer_sfd!=NULL)
    {
        free(peer_sfd);
    }
    free_urls();
    cache_destroy();
    logger (DEBUG, "Server Shutdown....\n");
}
