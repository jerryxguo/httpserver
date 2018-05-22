

#ifndef LOG_H
#define LOG_H

/**
 * log level definition
 */
typedef enum 
{
    ERROR = 0,
    WARNING,
    INFO,
    DEBUG
} log_level_t;

/**
 @brief     set the log file name
 @param     name:    the file name
 @return    void
*/ 
extern void set_log_name(const char * name);

/**
 @brief     set the log level
 @param     log_level:    the log level
 @return    void
 */
extern void set_log_level(const char * log_level);

/**
 @brief     write the data to log file
 @param     log_level:    the log level
 @param     fmt:          the data format for logging
 @return    void
 */
extern void logger(log_level_t log_level,char * fmt, ...);

#endif
