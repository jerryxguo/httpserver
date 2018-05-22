

#ifndef DATA_CACHE
#define DATA_CACHE

enum{
    SUCCESS = 0,
    FAILURE = -1,
};


/**
 * data type definition
 */
typedef enum
{
    TELEMETRY = 10,
    CHARGING_MODE,
    REFRESH_MODE,
    SYSTEM,
    INVALID
} data_type_t;

/**
 * generic structure for data cache buffer
 */
typedef struct data_buffer
{
    data_type_t type;
    char * data;
    time_t timestamp;
    struct data_buffer * pNext;
} data_buffer_t;


/**
 @brief     add the data to cache or update the cached data
 @param     data    the data string
 @param     type    the data type
 @return    void
*/ 
extern void cache_add(char * data, data_type_t type);

/**
 @brief     remove the buffered data by data type
 @param     type    the data type
 @return    void
*/
extern void cache_remove(data_type_t type);

/**
 @brief     get the buffered data by data type
 @param     type    the data type
 @param     buffer  the buffer pointer for retrieving the data
 @return    SUCCESS or FAILURE
*/
extern int cache_get(data_type_t type, data_buffer_t * buffer);

/**
 @brief     clean up cached data
 @return    void
*/
extern void cache_destroy(void);

#endif
