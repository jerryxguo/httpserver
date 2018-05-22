
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "constant.h"
#include "log.h"
#include "http_server.h"

#ifdef DEBUG_VERSION
#warning compiling DEBUG_VERSION
#define DEBUG(_X_,_ARGS_...) printf("[Main]"_X_,## _ARGS_)
#else
#define DEBUG(_X_,_ARGS_...) do{}while (0)
#endif



static int  port_number         = DEFAULT_PORT_NUMBER;


/**
 @brief     read the configs from the arg
 @param     argc:    the number of arg
 @param     argv:    arg options
 @return    void
*/ 
void initialize_config(int argc,char *argv[])
{
    int option;
    
    logger(DEBUG,"%s\n", __func__);
    while ((option = getopt(argc,argv,"p:l:v"))!=-1)
    {
        switch (option)
        {
            case 'p':
                port_number = atoi(optarg);
                break;
            case 'l':
                set_log_name(optarg);
                break;
            case 'v':
                set_log_level(optarg);
                break;
            default:
                logger(ERROR,"Please enter the right arguments %s\n", option);
                break;
        }
    }
}

/**
 @brief     the main thread code
 @return    int
*/
int main(int argc,char *argv[])
{    
    initialize_config(argc, argv);
    
    http_server_start(port_number);
    
    return 0;
}

