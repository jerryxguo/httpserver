
#ifndef URLS_H
#define URLS_H

/**
 @brief     PRE-set the URLs suported by this http server
 @return    void
*/ 
extern void initialize_urls(void);

/**
 @brief     clean up the url memory allocated
 @return    void
*/ 
extern void free_urls(void);


/**
 @brief     get the data type by the url
 @param     url   :         http url string
 @param     url_length:     http url string length
 @return    data type
*/ 
extern int get_data_type(const char *url, int url_length);

/**
 @brief     check if the data should be cached by the url
 @param     url   :         http url string
 @param     url_length:     http url string length
 @return    1 or 0
*/
extern int is_cached_data(const char *url, int url_length);


#endif

