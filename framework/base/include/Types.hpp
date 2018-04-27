#ifndef _INCLUDE_TYPE_HPP_
#define _INCLUDE_TYPE_HPP_

typedef enum {
    SUCCESS = 0,
    MALLOC_MEMORY_FAILED = -1,
    PARAM_ERROR = -2,
    
    FILE_OPEN_FAILED = -10,
    FILE_READ_FAILED = -11,
    FILE_WRITE_FAILED = -12,
    FILE_GET_STATUS_FAILED = -13,
    FILE_FORMAT_INVALID = -14,
    FILE_FFLUSH_FAILED = -15,
    FILE_APPEND_FAILED = -16,

    SOCKET_ADDRESS_CONVERT_FAILED = -17,
    SOCKET_CREATE_FAILED = -18,
    SOCKET_BIND_FAILED = -19,
    SOCKET_LISTEN_FAILED = -20,
    SOCKET_ACCEPT_FAILED = -21,
    SOCKET_CLOSE_FAILED = -22,
    SOCKET_GETLOCAK_NAME_FAILED = -23,
    SOCKET_CONNECT_FAILED = -24,

} Status;

#endif
