
/*
  HTTP 1.1 Spec - http://www.w3.org/Protocols/rfc2616/rfc2616.html
*/

#ifndef HTTP_H
#define HTTP_H


/*
 * HTTP METHODS
 */
typedef enum  {
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_GET,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_NOT_SUPPORTED,
    HTTP_INVALID
} http_method_t;

/*
 * HTTP STATUS CODES
 */
typedef enum http_status {
    /* Informational Statuses */
    HTTP_STATUS_CONTINUE,
    HTTP_STATUS_SWITCHING_PROTOCOLS,

    /* Success Statuses */
    HTTP_STATUS_OK,
    HTTP_STATUS_CREATED,
    HTTP_STATUS_ACCEPTED,
    HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION,
    HTTP_STATUS_NO_CONTENT,
    HTTP_STATUS_RESET_CONTENT,
    HTTP_STATUS_PARTIAL_CONTENT,
    
    /* Redirection Statuses */
    HTTP_STATUS_MULTIPLE_CHOICES,
    HTTP_STATUS_MOVED_PERMANENTLY,
    HTTP_STATUS_FOUND,
    HTTP_STATUS_SEE_OTHER,
    HTTP_STATUS_NOT_MODIFIED,
    HTTP_STATUS_USE_PROXY,
    HTTP_STATUS_TEMPORARY_REDIRECT,
    
    /* Client Error Statuses */
    HTTP_STATUS_BAD_REQUEST,
    HTTP_STATUS_UNAUTHORIZED,
    HTTP_STATUS_PAYMENT_REQUIRED,
    HTTP_STATUS_FORBIDDEN,
    HTTP_STATUS_NOT_FOUND,
    HTTP_STATUS_METHOD_NOT_ALLOWED,
    HTTP_STATUS_NOT_ACCEPTABLE,
    HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED,
    HTTP_STATUS_REQUEST_TIME_OUT,
    HTTP_STATUS_CONFLICT,
    HTTP_STATUS_GONE,
    HTTP_STATUS_LENGTH_REQUIRED,
    HTTP_STATUS_PRECONDITION_FAILED,
    HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE,
    HTTP_STATUS_REQUEST_URI_TOO_LARGE,
    HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
    HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE,
    HTTP_STATUS_EXPECTATION_FAILED,
    
    /* Server Error Statuses */
    HTTP_STATUS_INTERNAL_SERVER_ERROR,
    HTTP_STATUS_NOT_IMPLEMENTED,
    HTTP_STATUS_BAD_GATEWAY,
    HTTP_STATUS_SERVICE_UNAVAILABLE,
    HTTP_STATUS_GATEWAY_TIME_OUT,
    HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED
} http_status_t;

/**
 *  generic http status to string map structure 
 */
typedef struct {
    http_status_t   code;
    char *reason;
} http_status_map_t;


/**
 *  generic structure for holding http request 
 */
typedef struct {
    int            method;
    int            type;
    http_status_t  status;
    char           url[MAX_URL_LENGTH+1];
    int            major_version;
    int            minor_version;
} http_request_t;




#endif
