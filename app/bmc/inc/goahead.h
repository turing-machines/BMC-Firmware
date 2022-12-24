/*
    goahead.h -- GoAhead Web Server Header

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

#ifndef _h_GOAHEAD
#define _h_GOAHEAD 1

/************************************ Overrides *******************************/
/*
    Override osdep defaults
 */
#define ME_MAX_IP 64                /**< Maximum IP address size */

/************************************ Includes ********************************/

#include    "me.h"
#include    "osdep.h"

/************************************ Defaults ********************************/

#ifdef __cplusplus
extern "C" {
#endif


#if (ME_COM_MBEDTLS + ME_COM_MATRIXSSL + ME_COM_NANOSSL + ME_COM_OPENSSL) > 1
#error "Cannot have more than one SSL provider configured"
#endif

#ifndef ME_GOAHEAD_LOGGING
#define ME_GOAHEAD_LOGGING 1                /**< Default for logging is "on" */
#endif
#ifndef ME_GOAHEAD_TRACING
#define ME_GOAHEAD_TRACING 1                /**< Default for tracing "on" */
#endif
#ifndef ME_GOAHEAD_DEBUG
#if ME_DEBUG
#define ME_GOAHEAD_DEBUG 1              /**< Debug logging on in debug builds by default */
#else
#define ME_GOAHEAD_DEBUG 0
#endif
#endif
#if ECOS
#if ME_GOAHEAD_CGI
#error "Ecos does not support CGI. Disable ME_GOAHEAD_CGI"
#endif
#endif /* ECOS */

#if QNX
typedef long fd_mask;
#define NFDBITS (sizeof (fd_mask) * NBBY)   /* bits per mask */
#endif

#if MACOSX
typedef int32_t fd_mask;
#endif
#if WINDOWS
typedef fd_set fd_mask;
#endif

#if !LINUX
PUBLIC char *basename(char *name);
#endif

#if VXWORKS
PUBLIC int vxchdir(char *dirname);
#endif

#if DOXYGEN
typedef int Socket;
typedef int Socklen;
typedef int64 Offset;
#endif

/**
    File status structure
 */
typedef struct stat WebsStat;

/*
    Copyright. The software license requires that this not be modified or removed.
 */
#define EMBEDTHIS_GOAHEAD_COPYRIGHT \
    "Copyright (c) Embedthis Software Inc., 1993-2014. All Rights Reserved." \
    "Copyright (c) GoAhead Software Inc., 2003. All Rights Reserved."

/************************************* Main ***********************************/

#define ME_MAX_ARGC 32
#if VXWORKS
#define MAIN(name, _argc, _argv, _envp)  \
        static int innerMain(int argc, char **argv, char **envp); \
        int name(char *arg0, ...) { \
            va_list args; \
            char *argp, *largv[ME_MAX_ARGC]; \
            int largc = 0; \
            va_start(args, arg0); \
            largv[largc++] = #name; \
            if (arg0) { \
                largv[largc++] = arg0; \
            } \
            for (argp = va_arg(args, char*); argp && largc < ME_MAX_ARGC; argp = va_arg(args, char*)) { \
                largv[largc++] = argp; \
            } \
            return innerMain(largc, largv, NULL); \
        } \
        static int innerMain(_argc, _argv, _envp)
#elif ME_WIN_LIKE
#define MAIN(name, _argc, _argv, _envp)  \
        APIENTRY WinMain(HINSTANCE inst, HINSTANCE junk, char *command, int junk2) { \
            extern int main(); \
            char *largv[ME_MAX_ARGC]; \
            int largc; \
            largc = websParseArgs(command, &largv[1], ME_MAX_ARGC - 1); \
            largv[0] = #name; \
            main(largc, largv, NULL); \
        } \
        int main(_argc, _argv, _envp)
#else
#define MAIN(name, _argc, _argv, _envp) int main(_argc, _argv, _envp)
#endif

PUBLIC int websParseArgs(char *args, char **argv, int maxArgc);

#if WINDOWS
PUBLIC void websSetInst(HINSTANCE inst);
PUBLIC HINSTANCE websGetInst();
#endif

/************************************ Tunables ********************************/

#define WEBS_MAX_LISTEN     8           /**< Maximum number of listen endpoints */
#define WEBS_SMALL_HASH     31          /**< General small hash size */
#define WEBS_MAX_PASSWORD   32          /**< Default maximum password */

/************************************* Error **********************************/
#if ME_GOAHEAD_LOGGING

#define WEBS_L          __FILE__, __LINE__
#define WEBS_ARGS_DEC   char *file, int line
#define WEBS_ARGS       file, line

PUBLIC_DATA int logLevel;

/**
    Standard logging trace levels are 0 to 9 with 0 being the most verbose. These are ored with the error source
    and type flags. The WEBS_LOG_MASK is used to extract the trace level from a flags word. We expect most apps
    to run with level 2 trace enabled.
*/
#define WEBS_ERROR          1           /**< Hard error trace level */
#define WEBS_WARN           2           /**< Soft warning trace level */
#define WEBS_CONFIG         2           /**< Configuration settings trace level. */
#define WEBS_VERBOSE        9           /**< Highest level of trace */
#define WEBS_LEVEL_MASK     0xF         /**< Level mask */

/*
    Log message flags
 */
#define WEBS_ASSERT_MSG     0x10        /**< Originated from assert */
#define WEBS_ERROR_MSG      0x20        /**< Originated from error */
#define WEBS_LOG_MSG        0x100       /**< Originated from logmsg */
#define WEBS_RAW_MSG        0x200       /**< Raw message output */
#define WEBS_TRACE_MSG      0x400       /**< Originated from trace */


#if ME_GOAHEAD_TRACING && ME_GOAHEAD_LOGGING
#if ME_COMPILER_HAS_MACRO_VARARGS
#define trace(l, ...) if (((l) & WEBS_LEVEL_MASK) <= websGetLogLevel()) { traceProc(l, __VA_ARGS__); } else {}
#else
inline void trace(int level, cchar *fmt, ...)
{
    WebsLogHandler logHandler = logGetHandler();
    if ((level & WEBS_LEVEL_MASK) <= logLevel && logHandler)
    {
        va_list args;
        va_start(args, fmt);
        char *message = sfmtv((char*) fmt, args);
        logHandler(level | WEBS_TRACE_MSG, message);
        wfree(message);
        va_end(args);
    }
}
#endif
#else
#define trace(l, ...) if (1) ; else {}
#endif

#if ME_GOAHEAD_LOGGING
#if ME_COMPILER_HAS_MACRO_VARARGS
#define logmsg(l, ...) if ((l) <= logLevel) { logmsgProc(l, __VA_ARGS__); } else {}
#else
inline void logmsg(int level, cchar *fmt, ...)
{
    WebsLogHandler logHandler = logGetHandler();
    if ((level & WEBS_LEVEL_MASK) <= logLevel && logHandler)
    {
        va_list args;
        va_start(args, fmt);
        char *message = sfmtv((char*) fmt, args);
        logHandler(level | WEBS_TRACE_MSG, message);
        wfree(message);
        va_end(args);
    }
}
#endif
#else
#define logmsg(l, ...) if (1) ; else {}
#endif


#if DOXYGEN
#undef assert
/**
    Assure that an assert condition is true
    @param cond Boolean result of a conditional test
    @stability Stable
 */
extern void assert(bool cond);
#elif ME_GOAHEAD_DEBUG
#define assert(C)       if (C) ; else assertError(WEBS_L, "%s", #C) //hisi
PUBLIC void assertError(WEBS_ARGS_DEC, char *fmt, ...);
#else
#define assert(C)       if (1) ; else {}  //hisi
#endif

//  Deprecated 3.1
#if DEPRECATED
#define traceOpen logOpen
#define traceClose logClose
#define WebsTraceHandler WebLogHandler
#define traceSetPath logSetPath
#define websGetTraceLevel websGetLogLevel
#endif

/**
    Callback for emitting trace log output
    @param level Integer between 0 and 9. Zero is the lowest trace level used for the most important messages.
    @param msg Message to log
    @return Zero if successful
    @internal
 */
typedef void (*WebsLogHandler)(int level, char *msg);

/**
    Emit an error message
    @return Zero if successful
    @stability Stable
*/
PUBLIC void error(char *fmt, ...);

/**
    Open the log logging module
    @return Zero if successful
    @internal
 */
PUBLIC int logOpen();

/**
    Close the log logging module
    @internal
 */
PUBLIC void logClose();

/**
    Get the  log callback
    @return handler Callback handler function of type WebsLogHandler
    @stability Stable
 */
PUBLIC WebsLogHandler logGetHandler();

/**
    Set a log callback
    @param handler Callback handler function of type WebsLogHandler
    @return The previous callback function
    @stability Stable
 */
PUBLIC WebsLogHandler logSetHandler(WebsLogHandler handler);

/**
    Get the current trace log level
    @return Number between 0 and 9
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websGetLogLevel();

/**
    Set the current trace log level
    @return Number between 0 and 9
    @ingroup Webs
    @stability Prototype
 */
void websSetLogLevel(int level);

/**
    Set the filename to save logging output
    @param path Filename path to use
    @stability Stable
 */
PUBLIC void logSetPath(char *path);

/**
    Emit a message to the log
    @description This emits a message at the specified level. GoAhead filters logging messages by defining a verbosity
    level at startup. Level 0 is the least verbose where only the most important messages will be output. Level 9 is the
    Logging support is enabled by the MakeMe setting: "logging: true" which creates the ME_GOAHEAD_LOGGING define in me.h
    most verbose. Level 2-4 are the most useful for debugging.
    @param level Integer verbosity level (0-9).
    @param fmt Printf style format string
    @param ... Arguments for the format string
    @stability Stable
 */
PUBLIC void logmsgProc(int level, char *fmt, ...);

/**
    Emit a debug trace message to the log
    @description This emits a message at the specified level. GoAhead filters logging messages by defining a verbosity
    level at startup. Level 0 is the least verbose where only the most important messages will be output. Level 9 is the
    most verbose. Level 2-4 are the most useful for debugging.
    Debug trace support is enabled by the MakeMe setting: "tracing: true" which creates the ME_GOAHEAD_TRACING define in
    me.h.
    @param level Integer verbosity level (0-9).
    @param fmt Printf style format string
    @param ... Arguments for the format string
    @stability Stable
 */
PUBLIC void traceProc(int level, char *fmt, ...);

#else /*! ME_GOAHEAD_LOGGING */
#define assert(C) if (1) ; else {}
#define error(l, ...) if (1) ; else {}
#define trace(l, ...) if (1) ; else {}
#define logOpen() if (1) ; else {}
#define logClose() if (1) ; else {}
#define websGetLogLevel() 0
#define logmsg(l, ...) if (1) ; else {}
#define logSetPath(p) if (1) ; else {}
#endif

/*********************************** HTTP Codes *******************************/
/*
    Standard HTTP/1.1 status codes
 */
#define HTTP_CODE_CONTINUE                  100     /**< Continue with request, only partial content transmitted */
#define HTTP_CODE_OK                        200     /**< The request completed successfully */
#define HTTP_CODE_CREATED                   201     /**< The request has completed and a new resource was created */
#define HTTP_CODE_ACCEPTED                  202     /**< The request has been accepted and processing is continuing */
#define HTTP_CODE_NOT_AUTHORITATIVE         203     /**< The request has completed but content may be from another source */
#define HTTP_CODE_NO_CONTENT                204     /**< The request has completed and there is no response to send */
#define HTTP_CODE_RESET                     205     /**< The request has completed with no content. Client must reset view */
#define HTTP_CODE_PARTIAL                   206     /**< The request has completed and is returning partial content */
#define HTTP_CODE_MOVED_PERMANENTLY         301     /**< The requested URI has moved permanently to a new location */
#define HTTP_CODE_MOVED_TEMPORARILY         302     /**< The URI has moved temporarily to a new location */
#define HTTP_CODE_SEE_OTHER                 303     /**< The requested URI can be found at another URI location */
#define HTTP_CODE_NOT_MODIFIED              304     /**< The requested resource has changed since the last request */
#define HTTP_CODE_USE_PROXY                 305     /**< The requested resource must be accessed via the location proxy */
#define HTTP_CODE_TEMPORARY_REDIRECT        307     /**< The request should be repeated at another URI location */
#define HTTP_CODE_BAD_REQUEST               400     /**< The request is malformed */
#define HTTP_CODE_UNAUTHORIZED              401     /**< Authentication for the request has failed */
#define HTTP_CODE_PAYMENT_REQUIRED          402     /**< Reserved for future use */
#define HTTP_CODE_FORBIDDEN                 403     /**< The request was legal, but the server refuses to process */
#define HTTP_CODE_NOT_FOUND                 404     /**< The requested resource was not found */
#define HTTP_CODE_BAD_METHOD                405     /**< The request HTTP method was not supported by the resource */
#define HTTP_CODE_NOT_ACCEPTABLE            406     /**< The requested resource cannot generate the required content */
#define HTTP_CODE_REQUEST_TIMEOUT           408     /**< The server timed out waiting for the request to complete */
#define HTTP_CODE_CONFLICT                  409     /**< The request had a conflict in the request headers and URI */
#define HTTP_CODE_GONE                      410     /**< The requested resource is no longer available*/
#define HTTP_CODE_LENGTH_REQUIRED           411     /**< The request did not specify a required content length*/
#define HTTP_CODE_PRECOND_FAILED            412     /**< The server cannot satisfy one of the request preconditions */
#define HTTP_CODE_REQUEST_TOO_LARGE         413     /**< The request is too large for the server to process */
#define HTTP_CODE_REQUEST_URL_TOO_LARGE     414     /**< The request URI is too long for the server to process */
#define HTTP_CODE_UNSUPPORTED_MEDIA_TYPE    415     /**< The request media type is not supported by the server or resource */
#define HTTP_CODE_RANGE_NOT_SATISFIABLE     416     /**< The request content range does not exist for the resource */
#define HTTP_CODE_EXPECTATION_FAILED        417     /**< The server cannot satisfy the Expect header requirements */
#define HTTP_CODE_NO_RESPONSE               444     /**< The connection was closed with no response to the client */
#define HTTP_CODE_INTERNAL_SERVER_ERROR     500     /**< Server processing or configuration error. No response generated */
#define HTTP_CODE_NOT_IMPLEMENTED           501     /**< The server does not recognize the request or method */
#define HTTP_CODE_BAD_GATEWAY               502     /**< The server cannot act as a gateway for the given request */
#define HTTP_CODE_SERVICE_UNAVAILABLE       503     /**< The server is currently unavailable or overloaded */
#define HTTP_CODE_GATEWAY_TIMEOUT           504     /**< The server gateway timed out waiting for the upstream server */
#define HTTP_CODE_BAD_VERSION               505     /**< The server does not support the HTTP protocol version */
#define HTTP_CODE_INSUFFICIENT_STORAGE      507     /**< The server has insufficient storage to complete the request */

/*
    Proprietary HTTP status codes
 */
#define HTTP_CODE_START_LOCAL_ERRORS        550
#define HTTP_CODE_COMMS_ERROR               550     /**< The server had a communicationss error responding to the client */

/************************************* WebsValue ******************************/
/**
    Value types.
 */
typedef enum WebsType
{
    undefined   = 0,
    byteint     = 1,
    shortint    = 2,
    integer     = 3,
    hex         = 4,
    percent     = 5,
    octal       = 6,
    big         = 7,
    flag        = 8,
    floating    = 9,
    string      = 10,
    bytes       = 11,
    symbol      = 12,
    errmsg      = 13
} WebsType;

/**
    System native time type. This is the time in seconds.
    This may be 32 or 64 bits and may be signed or unsigned on some systems.
 */
typedef time_t WebsTime;

/**
    Value union to store primitive value types
 */
typedef struct WebsValue
{
    union
    {
        char    flag;
        char    byteint;
        short   shortint;
        char    percent;
        long    integer;
        long    hex;
        long    octal;
        long    big[2];
#if ME_FLOAT
        double  floating;
#endif
        char    *string;
        char    *bytes;
        char    *errmsg;
        void    *symbol;
    } value;
    WebsType    type;
    uint        valid       : 8;
    uint        allocated   : 8;        /* String was allocated */
} WebsValue;

/**
    The value is a numeric type
 */
#define value_numeric(t)    (t >= byteint && t <= big)

/**
    The value is a string type
 */
#define value_str(t)        (t >= string && t <= bytes)

/**
    The value is valid supported type
 */
#define value_ok(t)         (t > undefined && t <= symbol)

/**
    Allocate strings using malloc
 */
#define VALUE_ALLOCATE      0x1

/**
    Create an integer value
    @param value Integer long value
    @return Value object containing the integer
    @stability Stable
 */
PUBLIC WebsValue valueInteger(long value);

/**
    Create an string value
    @param value String long value
    @param flags Set to VALUE_ALLOCATE to store a copy of the string reference
    @return Value object containing the string
    @stability Stable
 */
PUBLIC WebsValue valueString(char *value, int flags);

/**
    Create an symbol value containing an object reference
    @param value Value reference
    @return Value object containing the symbol reference
    @stability Stable
 */
PUBLIC WebsValue valueSymbol(void *value);

/**
    Free any allocated string in a value
    @param value Value object
    @stability Stable
 */
PUBLIC void valueFree(WebsValue *value);

/************************************* Ringq **********************************/
/**
    A WebsBuf (ring queue) allows maximum utilization of memory for data storage and is
    ideal for input/output buffering. This module provides a highly effecient
    implementation and a vehicle for dynamic strings.
    \n\n
    WARNING:  This is a public implementation and callers have full access to
    the queue structure and pointers.  Change this module very carefully.
    \n\n
    This module follows the open/close model.
    \n\n
    Operation of a WebsBuf where bp is a pointer to a WebsBuf :

        bp->buflen contains the size of the buffer.
        bp->buf will point to the start of the buffer.
        bp->servp will point to the first (un-consumed) data byte.
        bp->endp will point to the next free location to which new data is added
        bp->endbuf will point to one past the end of the buffer.
    \n\n
    Eg. If the WebsBuf contains the data "abcdef", it might look like :
    \n\n
    +-------------------------------------------------------------------+
    |   |   |   |   |   |   |   | a | b | c | d | e | f |   |   |   |   |
    +-------------------------------------------------------------------+
      ^                           ^                       ^               ^
      |                           |                       |               |
    bp->buf                    bp->servp               bp->endp      bp->enduf
    \n\n
    The queue is empty when servp == endp.  This means that the queue will hold
    at most bp->buflen -1 bytes.  It is the fillers responsibility to ensure
    the WebsBuf is never filled such that servp == endp.
    \n\n
    It is the fillers responsibility to "wrap" the endp back to point to
    bp->buf when the pointer steps past the end. Correspondingly it is the
    consumers responsibility to "wrap" the servp when it steps to bp->endbuf.
    The bufPutc and bufGetc routines will do this automatically.
    @defgroup WebsBuf WebsBuf
    @stability Stable
 */
typedef struct WebsBuf
{
    char    *buf;               /**< Holding buffer for data */
    char    *servp;             /**< Pointer to start of data */
    char    *endp;              /**< Pointer to end of data */
    char    *endbuf;            /**< Pointer to end of buffer */
    ssize   buflen;             /**< Length of ring queue */
    ssize   maxsize;            /**< Maximum size */
    int     increment;          /**< Growth increment */
} WebsBuf;

/**
    Add a trailing null to the buffer. The end pointer is not changed.
    @param bp Buffer reference
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufAddNull(WebsBuf *bp);

/**
    Adjust the endp pointer by the specified size.
    @description This is useful after manually copying data into the buffer and needing to adjust the end pointer.
    @param bp Buffer reference
    @param size Size of adjustment. May be positive or negative value.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufAdjustEnd(WebsBuf *bp, ssize size);

/**
    Adjust the start (servp) reference
    @param bp Buffer reference
    @param count Number of bytes to adjust
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufAdjustStart(WebsBuf *bp, ssize count);

/**
    Compact the data in the buffer and move to the start of the buffer
    @param bp Buffer reference
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufCompact(WebsBuf *bp);

/**
    Create a buffer
    @param bp Buffer reference
    @param increment Incremental size to grow the buffer. This will be increased by a power of two each time
        the buffer grows.
    @param maxsize Maximum size of the buffer
    @return Zero if successful
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC int bufCreate(WebsBuf *bp, int increment, int maxsize);

/**
    Flush all data in the buffer and reset the pointers.
    @param bp Buffer reference
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufFlush(WebsBuf *bp);

/**
    Free allocated storage for the buffer
    @param bp Buffer reference
    @return Zero if successful
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufFree(WebsBuf *bp);

/**
    Copy a block of from the buffer and adjust the servp.
    @param bp Buffer reference
    @param blk Block into which to place the data
    @param len Length of the block
    @return Number of bytes copied.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufGetBlk(WebsBuf *bp, char *blk, ssize len);

/**
    Return the maximum number of bytes the buffer can provide via a single block copy.
    @description Useful if the user is doing their own data retrieval.
    @param bp Buffer reference
    @return Number of bytes available for copying.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufGetBlkMax(WebsBuf *bp);

/**
    Get a character from the buffer and increment the servp
    @param bp Buffer reference
    @return The next character or -1 if the buffer is empty
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC int bufGetc(WebsBuf *bp);

/**
    Grow the buffer by at least the required amount of room
    @param bp Buffer reference
    @param room Available size required after growing the buffer
    @return True if the buffer can be grown to have the required amount of room.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC bool bufGrow(WebsBuf *bp, ssize room);

/**
    Get the length of available data in the buffer
    @param bp Buffer reference
    @return Size of available data in bytes
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufLen(WebsBuf *bp);

/**
    Insert a character to the buffer before the servp position and decrement the servp
    @param bp Buffer reference
    @param c Character to insert
    @return Zero if successful
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC int bufInsertc(WebsBuf *bp, char c);

/**
    Append a character to the buffer at the endp position and increment the endp
    @param bp Buffer reference
    @param c Character to append
    @return Zero if successful
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC int bufPutc(WebsBuf *bp, char c);
/**
    Put a block to the buffer.
    @param bp Buffer reference
    @param blk Block to append to the buffer
    @param len Size of the block
    @return Length of data appended. Should equal len.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufPutBlk(WebsBuf *bp, char *blk, ssize len);

/**
    Append a formatted string to the buffer at the endp position and increment the endp
    @param bp Buffer reference
    @param fmt Printf style format string
    @param ... Variable arguments for the format string
    @return Count of characters appended. Returns negative if there is an allocation error.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufPut(WebsBuf *bp, char *fmt, ...) PRINTF_ATTRIBUTE(2, 3);

/**
    Append a string to the buffer at the endp position and increment the endp
    @param bp Buffer reference
    @param str String to append
    @return Count of characters appended. Returns negative if there is an allocation error.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufPutStr(WebsBuf *bp, char *str);

/**
    Reset the buffer pointers to the start of the buffer if empty
    @param bp Buffer reference
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC void bufReset(WebsBuf *bp);

/**
    Determine the room available in the buffer.
    @description This returns the maximum number of bytes the buffer can absorb in a single block copy.
    @param bp Buffer reference
    @return Number of bytes of availble space.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC ssize bufRoom(WebsBuf *bp);

/**
    Get a reference to the start of buffer data
    @param bp Buffer reference
    @return A string pointer.
    @ingroup WebsBuf
    @stability Stable
 */
PUBLIC char *bufStart(WebsBuf *bp);

/******************************* Malloc Replacement ***************************/
#if ME_GOAHEAD_REPLACE_MALLOC
/**
    GoAhead allocator memory block
    Memory block classes are: 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536.
    @defgroup WebsAlloc WebsAlloc
    @stability Stable
 */
typedef struct WebsAlloc
{
    union
    {
        void    *next;                          /**< Pointer to next in q */
        int     size;                           /**< Actual requested size */
    } u;
    int         flags;                          /**< Per block allocation flags */
} WebsAlloc;

#define WEBS_DEFAULT_MEM   (64 * 1024)         /**< Default memory allocation */
#define WEBS_MAX_CLASS     13                  /**< Maximum class number + 1 */
#define WEBS_SHIFT         4                   /**< Convert size to class */
#define WEBS_ROUND         ((1 << (B_SHIFT)) - 1)
#define WEBS_MALLOCED      0x80000000          /* Block was malloced */
#define WEBS_FILL_CHAR     (0x77)              /* Fill byte for buffers */
#define WEBS_FILL_WORD     (0x77777777)        /* Fill word for buffers */

/*
    Flags. The integrity value is used as an arbitrary value to fill the flags.
 */
#define WEBS_USE_MALLOC        0x1             /**< Okay to use malloc if required */
#define WEBS_USER_BUF          0x2             /* User supplied buffer for mem */
#define WEBS_INTEGRITY         0x8124000       /* Integrity value */
#define WEBS_INTEGRITY_MASK    0xFFFF000       /* Integrity mask */
#endif /* ME_GOAHEAD_REPLACE_MALLOC */

/**
    Close the GoAhead memory allocator
    @ingroup WebsAlloc
    @stability Stable
 */
PUBLIC void wcloseAlloc();

/**
    Initialize the walloc module.
    @description The wopenAlloc function should be called the very first thing after the application starts and wclose
    should be called the last thing before exiting. If wopenAlloc is not called, it will be called on the first allocation
    with default values. "buf" points to memory to use of size "bufsize". If buf is NULL, memory is allocated using malloc.
    flags may be set to WEBS_USE_MALLOC if using malloc is okay. This routine will allocate *  an initial buffer of size
    bufsize for use by the application.
    @param buf Optional user supplied block of memory to use for allocations
    @param bufsize Size of buf
    @param flags Allocation flags. Set to WEBS_USE_MALLOC to permit the use of malloc() to grow memory.
    @return Zero if successful, otherwise -1.
    @ingroup WebsAlloc
    @stability Stable
 */
PUBLIC int wopenAlloc(void *buf, int bufsize, int flags);

/**
    Allocate a block of the requested size
    @param size Memory size required
    @return A reference to the allocated block
    @ingroup WebsAlloc
    @stability Stable
 */
PUBLIC void *walloc(ssize size);

/**
    Free an allocated block of memory
    @param blk Reference to the memory block to free.
    @ingroup WebsAlloc
    @stability Stable
 */
PUBLIC void wfree(void *blk);

/**
    Reallocate a block of memory and grow its size
    @description If the new size is larger than the existing block, a new block will be allocated and the old data
        will be copied to the new block.
    @param blk Original block reference
    @param newsize Size of the new block.
    @return Reference to the new memory block
    @ingroup WebsAlloc
    @stability Stable
 */
PUBLIC void *wrealloc(void *blk, ssize newsize);

/**
    Duplicate memory
    @param ptr Original block reference
    @param usize Size to allocate
    @return Reference to the new memory block
    @ingroup WebsAlloc
 */
PUBLIC void *wdup(cvoid *ptr, size_t usize);

typedef void (*WebsMemNotifier)(ssize size);

/**
    Define a global memory allocation notifier.
    @description The notifier is called if any memory allocation fails. It is called with the requested allocation size
        as its only parameter.
    @param cback Callback function to invoke for allocation failures.
    @ingroup WebsAlloc
    @stability Evolving
 */
PUBLIC void websSetMemNotifier(WebsMemNotifier cback);

#ifndef WEBS_SHIFT
#define WEBS_SHIFT 4
#endif

#if DEPRECATE || 1
PUBLIC ssize mtow(wchar *dest, ssize count, char *src, ssize len);
PUBLIC ssize wtom(char *dest, ssize count, wchar *src, ssize len);
PUBLIC wchar *amtow(char *src, ssize *len);
PUBLIC char  *awtom(wchar *src, ssize *len);
#endif

/******************************* Hash Table *********************************/
/**
    Hash table entry structure.
    @description The hash structure supports growable hash tables with high performance, collision resistant hashes.
    Each hash entry has a descriptor entry. This is used to manage the hash table link chains.
    @see hashCreate hashFree hashLookup hashEnter hashDelete hashWalk hashFirst hashNext
    @defgroup WebsHash WebsHash
    @stability Stable
 */
typedef struct WebsKey
{
    struct WebsKey  *forw;                  /* Pointer to next hash list */
    WebsValue       name;                   /* Name of symbol */
    WebsValue       content;                /* Value of symbol */
    int             arg;                    /* Parameter value */
    int             bucket;                 /* Bucket index */
} WebsKey;

/**
    Hash table ID returned by hashCreate
 */
typedef int WebsHash;                       /* Returned by symCreate */

/**
    Create a hash table
    @param size Minimum size of the hash index
    @return Hash table ID. Negative if the hash cannot be created.
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC WebsHash hashCreate(int size);

/**
    Free a hash table
    @param id Hash table id returned by hashCreate
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC void hashFree(WebsHash id);

/**
    Lookup a name in the hash table
    @param id Hash table id returned by hashCreate
    @param name Key name to search for
    @return Reference to the WebKey object storing the key and value
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC WebsKey *hashLookup(WebsHash id, char *name);

/**
    Lookup a name in the hash table and return a symbol reference
    @param sd Hash table id returned by hashCreate
    @param name Key name to search for
    @return Reference to the symbole
    @ingroup WebsHash
    @stability Evolving
 */
PUBLIC void *hashLookupSymbol(WebsHash sd, char *name);

/**
    Enter a new key and value into the hash table
    @param id Hash table id returned by hashCreate
    @param name Key name to create
    @param value Key value to enter
    @param arg Optional extra argument to store with the value
    @return Reference to the WebKey object storing the key and value
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC WebsKey *hashEnter(WebsHash id, char *name, WebsValue value, int arg);

/**
    Delete a key by name
    @param id Hash table id returned by hashCreate
    @param name Key name to delete
    @return Zero if the delete was successful. Otherwise -1 if the key was not found.
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC int hashDelete(WebsHash id, char *name);

/**
    Start walking the hash keys by returning the first key entry in the hash
    @param id Hash table id returned by hashCreate
    @return Reference to the first WebKey object. Return null if there are no keys in the hash.
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC WebsKey *hashFirst(WebsHash id);

/**
    Continue walking the hash keys by returning the next key entry in the hash
    @param id Hash table id returned by hashCreate
    @param last Reference to a WebsKey to hold the current traversal key state.
    @return Reference to the next WebKey object. Returns null if no more keys exist to be traversed.
    @ingroup WebsHash
    @stability Stable
 */
PUBLIC WebsKey *hashNext(WebsHash id, WebsKey *last);

/************************************ Socket **********************************/
/*
    Socket flags
 */
#define SOCKET_EOF              0x1     /**< Seen end of file */
#define SOCKET_CONNECTING       0x2     /**< Connect in progress */
#define SOCKET_RESERVICE        0x4     /**< Socket needs re-servicing */
#define SOCKET_ASYNC            0x8     /**< Use async connect */
#define SOCKET_BLOCK            0x10    /**< Use blocking I/O */
#define SOCKET_LISTENING        0x20    /**< Socket is server listener */
#define SOCKET_CLOSING          0x40    /**< Socket is closing */
#define SOCKET_CONNRESET        0x80    /**< Socket connection was reset */
#define SOCKET_HANDSHAKING      0x100   /**< Doing SSL handshake */
#define SOCKET_BUFFERED_READ    0x200   /**< Message pending on this socket */
#define SOCKET_BUFFERED_WRITE   0x400   /**< Message pending on this socket */
#define SOCKET_NODELAY          0x800   /**< Disable Nagle algorithm */

#define SOCKET_PORT_MAX         0xffff  /**< Max Port size */

#ifndef ME_MAX_IP
#define ME_MAX_IP 64                /**< Maximum IP address size */
#endif

/*
    Socket error values
 */
#define SOCKET_WOULDBLOCK       1       /**< Socket would block on I/O */
#define SOCKET_RESET            2       /**< Socket has been reset */
#define SOCKET_NETDOWN          3       /**< Network is down */
#define SOCKET_AGAIN            4       /**< Issue the request again */
#define SOCKET_INTR             5       /**< Call was interrupted */
#define SOCKET_INVAL            6       /**< Invalid */

/*
    Handler event masks
 */
#define SOCKET_READABLE         0x2     /**< Make socket readable */
#define SOCKET_WRITABLE         0x4     /**< Make socket writable */
#define SOCKET_EXCEPTION        0x8     /**< Interested in exceptions */

/**
    Socket I/O callback
    @param sid Socket ID handle returned from socketConnect or when a new socket is passed to a SocketAccept
        callback..
    @param mask Mask of events of interest. Set to SOCKET_READABLE | SOCKET_WRITABLE | SOCKET_EXCEPTION.
    @param data Data argument to pass to the callback function.
    @ingroup WebsSocket
    @stability Stable
 */
typedef void (*SocketHandler)(int sid, int mask, void *data);

/**
    Socket accept callback
    @param sid Socket ID handle for the newly accepted socket
    @param ipaddr IP address of the connecting client.
    @param port Port of the connecting client.
    @param listenSid Socket ID for the listening socket
    @ingroup WebsSocket
    @stability Stable
 */
typedef int (*SocketAccept)(int sid, char *ipaddr, int port, int listenSid);

/**
    Socket control structure
    @see socketAddress socketAddressIsV6 socketClose socketCloseConnection socketCreateHandler
    socketDeletehandler socketReservice socketEof socketGetPort socketInfo socketIsV6
    socketOpen socketListen socketParseAddress socketProcess socketRead socketWrite socketWriteString
    socketSelect socketGetHandle socketSetBlock socketGetBlock socketAlloc socketFree socketGetError
    socketSetError socketPtr socketWaitForEvent socketRegisterInterest
    @defgroup WebsSocket WebsSocket
    @stability Stable
 */
typedef struct WebsSocket
{
    WebsBuf         lineBuf;            /**< Line ring queue */
    SocketAccept    accept;             /**< Accept handler */
    SocketHandler   handler;            /**< User I/O handler */
    char            *ip;                /**< Server listen address or remote client address */
    void            *handler_data;      /**< User handler data */
    int             handlerMask;        /**< Handler events of interest */
    int             sid;                /**< Index into socket[] */
    int             port;               /**< Port to listen on */
    int             flags;              /**< Current state flags */
    Socket          sock;               /**< Actual socket handle */
    int             fileHandle;         /**< ID of the file handler */
    int             interestEvents;     /**< Mask of events to watch for */
    int             currentEvents;      /**< Mask of ready events (FD_xx) */
    int             selectEvents;       /**< Events being selected */
    int             saveMask;           /**< saved Mask for socketFlush */
    int             error;              /**< Last error */
    int             secure;             /**< Socket is using SSL */
    int             handshakes;         /**< Number of renegotiations */
} WebsSocket;


/**
    Extract the numerical IP address and port for the given socket info
    @param addr Reference to the socket address.
    @param addrlen Length of the socket address
    @param ipbuf Buffer to contain the parsed IP address
    @param ipLen Size of ipbuf
    @param port Reference to an integer to hold the parsed port.
    @return Zero if successful. Otherwise -1 for parse errors.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketAddress(struct sockaddr *addr, int addrlen, char *ipbuf, int ipLen, int *port);

/**
    Determine if an IP address is an IPv6 address.
    @param ip String IP address.
    @return True if the address is an IPv6 address.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC bool socketAddressIsV6(char *ip);

/**
    Allocate a socket object
    @param host String host IP address.
    @param port Socket port
    @param accept Optional SocketAccept accept callback function
    @param flags Control flags
    @return Socket ID handle to use with other APIs.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketAlloc(char *host, int port, SocketAccept accept, int flags);

/**
    Close the socket module
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketClose();

/**
    Close a socket connection
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketCloseConnection(int sid);

/**
    Connect to a server and create a new socket
    @param host Host IP address.
    @param port Port number to connect to
    @param flags Set to SOCKET_BLOCK for blocking I/O. Otherwise non-blocking I/O is used.
    @return True if the address is an IPv6 address.
    @ingroup WebsSocket
    @internal
    @stability Stable
 */
PUBLIC int socketConnect(char *host, int port, int flags);

/**
    Create a socket handler that will be invoked when I/O events occur.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param mask Mask of events of interest. Set to SOCKET_READABLE | SOCKET_WRITABLE | SOCKET_EXCEPTION.
    @param handler Socket handler function.
    @param arg Arbitrary object reference to pass to the SocketHandler callback function.
    @return True if the address is an IPv6 address.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketCreateHandler(int sid, int mask, SocketHandler handler, void *arg);

/**
    Delete a socket handler created via socketCreateHandler
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketDeleteHandler(int sid);

/**
    Determine if the socket is at end-of-file for input.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @return True if the address is at EOF
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC bool socketEof(int sid);

/**
    Free (and close) the socket
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketFree(int sid);

/**
    Get the current blocking mode
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @return True if the socket is in blocking mode.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketGetBlock(int sid);

/**
    Get the error code for the last socket operation on this thread.
    @return Integer error code. See errno or GetLastError() on windows.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketGetError();

/**
    Get the underlying socket operating system socket/file handle
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @return The socket handle
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC Socket socketGetHandle(int sid);

/**
    Get the list of sockets
    @return The socket list pointer
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC WebsSocket **socketGetList();

/**
    Get the IP port associated with this socket.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @return The TCP/IP port for this socket
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketGetPort(int sid);

/**
    Indicate if the system has a dual IPv4 and IPv6 stack
    @return True if IPv4 and IPv6 are supported on a single stack
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC bool socketHasDualNetworkStack();

/**
    Indicate if the system has IPv6 support
    @return True if IPv6 is supported on this system
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC bool socketHasIPv6();

/**
    Indicate that the application layer has buffered data for the socket.
    @description This is used by SSL and other network stacks that buffer pending data
    @param sp Socket object returned from #socketPtr
    @param len Length of buffered data in bytes
    @param dir Buffer direction. Set to MPR_READABLE for buffered read data and MPR_WRITABLE for buffered write data.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketHiddenData(WebsSocket *sp, ssize len, int dir);

/**
    Get a socket address structure for the specified IP:Port
    @description This returns address details in *family, *protocol, *addr, and *addrlen.
    @param ip IP address to parse
    @param port TCP/IP port number
    @param family Reference to an integer to hold the address family
    @param protocol Reference to an integer to hold the address protocol
    @param addr Reference to an integer to hold the address structure
    @param addrlen Reference to an integer to hold the address structure length
    @return Zero if successful, otherwise -1.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketInfo(char *ip, int port, int *family, int *protocol, struct sockaddr_storage *addr, Socklen *addrlen);

/**
    Determine if a socket is bound to an IPv6 address.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @return True if the socket is using IPv6.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC bool socketIsV6(int sid);

/**
    Open a listening socket
    @param host Host IP address on which to listen. Set to NULL to listen on all interfaces.
    @param port TCP/IP port on which to listen
    @param accept SocketAccept callback function to invoke to receive incoming connections.
    @param flags Reserved
    @return Zero if successful, otherwise -1.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketListen(char *host, int port, SocketAccept accept, int flags);

/**
    Open the socket module
    @return Zero if successful, otherwise -1.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketOpen();

/**
    Parse an IP address into its constituent parts.
    @description Parse the IP address and return the IP address and port components. Handles ipv4 and ipv6 addresses.
    If the IP portion is absent, pip is set to null. If the port portion is absent, port is set to the defaultPort.
    If a ":*" port specifier is used, pport is set to -1;
    When an address contains an ipv6 port it should be written as
        aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh:iiii
    or
        [aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh:iiii]:port
    If supplied an IPv6 address, the backets are stripped in the returned IP address.
    @param ipAddrPort IP address which may contain an optional ":port" component.
    @param pip Returns a reference to an allocated string containing the IP address portion. Caller must free.
    @param pport Reference to an integer to hold the port component.
    @param secure Reference to an integer to be set to true if the address is using SSL/TLS.
    @param defaultPort Default port number to use if no port specifier is included in ipAddrPort.
    @return Zero if successful, otherwise -1.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketParseAddress(char *ipAddrPort, char **pip, int *pport, int *secure, int defaultPort);

/**
    Process pending socket I/O events.
    @ingroup WebsSocket
    @stability Stable
    @internal
 */
PUBLIC void socketProcess();

/**
    Read data from a socket
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param buf Buffer to hold read data
    @param len Size of the buffer
    @return Count of bytes actually read. Returns -1 for errors and EOF. Distinguish between errors and EOF
        via socketEof().
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC ssize socketRead(int sid, void *buf, ssize len);

/**
    Register interest in socket I/OEvents
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param mask Mask of events of interest. Set to SOCKET_READABLE | SOCKET_WRITABLE | SOCKET_EXCEPTION.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketRegisterInterest(int sid, int mask);

/**
    Request that the socket be reserviced.
    @description This routine is useful when upper layers have unprocessed, buffered data for the socket.
        This routine will cause the socket I/O callback handler to be invoked soon in the future.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketReservice(int sid);

/**
    Wait for I/O on a socket
    @description This call uses the mask of events of interest defined by socketRegisterInterest. It blocks the caller
        until a suitable I/O event or timeout occurs.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param timeout Timeout in milliseconds.
    @return Number of I/O events.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketSelect(int sid, int timeout);

/**
    Set the socket blocking mode
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param on Set to 1 to enable blocking
    @return The previous blocking mode
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketSetBlock(int sid, int on);

/**
    Set the error code for the last socket operation on this thread.
    @param error Integer error code. See errno or GetLastError() on windows.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC void socketSetError(int error);

/**
    Set the socket delay mode
    @description This is used to enable or disable the TCP Nagle algorithm
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param on Set to 1 to disable the Nagle algorithm
    @return The previous blocking mode
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketSetNoDelay(int sid, bool on);

/**
    Wait for a socket I/O event
    @param sp Socket object
    @param mask Mask of events of interest. Set to SOCKET_READABLE | SOCKET_WRITABLE | SOCKET_EXCEPTION.
    @return Zero if successful in waiting for the desired event, othewise return -1.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC int socketWaitForEvent(WebsSocket *sp, int mask);

/**
    Write data to the socket
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @param buf Buffer containing data to write
    @param len Size of buf
    @return Count of bytes written. May be less than len if the socket is in non-blocking mode.
        Returns -1 for errors and if the socket cannot absorb any more data. If the transport is saturated,
        will return a negative error and errno will be set to EAGAIN or EWOULDBLOCK.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC ssize socketWrite(int sid, void *buf, ssize len);

/**
    Return the socket object for the socket ID.
    @param sid Socket ID handle returned from socketConnect or socketAccept.
    @return Corresponding socket object.
    @ingroup WebsSocket
    @stability Stable
 */
PUBLIC WebsSocket *socketPtr(int sid);

/*********************************** Runtime **********************************/

/**
    GoAhead Web Server Runtime
    @description GoAhead provides a secure runtime environment for safe string manipulation and to
        help prevent buffer overflows and other potential security traps.
    @defgroup WebsRuntime WebsRuntime
    @see fmt wallocHandle wallocObject wfreeHandle hextoi itosbuf scaselesscmp scaselessmatch
        sclone scmp scopy sfmt sfmtv slen slower smatch sstarts sncaselesscmp sncmp sncopy stok strim supper
    @stability Stable
 */

/**
    Format a string into a static buffer.
    @description This call format a string using printf style formatting arguments. A trailing null will
        always be appended. The call returns the size of the allocated string excluding the null.
    @param buf Pointer to the buffer.
    @param maxSize Size of the buffer.
    @param format Printf style format string
    @param ... Variable arguments to format
    @return Returns the buffer.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *fmt(char *buf, ssize maxSize, char *format, ...);

/**
    Allocate a handle from a map
    @param map Reference to a location holding the map reference. On the first call, the map is allocated.
    @return Integer handle index. Otherwise return -1 on allocation errors.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int wallocHandle(void *map);

/**
    Allocate an object in a halloc map
    @param map Reference to a location holding the map reference. On the first call, the map is allocated.
    @param max Reference to an integer that holds the maximum handle in the map.
    @param size Size of the object to allocate.
    @return Integer handle index. Otherwise return -1 on allocation errors.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int wallocObject(void *map, int *max, int size);

/**
    Free a handle in the map
    @param map Reference to a location to hold the map reference.
    @param handle Handle to free in the map.
    @return Integer handle index. Otherwise return -1 on allocation errors.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int wfreeHandle(void *map, int handle);

/**
    Convert a hex string to an integer
    @description This call converts the supplied string to an integer using base 16.
    @param str Pointer to the string to parse.
    @return Returns the integer equivalent value of the string.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC uint hextoi(char *str);

/**
    Convert an integer to a string buffer.
    @description This call converts the supplied 64 bit integer into a string formatted into the supplied buffer according
        to the specified radix.
    @param buf Pointer to the buffer that will hold the string.
    @param size Size of the buffer.
    @param value Integer value to convert
    @param radix The base radix to use when encoding the number
    @return Returns a reference to the string.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *itosbuf(char *buf, ssize size, int64 value, int radix);

/**
    Compare strings ignoring case. This is a safe replacement for strcasecmp. It can handle NULL args.
    @description Compare two strings ignoring case differences. This call operates similarly to strcmp.
    @param s1 First string to compare.
    @param s2 Second string to compare.
    @return Returns zero if the strings are equivalent, < 0 if s1 sorts lower than s2 in the collating sequence
        or > 0 if it sorts higher.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int scaselesscmp(char *s1, char *s2);

/**
    Compare strings ignoring case. This is similar to scaselesscmp but it returns a boolean.
    @description Compare two strings ignoring case differences.
    @param s1 First string to compare.
    @param s2 Second string to compare.
    @return Returns true if the strings are equivalent, otherwise false.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC bool scaselessmatch(char *s1, char *s2);

/**
    Clone a string
    @description Copy a string into a newly allocated block.
    @param str Pointer to the block to duplicate.
    @return Returns a newly allocated string.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *sclone(char *str);

/**
    Clone a substring.
    @description Copy a substring into a newly allocated block.
    @param str Pointer to the block to duplicate.
    @param len Number of bytes to copy. The actual length copied is the minimum of the given length and the length of
        the supplied string. The result is null terminated.
    @return Returns a newly allocated string.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *snclone(char *str, ssize len);

/**
    Compare strings.
    @description Compare two strings. This is a safe replacement for strcmp. It can handle null args.
    @param s1 First string to compare.
    @param s2 Second string to compare.
    @return Returns zero if the strings are identical. Return -1 if the first string is less than the second. Return 1
        if the first string is greater than the second.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int scmp(char *s1, char *s2);

/**
    Copy a string.
    @description Safe replacement for strcpy. Copy a string and ensure the destination buffer is not overflowed.
        The call returns the length of the resultant string or an error code if it will not fit into the target
        string. This is similar to strcpy, but it will enforce a maximum size for the copied string and will
        ensure it is always terminated with a null.
    @param dest Pointer to a pointer that will hold the address of the allocated block.
    @param destMax Maximum size of the target string in characters.
    @param src String to copy
    @return Returns the number of characters in the target string.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC ssize scopy(char *dest, ssize destMax, char *src);

/*
    String trim flags
 */
#define WEBS_TRIM_START  0x1             /**< Flag for strim to trim from the start of the string */
#define WEBS_TRIM_END    0x2             /**< Flag for strim to trim from the end of the string */
#define WEBS_TRIM_BOTH   0x3             /**< Flag for strim to trim from both the start and the end of the string */

/**
    Format a string. This is a secure verion of printf that can handle null args.
    @description Format the given arguments according to the printf style format. See fmt() for a full list of the
        format specifies. This is a secure replacement for sprintf, it can handle null arguments without crashes.
    @param format Printf style format string
    @param ... Variable arguments for the format string
    @return Returns a newly allocated string
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *sfmt(char *format, ...);

/**
    Format a string with varargs. This is a secure verion of printf that can handle null args.
    @description Format the given arguments according to the printf style format. See fmt() for a full list of the
        format specifies. This is a secure replacement for sprintf, it can handle null arguments without crashes.
    @param format Printf style format string
    @param args Varargs argument obtained from va_start.
    @return Returns a newly allocated string
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *sfmtv(char *format, va_list args);

/**
    Return the length of a string.
    @description Safe replacement for strlen. This call returns the length of a string and tests if the length is
        less than a given maximum. It will return zero for NULL args.
    @param str String to measure.
    @return Returns the length of the string
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC ssize slen(cchar *str);

/**
    Convert a string to lower case.
    @description Convert a string to its lower case equivalent.
    @param str String to convert. This string is modified.
    @return Reference to the supplied str.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *slower(char *str);

/**
    Compare strings
    @description Compare two strings. This is similar to #scmp but it returns a boolean.
    @param s1 First string to compare.
    @param s2 Second string to compare.
    @return Returns true if the strings are equivalent, otherwise false.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC bool smatch(char *s1, char *s2);

/**
    Compare strings ignoring case.
    @description Compare two strings ignoring case differences for a given string length. This call operates
        similarly to strncasecmp.
    @param s1 First string to compare.
    @param s2 Second string to compare.
    @param len Length of characters to compare.
    @return Returns zero if the strings are equivalent, < 0 if s1 sorts lower than s2 in the collating sequence
        or > 0 if it sorts higher.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int sncaselesscmp(char *s1, char *s2, ssize len);

/**
    Compare strings.
    @description Compare two strings for a given string length. This call operates similarly to strncmp.
    @param s1 First string to compare.
    @param s2 Second string to compare.
    @param len Length of characters to compare.
    @return Returns zero if the strings are equivalent, < 0 if s1 sorts lower than s2 in the collating sequence
        or > 0 if it sorts higher.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int sncmp(char *s1, char *s2, ssize len);

/**
    Copy characters from a string.
    @description Safe replacement for strncpy. Copy bytes from a string and ensure the target string is not overflowed.
        The call returns the length of the resultant string or an error code if it will not fit into the target
        string. This is similar to strcpy, but it will enforce a maximum size for the copied string and will
        ensure it is terminated with a null.
    @param dest Pointer to a pointer that will hold the address of the allocated block.
    @param destMax Maximum size of the target string in characters.
    @param src String to copy
    @param count Maximum count of characters to copy
    @return Returns a reference to the destination if successful or NULL if the string won't fit.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC ssize sncopy(char *dest, ssize destMax, char *src, ssize count);

/*
    Test if a string is a radix 10 number.
    @description The supported format is: [(+|-)][DIGITS]
    @return true if all characters are digits or '+' or '-'
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC bool snumber(cchar *s);

/**
    Split a string at a delimiter
    @description Split a string and return parts. The string is modified.
        This routiner never returns null. If there are leading delimiters, the empty string will be returned
        and *last will be set to the portion after the delimiters.
        If str is null, an empty string will be returned.
        If there are no characters after the delimiter, then *last will be set to the empty string.
    @param str String to tokenize.
    @param delim Set of characters that are used as token separators.
    @param last Reference to the portion after the delimiters. Will return an empty string if is not trailing portion.
    @return Returns a pointer to the first part before the delimiters. If the string begins with delimiters, the empty
        string will be returned.
    @ingroup WebsRuntime
    @stability Evolving
 */
PUBLIC char *ssplit(char *str, cchar *delim, char **last);

/**
    Test if the string starts with a given pattern.
    @param str String to examine
    @param prefix Pattern to search for
    @return Returns TRUE if the pattern was found. Otherwise returns zero.
    @ingroup MprString
    @stability Stable
 */
PUBLIC bool sstarts(cchar *str, cchar *prefix);

/**
    Tokenize a string
    @description Split a string into tokens.
    @param str String to tokenize.
    @param delim String of characters to use as token separators.
    @param last Last token pointer.
    @return Returns a pointer to the next token.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *stok(char *str, char *delim, char **last);

/**
    Trim a string.
    @description Trim leading and trailing characters off a string.
    @param str String to trim.
    @param set String of characters to remove.
    @param where Flags to indicate trim from the start, end or both. Use WEBS_TRIM_START, WEBS_TRIM_END, WEBS_TRIM_BOTH.
    @return Returns a pointer to the trimmed string. May not equal \a str.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *strim(char *str, char *set, int where);

/**
    Convert a string to upper case.
    @description Convert a string to its upper case equivalent.
    @param str String to convert. This string is modified.
    @return Returns a pointer to the converted string. Will always equal str.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC char *supper(char *str);

/**
    Callback function for events
    @param data Opaque data argument
    @param id Event ID
    @ingroup WebsRuntime
    @stability Stable
 */
typedef void (*WebsEventProc)(void *data, int id);

/**
    Start a callback event
    @description This schedules an event to run once. The event can be rescheduled in the callback by invoking
    websRestartEvent.
    @param delay Delay in milliseconds in which to run the callback
    @param proc Callback procedure function. Signature is: void (*fn)(void *data, int id)
    @param data Data reference to pass to the callback
    @return A positive integer event ID
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC int websStartEvent(int delay, WebsEventProc proc, void *data);

/**
    Stop an event
    @param id Event id allocated by websStartEvent
    @return Integer handle index. Otherwise return -1 on allocation errors.
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC void websStopEvent(int id);

/**
    Restart an event
    @param id Event id allocated by websStartEvent
    @param delay Delay in milliseconds till the event next runs
    @ingroup WebsRuntime
    @stability Stable
 */
PUBLIC void websRestartEvent(int id, int delay);

/**
    Run due events
    @ingroup WebsRuntime
    @return Time delay till the next event
    @internal
 */
PUBLIC int websRunEvents();

/* Forward declare */
struct WebsRoute;
struct WebsUser;
struct WebsSession;
struct Webs;

/********************************** Upload ************************************/
#if ME_GOAHEAD_UPLOAD

/**
    File upload structure
    @see websUploadOpen websLookupUpload websGetUpload
    @defgroup WebsUpload WebsUpload
 */
typedef struct WebsUpload
{
    char    *filename;              /**< Local (temp) name of the file */
    char    *clientFilename;        /**< Client side name of the file */
    char    *contentType;           /**< Content type */
    ssize   size;                   /**< Uploaded file size */
} WebsUpload;

/**
    Open the file upload filter
    @ingroup WebsUpload
    @stability Stable
 */
PUBLIC void websUploadOpen();

/**
    Get the hash of uploaded files for the request
    @param wp Webs request object
    @return Hash table of uploaded files
    @ingroup WebsUpload
    @stability Stable
 */
PUBLIC WebsHash websGetUpload(struct Webs *wp);

/**
    Open the file upload filter
    @param wp Webs request object
    @param key Form upload name
    @return Upload object for the uploaded file
    @ingroup WebsUpload
    @stability Stable
 */
PUBLIC WebsUpload *websLookupUpload(struct Webs *wp, char *key);
#endif
/********************************** Defines ***********************************/

#define WEBS_MAX_PORT_LEN       16          /* Max digits in port number */
#define WEBS_HASH_INIT          67          /* Hash size for form table */
#define WEBS_SESSION_HASH       31          /* Hash size for session stores */
#define WEBS_SESSION_PRUNE      (60*1000)   /* Prune sessions every minute */

/*
    The license agreement stipulates that you must not change this definition.
 */
#define WEBS_NAME "Server: GoAhead-http"

/*
    Request flags
 */
#define WEBS_ACCEPTED           0x1         /**< TLS connection accepted */
#define WEBS_CHUNKING           0x2         /**< Currently chunking output body data */
#define WEBS_CLOSED             0x4         /**< Connection closed, ready to free */
#define WEBS_COOKIE             0x8         /**< Cookie supplied in request */
#if DEPRECATED || 1
#define WEBS_FINALIZED          0x10        /**< Output is finalized */
#endif
#define WEBS_FORM               0x20        /**< Request is a form (url encoded data) */
#define WEBS_HEADERS_CREATED    0x40        /**< Headers have been created and buffered */
#define WEBS_HTTP11             0x80        /**< Request is using HTTP/1.1 */
#define WEBS_JSON               0x100       /**< Request has a JSON payload */
#define WEBS_KEEP_ALIVE         0x200       /**< HTTP/1.1 keep alive */
#define WEBS_REROUTE            0x400       /**< Restart route matching */
#define WEBS_RESPONSE_TRACED    0x800       /**< Started tracing the response */
#define WEBS_SECURE             0x1000      /**< Connection uses SSL */
#define WEBS_UPLOAD             0x2000      /**< Multipart-mime file upload */
#define WEBS_VARS_ADDED         0x4000      /**< Query and body form vars added */
#if ME_GOAHEAD_LEGACY
#define WEBS_LOCAL              0x8000      /**< Request from local system */
#endif

/*
    Incoming chunk encoding states. Used for tx and rx chunking.
 */
#define WEBS_CHUNK_UNCHUNKED    0           /**< Data is not transfer-chunk encoded */
#define WEBS_CHUNK_START        1           /**< Start of a new chunk */
#define WEBS_CHUNK_HEADER       2           /**< Preparing tx chunk header */
#define WEBS_CHUNK_DATA         3           /**< Start of chunk data */

/*
    Webs state
 */
#define WEBS_BEGIN              0           /**< Beginning state */
#define WEBS_CONTENT            1           /**< Ready for body data */
#define WEBS_READY              2           /**< Ready to route and start handler */
#define WEBS_RUNNING            3           /**< Processing request */
#define WEBS_COMPLETE           4           /**< Request complete */

/*
    Session names
 */
#define WEBS_SESSION            "-goahead-session-"
#define WEBS_SESSION_USERNAME   "_:USERNAME:_"  /* Username variable */

/*
    WebsDone flags
 */
#define WEBS_CODE_MASK      0xFFFF      /**< Mask valid status codes */
#define WEBS_CLOSE          0x20000     /**< Close connection */
#define WEBS_NOLOG          0x40000     /**< Don't write error to log */

/**
    Callback for write I/O events
 */
typedef void (*WebsWriteProc)(struct Webs *wp);

/**
    GoAhead request structure. This is a per-socket connection structure.
    @defgroup Webs Webs
 */
typedef struct Webs
{
    WebsBuf         rxbuf;              /**< Raw receive buffer */
    WebsBuf         input;              /**< Receive buffer after de-chunking */
    WebsBuf         output;             /**< Transmit buffer after chunking */
    WebsBuf         chunkbuf;           /**< Pre-chunking data buffer */
    WebsBuf         *txbuf;
    WebsTime        since;              /**< Parsed if-modified-since time */
    WebsTime        timestamp;          /**< Last transaction with browser */
    WebsHash        vars;               /**< CGI standard variables */
    int             timeout;            /**< Timeout handle */
    char            ipaddr[ME_MAX_IP];  /**< Connecting ipaddress */
    char            ifaddr[ME_MAX_IP];  /**< Local interface ipaddress */

    int             rxChunkState;       /**< Rx chunk encoding state */
    ssize           rxChunkSize;        /**< Rx chunk size */
    char            *rxEndp;            /**< Pointer to end of raw data in input beyond endp */
    ssize           lastRead;           /**< Number of bytes last read from the socket */
    bool            eof;                /**< If at the end of the request content */

    char            txChunkPrefix[16];  /**< Transmit chunk prefix */
    char            *txChunkPrefixNext; /**< Current I/O pos in txChunkPrefix */
    ssize           txChunkPrefixLen;   /**< Length of prefix */
    ssize           txChunkLen;         /**< Length of the chunk */
    int             txChunkState;       /**< Transmit chunk state */

    char            *authDetails;       /**< Http header auth details */
    char            *authResponse;      /**< Outgoing auth header */
    char            *authType;          /**< Authorization type (Basic/DAA) */
    char            *contentType;       /**< Body content type */
    char            *cookie;            /**< Request cookie string */
    char            *decodedQuery;      /**< Decoded request query */
    char            *digest;            /**< Password digest */
    char            *ext;               /**< Path extension */
    char            *filename;          /**< Document path name */
    char            *host;              /**< Requested host */
    char            *method;            /**< HTTP request method */
    char            *password;          /**< Authorization password */
    char            *path;              /**< Path name without query. This is decoded. */
    char            *protoVersion;      /**< Protocol version (HTTP/1.1)*/
    char            *protocol;          /**< Protocol scheme (normally http|https) */
    char            *putname;           /**< PUT temporary filename */
    char            *query;             /**< Request query. This is decoded. */
    char            *realm;             /**< Realm field supplied in auth header */
    char            *referrer;          /**< The referring page */
    char            *responseCookie;    /**< Outgoing cookie */
    char            *url;               /**< Full request url. This is not decoded. */
    char            *userAgent;         /**< User agent (browser) */
    char            *username;          /**< Authorization username */
    int             sid;                /**< Socket id (handler) */
    int             listenSid;          /**< Listen Socket id */
    int             port;               /**< Request port number */
    int             state;              /**< Current state */
    int             flags;              /**< Current flags -- see above */
    int             code;               /**< Response status code */
    int             routeCount;         /**< Route count limiter */
    ssize           rxLen;              /**< Rx content length */
    ssize           rxRemaining;        /**< Remaining content to read from client */
    ssize           txLen;              /**< Tx content length header value */
    int             wid;                /**< Index into webs */
#if ME_GOAHEAD_CGI
    char            *cgiStdin;          /**< Filename for CGI program input */
    int             cgifd;              /**< File handle for CGI program input */
#endif
#if !ME_ROM
    int             putfd;              /**< File handle to write PUT data */
#endif
    int             docfd;              /**< File descriptor for document being served */
    ssize           written;            /**< Bytes actually transferred */
    ssize           putLen;             /**< Bytes read by a PUT request */

    int             finalized: 1;          /**< Request has been completed */
    int             error: 1;              /**< Request has an error */
    int             connError: 1;          /**< Request has a connection error */

    struct WebsSession *session;        /**< Session record */
    struct WebsRoute *route;            /**< Request route */
    struct WebsUser *user;              /**< User auth record */
    WebsWriteProc   writeData;          /**< Handler write I/O event callback. Used by fileHandler */
    int             encoded;            /**< True if the password is MD5(username:realm:password) */
#if ME_GOAHEAD_DIGEST
    char            *cnonce;            /**< check nonce */
    char            *digestUri;         /**< URI found in digest header */
    char            *nonce;             /**< opaque-to-client string sent by server */
    char            *nc;                /**< nonce count */
    char            *opaque;            /**< opaque value passed from server */
    char            *qop;               /**< quality operator */
#endif
#if ME_GOAHEAD_UPLOAD
    int             upfd;               /**< Upload file handle */
    WebsHash        files;              /**< Uploaded files */
    char            *boundary;          /**< Mime boundary (static) */
    ssize           boundaryLen;        /**< Boundary length */
    int             uploadState;        /**< Current file upload state */
    WebsUpload      *currentFile;       /**< Current file context */
    char            *clientFilename;    /**< Current file filename */
    char            *uploadTmp;         /**< Current temp filename for upload data */
    char            *uploadVar;         /**< Current upload form variable name */
#endif
    void            *ssl;               /**< SSL context */
} Webs;

#if ME_GOAHEAD_LEGACY
#define WEBS_LEGACY_HANDLER 0x1     /* Using legacy calling sequence */
#endif


/**
    GoAhead handler service callback
    @param wp Webs request object
    @return True if the handler serviced the request
    @ingroup Webs
    @stability Stable
 */
typedef bool (*WebsHandlerProc)(Webs *wp);

/**
    GoAhead handler close to release memory prior to shutdown.
    @description This callback is invoked when GoAhead is shutting down.
    @ingroup Webs
    @stability Stable
 */
typedef void (*WebsHandlerClose)();

/**
    GoAhead handler object
    @ingroup Webs
    @stability Stable
 */
typedef struct WebsHandler
{
    char                *name;              /**< Handler name */
    WebsHandlerProc     match;              /**< Handler match callback */
    WebsHandlerProc     service;            /**< Handler service callback */
    WebsHandlerClose    close;              /**< Handler close callback  */
    int                 flags;              /**< Handler control flags */
} WebsHandler;

/**
    Action callback
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
typedef void (*WebsAction)(Webs *wp);

#if ME_GOAHEAD_LEGACY
typedef void (*WebsProc)(Webs *wp, char *path, char *query);
#endif

/**
    Error code list
    @ingroup Webs
    @stability Stable
 */
typedef struct WebsError
{
    int     code;                           /**< HTTP error code */
    char    *msg;                           /**< HTTP error message */
} WebsError;

/**
    Mime type list
    @ingroup Webs
    @stability Stable
 */
typedef struct WebsMime
{
    char    *type;                          /**< Mime type */
    char    *ext;                           /**< File extension */
} WebsMime;

/**
    File information structure.
    @ingroup Webs
    @stability Stable
 */
typedef struct WebsFileInfo
{
    ulong           size;                   /**< File length */
    int             isDir;                  /**< Set if directory */
    WebsTime        mtime;                  /**< Modified time */
} WebsFileInfo;

/**
    Compiled Rom Page Index
    @ingroup Webs
    @stability Stable
 */
typedef struct WebsRomIndex
{
    char            *path;                  /**< Web page URL path */
    uchar           *page;                  /**< Web page data */
    int             size;                   /**< Size of web page in bytes */
    Offset          pos;                    /**< Current read position */
} WebsRomIndex;

#if ME_ROM
/**
    List of documents to service when built with ROM support
    @ingroup Webs
    @stability Stable
 */
PUBLIC_DATA WebsRomIndex websRomIndex[];
#endif

#define WEBS_DECODE_TOKEQ 1                 /**< Decode base 64 blocks up to a NULL or equals */

/**
    Accept a new connection
    @param sid Socket ID handle for the newly accepted socket
    @param ipaddr IP address originating the connection.
    @param port Port number originating the connection.
    @param listenSid Socket ID of the listening socket
    @return Zero if successful, otherwise -1
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websAccept(int sid, char *ipaddr, int port, int listenSid);

/**
    Open the action handler
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websActionOpen();

/**
    Allocate a new Webs object
    @param sid Socket ID handle for the newly accepted socket
    @return The webs[] handle index for the allocated Webs object
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websAlloc(int sid);

/**
    Cancel the request timeout.
    @description Handlers may choose to manually manage the request timeout. This routine will disable the
        centralized management of the timeout for this request.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websCancelTimeout(Webs *wp);

#if ME_GOAHEAD_CGI
/**
    Open the CGI handler
    @return Zero if successful, otherwise -1
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websCgiOpen();

/**
    CGI handler service callback
    @param wp Webs object
    @return Returns 1 if the request was handled.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websCgiHandler(Webs *wp);

/**
    Poll for output from CGI processes and output.
    @return Time delay till next poll
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websCgiPoll();

/* Internal */
PUBLIC bool cgiHandler(Webs *wp);

#endif /* ME_GOAHEAD_CGI */

/**
    Close the core GoAhead web server module
    @description Invoked when GoAhead is shutting down.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websClose();

/**
    Close an open file
    @param fd Open file handle returned by websOpenFile
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websCloseFile(int fd);

/**
    Compare a request variable
    @param wp Webs request object
    @param var Variable name
    @param value Value to compare with
    @return True if the value matches. Otherwise return 0
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websCompareVar(Webs *wp, char *var, char *value);

/**
    Consume input from the request input buffer.
    @description This is called by handlers when consuming data from the request input buffer.
        This call updates the input service pointers and compacts the input buffer if required.
    @param wp Webs request object
    @param nbytes Number of bytes the handler has consumed from the input buffer.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websConsumeInput(Webs *wp, ssize nbytes);

/**
    Decode the string using base-64 encoding
    @description This modifies the original string
    @param str String to decode
    @return The original string.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websDecode64(char *str);

/**
    Decode a block using base-46 encoding
    @param str String to decode. The string must be null terminated.
    @param len Reference to an integer holding the length of the decoded string.
    @param flags Reserved.
    @return The original string.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websDecode64Block(char *str, ssize *len, int flags);

/**
    Decode a URL expanding %NN encoding
    @description Supports insitu decoding. i.e. Input and output buffers may be the same.
    @param decoded Buffer to hold the decoded URL
    @param input Input URL or buffer to decode
    @param len Length of the decoded buffer.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websDecodeUrl(char *decoded, char *input, ssize len);

/**
    Define a request handler
    @param name Name of the handler
    @param match Handler callback match procedure. Invoked to match the request with the handler.
        The handler should return true to accept the request.
    @param service Handler callback service procedure. Invoked to service each request.
    @param close Handler callback close procedure. Called when GoAhead is shutting down.
    @param flags Set to WEBS_LEGACY_HANDLER to support the legacy handler API calling sequence.
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websDefineHandler(char *name, WebsHandlerProc match, WebsHandlerProc service, WebsHandlerClose close, int flags);

/**
    Complete a request.
    @description A handler should call websDone() to complete the request.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websDone(Webs *wp);

/**
    Encode a string using base-64 encoding
    @description The string is encoded insitu.
    @param str String to encode
    @return The original string.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websEncode64(char *str);

/**
    Encode a block using base-64 encoding
    @description The string is encoded insitu.
    @param str String to encode.
    @param len Length of string to encode
    @return The original string.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websEncode64Block(char *str, ssize len);

/**
    Escape unsafe characters in a string
    @param str String to escape
    @return An allocated block containing the escaped string. Caller must free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websEscapeHtml(char *str);

/**
    Complete a request with an error response
    @param wp Webs request object
    @param code HTTP status code
    @param fmt Message printf style format
    @param ... Format args
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websError(Webs *wp, int code, char *fmt, ...);

/**
    Get a message for a HTTP status code
    @param code HTTP status code
    @return Http status message
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websErrorMsg(int code);

/**
    Open and initialize the file handler
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websFileOpen();

/**
    Flush buffered transmit data and compact the transmit buffer to make room for more data
    @description This call initiates sending buffered data. If blocking mode is selected via the block parameter,
        this call will wait until all the data has been sent to the O/S for transmission to the client.
        If block is false, the flush will be initiated and the call will return immediately without blocking.
    @param wp Webs request object
    @param block Set to true to wait for all data to be written to the socket. Set to false to
        write whatever the socket can absorb without blocking.
    @return -1 for I/O errors. Return zero if there is more data remaining in the buffer. Return 1 if the
        contents of the transmit buffer are fully written and the buffer is now empty.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websFlush(Webs *wp, bool block);

/**
    Free the webs request object.
    @description Callers should call websDone to complete requests prior to invoking websFree.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websFree(Webs *wp);

/**
    Get the background execution flag
    @description If GoAhead is invoked with --background, it will run as a daemon in the background.
    @return True if GoAhead is running in the background.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websGetBackground();

#if ME_GOAHEAD_CGI
/**
    Get a unique temporary filename for CGI communications
    @return Filename string
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetCgiCommName();
#endif /* ME_GOAHEAD_CGI */

/**
    Get the request cookie if supplied
    @param wp Webs request object
    @return Cookie string if defined, otherwise null.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetCookie(Webs *wp);

/**
    Get a date as a string
    @description If sbuf is supplied, it is used to calculate the date. Otherwise, the current time is used.
    @param sbuf File info object
    @return An allocated date string. Caller should free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetDateString(WebsFileInfo *sbuf);

/**
    Get the debug flag
    @description If GoAhead is invoked with --debugger, the debug flag will be set to true
    @return True if GoAhead is running in debug mode.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websGetDebug();

/**
    Get the base file directory for a request
    @description Returns the request route directory if defined, otherwise returns the documents directory.
    @param wp Webs request object
    @return Path name string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetDir(Webs *wp);

/**
    Get the GoAhead base documents directory
    @description The documents directory is defined at build time and may be overridden by the GoAhead command line.
    @return Path string for the documents directory.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetDocuments();

/**
    Get the request EOF status
    @description The request EOF status is set to true when all the request body (POST|PUT) data has been received.
    @param wp Webs request object
    @return True if all the request body data has been received.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websGetEof(Webs *wp);

/**
    Get the request URI extension
    @param wp Webs request object
    @return The URI filename extension component. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetExt(Webs *wp);

/**
    Get the request filename
    @description The URI is mapped to a filename by decoding and prepending with the request directory.
    @param wp Webs request object
    @return Filename string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetFilename(Webs *wp);

/**
    Get the request host
    @description The request host is set to the Host HTTP header value if it is present. Otherwise it is set to
        the request URI hostname.
    @param wp Webs request object
    @return Host string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetHost(Webs *wp);

/**
    Get the request interface address
    @param wp Webs request object
    @return Network interface string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetIfaddr(Webs *wp);

/**
    Get the default index document name
    @description The default index is "index.html" and can be updated via websSetIndex.
    @return Index name string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetIndex();

/**
    Get the request method
    @param wp Webs request object
    @return HTTP method string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetMethod(Webs *wp);

/**
    Get the request password
    @description The request password may be encoded depending on the authentication scheme.
        See wp->encoded to test if it is encoded.
    @param wp Webs request object
    @return Password string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetPassword(Webs *wp);

/**
    Get the request path
    @description The URI path component excludes the http protocol, hostname, port, reference and query components.
    It always beings with "/".
    @param wp Webs request object
    @return Request path string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetPath(Webs *wp);

/**
    Get the request TCP/IP port
    @param wp Webs request object
    @return TCP/IP Port integer
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websGetPort(Webs *wp);

/**
    Get the request HTTP protocol
    @description This will be set to either "http" or "https"
    @param wp Webs request object
    @return Protocol string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetProtocol(Webs *wp);

/**
    Get the request query component
    @param wp Webs request object
    @return Request query string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetQuery(Webs *wp);

/**
    Get the server host name
    @return Host name string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetServer();

/**
    Get the server host name with port number.
    @return Host name string with port number. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetServerUrl();

/**
    Get the server IP address
    @return Server IP address string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetServerAddress();

/**
    Get the server IP address with port number
    @return Server IP:PORT address string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetServerAddressUrl();

/**
    Get the request URI
    @description This returns the request URI. This may be modified if the request is rewritten via websRewrite
    @param wp Webs request object
    @return URI string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetUrl(Webs *wp);

/**
    Get the client User-Agent HTTP header
    @param wp Webs request object
    @return User-Agent string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetUserAgent(Webs *wp);

/**
    Get the request username
    @description If the request is authenticated, this call returns the username supplied during authentication.
    @param wp Webs request object
    @return Username string if defined, otherwise null. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetUsername(Webs *wp);

/**
    Get a request variable
    @description Request variables are defined for HTTP headers of the form HTTP_*.
        Some request handlers also define their own variables. For example: CGI environment variables.
    @param wp Webs request object
    @param name Variable name
    @param defaultValue Default value to return if the variable is not defined
    @return Variable value string. Caller should not free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websGetVar(Webs *wp, char *name, char *defaultValue);

/**
    Listen on a TCP/IP address endpoint
    @description The URI is mapped to a filename by decoding and prepending with the request directory.
        For IPv6 addresses, use the format: [aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh:iiii]:port.
    @param endpoint IPv4 or IPv6 address on which to listen.
    @return Positive integer holding a Socket ID handle if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websListen(char *endpoint);

/**
    Get an MD5 digest of a string
    @param str String to analyze.
    @return Allocated MD5 checksum. Caller should free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websMD5(char *str);

/**
    Get an MD5 digest of a block and optionally prepend a prefix.
    @param buf Block to analyze
    @param length Length of block
    @param prefix Optional prefix to prepend to the MD5 sum.
    @return Allocated MD5 checksum. Caller should free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websMD5Block(char *buf, ssize length, char *prefix);

/**
    Normalize a URI path
    @description This removes "./", "../" and redundant separators.
    @param path URI path to normalize
    @return An allocated normalized URI path. Caller must free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websNormalizeUriPath(char *path);

/**
    Take not of the request activity and mark the time.
    @description This is used to defer the request timeout whenever there is request I/O activity.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websNoteRequestActivity(Webs *wp);

/**
    Close the runtime code.
    @description Called from websClose
    @ingroup Webs
    @internal
 */
PUBLIC void websRuntimeClose();

/**
    Open the runtime code.
    @description Called from websOpen
    @return Zero if successful
    @ingroup Webs
    @internal
 */
PUBLIC int websRuntimeOpen();

/**
    Open the web server
    @description This initializes the web server and defines the documents directory.
    @param documents Optional web documents directory. If set to null, the build time ME_GOAHEAD_DOCUMENTS value
        is used for the documents directory.
    @param routes Optional filename for a route configuration file to load. Additional route or
        authentication configuration files can be loaded via websLoad.
    @param routes Webs request object
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websOpen(char *documents, char *routes);

/**
    Close the O/S dependant code.
    @description Called from websClose
    @ingroup Webs
    @internal
 */
PUBLIC void websOsClose();

/**
    Open the O/S dependant code.
    @description Called from websOpen
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @internal
 */
PUBLIC int websOsOpen();

/**
    Open the web page document for the current request
    @param path Filename path to open
    @param flags File open flags
    @param mode Permissions mask
    @return Positive file handle if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websOpenFile(char *path, int flags, int mode);

/**
    Open the options handler
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websOptionsOpen();

/**
    Close the document page
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websPageClose(Webs *wp);

/**
    Test if the document page for the request corresponds to a directory
    @param wp Webs request object
    @return True if the filename is a directory
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websPageIsDirectory(Webs *wp);

/**
    Open a web page document for a request
    @param wp Webs request object
    @param mode File open mode. Select from O_RDONLY and O_BINARY. Rom files systems ignore this argument.
    @param perms Ignored
    @return File handle if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websPageOpen(Webs *wp, int mode, int perms);

/**
    Read data from the request page document
    @param wp Webs request object
    @param buf Buffer for the read data
    @param size Size of buf
    @return Count of bytes read if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize websPageReadData(Webs *wp, char *buf, ssize size);

/**
    Seek to a position in the request page document
    @param wp Webs request object
    @param offset Offset of location in the file to seek to. This is relative to the specified origin.
    @param origin Set to SEEK_CUR, SEEK_SET or SEEK_END to position relative to the current position,
        beginning or end of the document.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websPageSeek(Webs *wp, Offset offset, int origin);

/**
    Get file status for the current request document
    @param wp Webs request object
    @param sbuf File information structure to modify with file status
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websPageStat(Webs *wp, WebsFileInfo *sbuf);

#if !ME_ROM
/**
    Process request PUT body data
    @description This routine is called by the core HTTP engine to process request PUT data.
    @param wp Webs request object
    @return True if processing the request can proceed.
    @ingroup Webs
    @stability Stable
 */
PUBLIC bool websProcessPutData(Webs *wp);
#endif

/**
    Pump the state machine
    @description This routine will advance the connection state machine in response to events.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websPump(Webs *wp);

/**
    Define an action callback for use with the action handler.
    @description The action handler binds a C function to a URI under "/action".
    @param name URI path suffix. This suffix is added to "/action" to form the bound URI path.
    @param fun Callback function. The signature is void (*WebsAction)(Webs *wp);
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websDefineAction(cchar *name, void *fun);

/**
    Read data from an open file
    @param fd Open file handle returned by websOpenFile
    @param buf Buffer for the read data
    @param size Size of buf
    @return Count of bytes read if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize websReadFile(int fd, char *buf, ssize size);

/**
    Read all the data from a file
    @param path File path to read from
    @return An allocated buffer containing the file data with an appended null. Caller must free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websReadWholeFile(char *path);

/**
    Redirect the client to a new URL.
    @description This creates a response to the client with a Location header directing the client to a new location.
        The response uses a 302 HTTP status code.
    @param wp Webs request object
    @param url URL to direct the client to.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websRedirect(Webs *wp, char *url);

/**
    Redirect the client to a new URI
    @description The routing configuration file can define redirection routes for various HTTP status codes.
        This routine will utilize the appropriate route redirection based on the request route and specified status code.
    @param wp Webs request object
    @param status HTTP status code to use in selecting the route redirection.
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websRedirectByStatus(Webs *wp, int status);

/**
    Create and send a request response
    @description This creates a response for the current request using the specified HTTP status code and
        the supplied message.
    @param wp Webs request object
    @param status HTTP status code.
    @param msg Response message body
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websResponse(Webs *wp, int status, char *msg);

/**
    Rewrite a request
    @description Handlers may choose to not process a request but rather rewrite requests and then reroute.
    @param wp Webs request object
    @param url New request URL.
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websRewriteRequest(Webs *wp, char *url);

/**
    Open the file system module
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websFsOpen();

/**
    Close the file system module
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websFsClose();

/**
    Seek to a position in the current request page document
    @param fd Open file handle returned by websOpenFile
    @param offset Location in the file to seek to.
    @param origin Set to SEEK_CUR, SEEK_SET or SEEK_END to position relative to the current position,
        beginning or end of the document.
    @ingroup Webs
    @stability Stable
 */
PUBLIC Offset websSeekFile(int fd, Offset offset, int origin);

/**
    Get file status for a file
    @param path Filename path
    @param sbuf File information structure to modify with file status
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websStatFile(char *path, WebsFileInfo *sbuf);

/**
    One line embedding API.
    @description This call will also open auth.txt and route.txt for authentication and routing configuration.
    @param endpoint IP:PORT address on which to listen
    @param documents Directory containing web documents to serve
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websServer(char *endpoint, char *documents);

/**
    Service I/O events until finished
    @description This will wait for socket events and service those until *finished is set to true
    @param finished Integer location to test. If set to true, then exit. Note: setting finished will not
        automatically wake up the service routine.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websServiceEvents(int *finished);

/**
    Set the background processing flag
    @param on Value to set the background flag to.
    @ingroup Webs
    @internal
 */
PUBLIC void websSetBackground(int on);

/**
    Define a background write I/O event callback
    @param wp Webs request object
    @param proc Write callback
 */
PUBLIC void websSetBackgroundWriter(Webs *wp, WebsWriteProc proc);

/*
    Flags for websSetCookie
 */
#define WEBS_COOKIE_SECURE   0x1         /**< Flag for websSetCookie for secure cookies (https only) */
#define WEBS_COOKIE_HTTP     0x2         /**< Flag for websSetCookie for http cookies (http only) */

/**
    Define a cookie to include in the response
    @param wp Webs request object
    @param name Cookie name
    @param value Cookie value
    @param path URI path prefix applicable for this cookie
    @param domain Domain applicable for this cookie
    @param lifespan Cookie lifespan in seconds
    @param flags Set to WEBS_COOKIE_SECURE for https only. Set to WEBS_COOKIE_HTTP for http only.
        Otherwise the cookie applies to both http and https requests.
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetCookie(Webs *wp, char *name, char *value, char *path, char *domain, int lifespan, int flags);

/**
    Set the debug processing flag
    @param on Value to set the debug flag to.
    @ingroup Webs
    @internal
 */
PUBLIC void websSetDebug(int on);

/**
    Set the web documents directory
    @description The web documents directory is used when resolving request URIs into filenames.
    @param dir Directory path to use
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetDocuments(char *dir);

/**
    Create the CGI environment variables for the current request.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetEnv(Webs *wp);

/**
    Create request variables for query and POST body data
    @description This creates request variables if the request is a POST form (has a Content-Type of
        application/x-www-form-urlencoded). The POST body data is consumed from the input buffer.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetFormVars(Webs *wp);

/**
    Define the host name for the server
    @param host String host name
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetHost(char *host);

/**
    Define the host IP address
    @param ipaddr Host IP address
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetIpAddr(char *ipaddr);

/**
    Create and send a request response
    @description This creates a response for the current request using the specified HTTP status code and
        the supplied message.
    @param filename Web document name to use as the index. This should not contain any directory components.
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetIndex(char *filename);

/**
    Create request variables for query string data
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetQueryVars(Webs *wp);

/**
    Set the response HTTP status code
    @param wp Webs request object
    @param status HTTP status code
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetStatus(Webs *wp, int status);

/**
    Set the response body content length
    @param wp Webs request object
    @param length Length value to use
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websSetTxLength(Webs *wp, ssize length);

/**
    Set a request variable to a formatted string value
    @description Request variables are defined for HTTP headers of the form HTTP_*.
        Some request handlers also define their own variables. For example: CGI environment variables.
    @param wp Webs request object
    @param name Variable name to set
    @param fmt Value format string
    @param ... Args to format
    @return the allocated WebsKey
    @ingroup Webs
    @stability Evolving
 */
PUBLIC WebsKey *websSetVarFmt(Webs *wp, char *name, char *fmt, ...);

/**
    Set a request variable to a string value
    @description Request variables are defined for HTTP headers of the form HTTP_*.
        Some request handlers also define their own variables. For example: CGI environment variables.
    @param wp Webs request object
    @param name Variable name to set
    @param value Value to set
    @return the allocated WebsKey
    @ingroup Webs
    @stability Evolving
 */
PUBLIC WebsKey *websSetVar(Webs *wp, char *name, char *value);

/**
    Test if  a request variable is defined
    @param wp Webs request object
    @param name Variable name
    @return True if the variable is defined
    @ingroup Webs
    @stability Stable
 */
PUBLIC bool websTestVar(Webs *wp, char *name);

/**
    Create a temporary filename
    This does not guarantee the filename is unique or that it is not already in use by another application.
    @param dir Directory to locate the temp file. Defaults to the O/S default temporary directory (usually /tmp)
    @param prefix Filename prefix
    @return An allocated filename string
    @ingroup Webs
    @stability Stable
  */
PUBLIC char *websTempFile(char *dir, char *prefix);

/**
    Open the date/time parsing module
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Evolving
 */
PUBLIC int websTimeOpen();

/**
    Close the date/time parsing module
    @ingroup Webs
    @stability Evolving
*/
PUBLIC void websTimeClose();

/**
    Parse a date/time string
    @description Try to intelligently parse a date.
    This is a tolerant parser. It is not validating and will do its best to parse any possible date string.
    Supports the following date/time formats:
    \n\n
        ISO dates: 2009-05-21t16:06:05.000z
    \n\n
        Date:  07/28/2014, 07/28/08, Jan/28/2014, Jaunuary-28-2014, 28-jan-2014.
    \n\n
        Support date order: dd/mm/yy, mm/dd/yy and yyyy/mm/dd
    \n\n
        Support separators "/", ".", "-"
    \n\n
        Timezones: GMT|UTC[+-]NN[:]NN
    \n\n
        Time: 10:52[:23]
    \n\n
    @param time Reference to a
    @param date Date/time string to parse
    @param defaults Optionally supply missing components for the date/time. Set to NULL if not used.
    @return Zero if successful, otherwise -1
    @ingroup Webs
    @stability Evolving
  */
PUBLIC int websParseDateTime(WebsTime *time, char *date, struct tm *defaults);

/**
    Parse a URL into its components
    @param url URL to parse
    @param buf Buffer to hold storage for various parsed components. Caller must free. NOTE: the parsed components may
        point to locations in this buffer.
    @param protocol Parsed URL protocol component
    @param host Parsed hostname
    @param port Parsed URL port
    @param path Parsed URL path component
    @param ext Parsed URL extension
    @param reference Parsed URL reference portion (\#reference)
    @param query Parsed URL query component
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Evolving
 */
PUBLIC int websUrlParse(char *url, char **buf, char **protocol, char **host, char **port, char **path, char **ext,
                        char **reference, char **query);

/**
    Test if a webs object is valid
    @description After calling websDone, the websFree routine will have been called and the memory for the webs object
        will be released. Call websValid to test a Webs object for validity.
    @param wp Webs request object
    @return True if the webs object is still valid and the request has not been completed.
    @ingroup Webs
    @stability Stable
 */
PUBLIC bool websValid(Webs *wp);

/**
    Validate a URI path as expected in a HTTP request line
    @description This expects a URI beginning with "/" and containing only valid URI characters.
    The URI is decoded, and normalized removing "../" and "." segments.
    The URI must begin with a "/" both before and after decoding and normalization.
    @param uri URI to validate.
    @return A validated, normalized URI path. Caller must free.
    @ingroup Webs
    @stability Stable
 */
PUBLIC char *websValidateUriPath(char *uri);

/**
    Test if a URI is using only valid characters
    Note this does not test if the URI is fully legal. Some components of the URI have restricted character sets
    that this routine does not test. This tests if the URI has only characters valid to use in a URI before decoding.
    i.e. It will permit %NN encodings. The set of valid characters is:
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%"
    @param uri Uri to test
    @return True if the URI string is comprised of legal URI characters.
    @ingroup Webs
    @stability Evolving
  */
PUBLIC bool websValidUriChars(char *uri);

/**
    Write a set of standard response headers
    @param wp Webs request object
    @param contentLength Value for the Content-Length header which describes the length of the response body
    @param redirect Value for the Location header which redirects the client to a new URL.
    @ingroup Webs
    @see websSetStatus
    @stability Stable
 */
PUBLIC void websWriteHeaders(Webs *wp, ssize contentLength, char *redirect);

/**
    Signify the end of the response headers
    @description This call concludes the response headers and writes a blank line to the response.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websWriteEndHeaders(Webs *wp);

/**
    Write a response header
    @description This routine writes a response header. It should be invoked after calling websWriteHeaders
        to write the standard headers and before websWriteEndHeaders.
        This routine differs from websWrite in that it traces header values to the log.
    @param wp Webs request object
    @param key Header key value
    @param fmt Header value format string.
    @param ... Arguments to the format string.
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websWriteHeader(Webs *wp, char *key, char *fmt, ...);

/**
    Write data to the response
    @description The data is buffered and will be sent to the client when the buffer is full or websFlush is
        called.
    @param wp Webs request object
    @param fmt Printf style format string.
    @param ... Arguments to the format string.
    @return Count of bytes written
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize websWrite(Webs *wp, char *fmt, ...);

/**
    Write data to the open file
    @param fd Open file handle returned by websOpenFile
    @param buf Buffer for the read data
    @param size Size of buf
    @return Count of bytes read if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize websWriteFile(int fd, char *buf, ssize size);

/**
    Write a block of data to the response
    @description The data is buffered and will be sent to the client when the buffer is full or websFlush is
        called. This routine will never return "short", it will always write all the data unless there are errors.
    @param wp Webs request object
    @param buf Buffer of data to write
    @param size Length of buf
    @return Count of bytes written or -1. This will always equal size if there are no errors.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize websWriteBlock(Webs *wp, char *buf, ssize size);

/**
    Write a block of data to the network
    @description This bypassed output buffering and is the lowest level write.
    @param wp Webs request object
    @param buf Buffer of data to write
    @param size Length of buf
    @return Count of bytes written. May be less than len if the socket is in non-blocking mode.
        Returns -1 for errors and if the socket cannot absorb any more data. If the transport is saturated,
        will return a negative error and errno will be set to EAGAIN or EWOULDBLOCK.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize websWriteSocket(Webs *wp, char *buf, ssize size);

#if ME_GOAHEAD_UPLOAD
/**
    Process upload data for form, multipart mime file upload.
    @param wp Webs request object
    @return True if processing the request can proceed.
    @ingroup Webs
    @stability Stable
 */
PUBLIC bool websProcessUploadData(Webs *wp);

/**
    Free file upload data structures.
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void websFreeUpload(Webs *wp);
#endif

#if ME_GOAHEAD_CGI
/**
    Process CGI request body data.
    @param wp Webs request object
    @return True if processing the request can proceed.
    @ingroup Webs
    @stability Stable
 */
PUBLIC bool websProcessCgiData(Webs *wp);
#endif

/************************************** Crypto ********************************/

/**
    Get some random data
    @param buf Reference to a buffer to hold the random data
    @param length Size of the buffer
    @param block Set to true if it is acceptable to block while accumulating entropy sufficient to provide good
        random data. Setting to false will cause this API to not block and may return random data of a lower quality.
    @ingroup Crypto
    @stability Prototype.
  */
PUBLIC int websGetRandomBytes(char *buf, ssize length, bool block);

/**
    Encrypt a password using the Blowfish algorithm
    @param password User's password to encrypt
    @param salt Salt text to add to password. Helps to make each user's password unique.
    @param rounds Number of times to encrypt. More times, makes the routine slower and passwords harder to crack.
    @return The encrypted password.
    @ingroup Crypto
    @stability Prototype
 */
PUBLIC char *websCryptPassword(char *password, char *salt, int rounds);

/**
    Make salt for adding to a password.
    @param size Size in bytes of the salt text.
    @return The random salt text.
    @ingroup Crypto
    @stability Prototype
 */
PUBLIC char *websMakeSalt(ssize size);

/**
    Make a password hash for a plain-text password using the Blowfish algorithm.
    @param password User's password to encrypt
    @param saltLength Length of salt text to add to password. Helps to make each user's password unique.
    @param rounds Number of times to encrypt. More times, makes the routine slower and passwords harder to crack.
    @return The encrypted password.
    @ingroup Crypto
    @stability Prototype
 */
PUBLIC char *websMakePassword(char *password, int saltLength, int rounds);

/**
    Check a plain-text password against the defined hashed password.
    @param plainTextPassword User's plain-text-password to check
    @param passwordHash Required password in hashed format previously computed by websMakePassword.
    @return True if the password is correct.
    @ingroup Crypto
    @stability Prototype
 */
PUBLIC bool websCheckPassword(char *plainTextPassword, char *passwordHash);

/**
    Get a password from the terminal console
    @param prompt Text prompt to display before reading the password
    @return The entered password.
    @ingroup Crypto
    @stability Prototype
 */
PUBLIC char *websReadPassword(char *prompt);

/*************************************** JST ***********************************/

#if ME_GOAHEAD_JAVASCRIPT
/**
    Javascript native function
    @param jid JavaScript engine ID
    @param wp Webs request object
    @param argc Count of function arguments
    @param argv Array of function arguments
    @param defaultValue Default value to return if the variable is not defined
    @return Return zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
typedef int (*WebsJstProc)(int jid, Webs *wp, int argc, char **argv);

/**
    Define a Javscript native function.
    @description This routine binds a C function to a Javascript function. When the Javascript function is called,
        the C function is invoked.
    @param name Javascript function name
    @param fn C function to invoke
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websDefineJst(char *name, WebsJstProc fn);

/**
    Open the Javascript module.
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websJstOpen();

/**
    Write data to the response
    @param jid Javascript ID handle
    @param wp Webs request object
    @param argc Count of arguments
    @param argv Array arguments
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int websJstWrite(int jid, Webs *wp, int argc, char **argv);
#endif

/*************************************** SSL ***********************************/

#if ME_COM_SSL
/*
    Default MakeMe settings
 */
#ifndef ME_GOAHEAD_SSL_AUTHORITY
#define ME_GOAHEAD_SSL_AUTHORITY ""
#endif
#ifndef ME_GOAHEAD_SSL_CACHE
#define ME_GOAHEAD_SSL_CACHE 512
#endif
#ifndef ME_GOAHEAD_SSL_CERTIFICATE
#define ME_GOAHEAD_SSL_CERTIFICATE ""
#endif
#ifndef ME_GOAHEAD_SSL_CIPHERS
#define ME_GOAHEAD_SSL_CIPHERS ""
#endif
#ifndef ME_GOAHEAD_SSL_KEY
#define ME_GOAHEAD_SSL_KEY ""
#endif
#ifndef ME_GOAHEAD_SSL_LOG_LEVEL
#define ME_GOAHEAD_SSL_LOG_LEVEL 4
#endif
#ifndef ME_GOAHEAD_SSL_RENEGOTIATE
#define ME_GOAHEAD_SSL_RENEGOTIATE 1
#endif
#ifndef ME_GOAHEAD_SSL_REVOKE
#define ME_GOAHEAD_SSL_REVOKE ""
#endif
#ifndef ME_GOAHEAD_SSL_TICKET
#define ME_GOAHEAD_SSL_TICKET 1
#endif
#ifndef ME_GOAHEAD_SSL_TIMEOUT
#define ME_GOAHEAD_SSL_TIMEOUT 86400
#endif
#ifndef ME_GOAHEAD_SSL_VERIFY
#define ME_GOAHEAD_SSL_VERIFY 0
#endif
#ifndef ME_GOAHEAD_SSL_VERIFY_ISSUER
#define ME_GOAHEAD_SSL_VERIFY_ISSUER 0
#endif

/**
    Open the ssl module
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int sslOpen();

/**
    Close the ssl module
    @ingroup Webs
    @stability Stable
 */
PUBLIC void sslClose();

/**
    Free a ssl connection associated with a request
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
PUBLIC void sslFree(Webs *wp);

/**
    Upgrade a request connection to utilize SSL
    @description This routine is invoked on a connection received on a secure listening socket
    @param wp Webs request object
    @return Zero if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC int sslUpgrade(Webs *wp);

/**
    Read data from a secure socket
    @param wp Webs request object
    @param buf Buffer into which to read data
    @param len Size of buf
    @return Count of bytes read if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize sslRead(Webs *wp, void *buf, ssize len);

/**
    WRite data to a secure socket
    @param wp Webs request object
    @param buf Buffer from which to write data
    @param len Size of buf
    @return Count of bytes written if successful, otherwise -1.
    @ingroup Webs
    @stability Stable
 */
PUBLIC ssize sslWrite(Webs *wp, void *buf, ssize len);
#endif /* ME_COM_SSL */

/*************************************** Route *********************************/
/**
    Callback to prompt the user for their password
    @param wp Webs request object
    @ingroup Webs
    @stability Stable
 */
typedef void (*WebsAskLogin)(Webs *wp);

/**
    Callback to verify the username and password
    @param wp Webs request object
    @return True if the password is verified
    @ingroup Webs
    @stability Stable
 */
typedef bool (*WebsVerify)(Webs *wp);

/**
    Callback to parse authentication details submitted with the web request
    @param wp Webs request object
    @return True if the details can be parsed
    @ingroup Webs
    @stability Stable
 */
typedef bool (*WebsParseAuth)(Webs *wp);

/**
    Request route structure
    @defgroup WebsRoute WebsRoute
 */
typedef struct WebsRoute
{
    char            *prefix;                /**< Route path prefix */
    ssize           prefixLen;              /**< Prefix length */
    char            *dir;                   /**< Filesystem base directory for route documents */
    char            *protocol;              /**< HTTP protocol to use for this route */
    char            *authType;              /**< Authentication type */
    WebsHandler     *handler;               /**< Request handler to service requests */
    WebsHash        abilities;              /**< Required user abilities */
    WebsHash        extensions;             /**< Permissible URI extensions */
    WebsHash        redirects;              /**< Response redirections */
    WebsHash        methods;                /**< Supported HTTP methods */
    WebsAskLogin    askLogin;               /**< Route path prefix */
    WebsParseAuth   parseAuth;              /**< Parse authentication details callback*/
    WebsVerify      verify;                 /**< Verify password callback */
    int             flags;                  /**< Route control flags */
} WebsRoute;

/**
    Add a route to the routing tables
    @param uri Matching URI prefix
    @param handler Request handler to service routed requests
    @param pos Position in the list of routes. Zero inserts at the front of the list. A value of -1 will append to the
        end of the list.
    @return A route object
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC WebsRoute *websAddRoute(char *uri, char *handler, int pos);

/**
    Close the route module
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC void websCloseRoute();

/**
    Load routing tables from the specified filename
    @param path Route configuration filename
    @return Zero if successful, otherwise -1.
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC int websLoad(char *path);

/**
    Open the routing module
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC int websOpenRoute();

/**
    Remove a route from the routing tables
    @param uri Matching URI prefix
    @return Zero if successful, otherwise -1.
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC int websRemoveRoute(char *uri);

/**
    Route a request
    @description This routine will select a matching route and will invoke the selected route handler to service
        the request. In the process, authentication and request rewriting may take place.
        This routine is called internally by the request pipeline.
    @param wp Webs request object
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC void websRouteRequest(Webs *wp);

/**
    Run a request handler
    @description This routine will run the handler and route selected by #websRouteRequest.
        This routine is called internally by the request pipeline.
    @param wp Webs request object
    @return True if the handler serviced the request. Return false to test other routes to handle this request.
        This is for legacy handlers that do not have a match callback.
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC bool websRunRequest(Webs *wp);

/**
    Configure a route by adding matching criteria
    @param route Route to modify
    @param dir Set the route documents directory filename
    @param protocol Set the matching HTTP protocol (http or https)
    @param methods Hash of permissible HTTP methods. (GET, HEAD, POST, PUT)
    @param extensions Hash of permissible URI filename extensions.
    @param abilities Required user abilities. The user must be authenticated.
    @param abilities Required user abilities. If abilities are required, the user must be authenticated.
    @param redirects Set of applicable response redirections when completing the request.
    @return Zero if successful, otherwise -1.
    @ingroup WebsRoute
    @stability Evolving
 */
PUBLIC int websSetRouteMatch(WebsRoute *route, char *dir, char *protocol, WebsHash methods, WebsHash extensions,
                             WebsHash abilities, WebsHash redirects);

/**
    Set route authentication scheme
    @param route Route to modify
    @param authType Set to "basic", "digest" or "form".
    @return Zero if successful, otherwise -1.
    @ingroup WebsRoute
    @stability Stable
 */
PUBLIC int websSetRouteAuth(WebsRoute *route, char *authType);

/*************************************** Auth **********************************/
#if ME_GOAHEAD_AUTH

#define WEBS_USIZE          128              /* Size of realm:username */

/**
    GoAhead Authentication
    @defgroup WebsAuth WebsAuth
 */
/**
    User definition structure
    @ingroup WebsAuth
    @stability Stable
 */
typedef struct WebsUser
{
    char    *name;                          /**< User name */
    char    *password;                      /**< User password (encrypted) */
    char    *roles;                         /**< User roles */
    WebsHash abilities;                     /**< Resolved user abilities */
} WebsUser;

/**
    Role definition structure
    @ingroup WebsAuth
    @stability Stable
 */
typedef struct WebsRole
{
    WebsHash  abilities;                    /**< Resolved role abilities */
} WebsRole;

/**
    Add a role
    @description The role is added to the list of roles
    @param role Role name
    @param abilities Hash of abilities for the role
    @return The allocated role.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC WebsRole *websAddRole(char *role, WebsHash abilities);

/**
    Add a user
    @description The user is added to the list of users
    @param username User name
    @param password User password (encrypted)
    @param roles Space separated list of roles. This may also contain abilities.
    @return User object.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC WebsUser *websAddUser(char *username, char *password, char *roles);

/**
    Authenticate a user
    @description The user is authenticated if required by the selected request route.
    @param wp Webs request object
    @return True if the route does not require authentication or the user is authenticated successfully.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websAuthenticate(Webs *wp);

/**
    Test if a user possesses the required ability
    @param wp Webs request object
    @param ability Set of required abilities.
    @return True if the user has the required ability.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websCan(Webs *wp, WebsHash ability);

/**
    Close the authentication module
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC void websCloseAuth();

/**
    Compute the abilities for all users by resolving roles into abilities
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC void websComputeAllUserAbilities();

/**
    Set the password store verify callback
    @return verify WebsVerify callback function
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC WebsVerify websGetPasswordStoreVerify();

/**
    Get the roles hash
    @return The roles hash object
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC WebsHash websGetRoles();

/**
    Get the users hash
    @return The users hash object
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC WebsHash websGetUsers();

/**
    Login a user by verifying the login credentials.
    @description This may be called by handlers to manually authenticate a user.
    @param wp Webs request object
    @param username User name
    @param password User password (encrypted)
    @return True if the user can be authenticated.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websLoginUser(Webs *wp, char *username, char *password);

/**
    Logout a user and remove the user login session.
    @param wp Webs request object
    @return True if successful.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websLogoutUser(Webs *wp);

/**
    Lookup if a user exists
    @param username User name to search for
    @return User object or null if the user cannot be found
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC WebsUser *websLookupUser(char *username);

/**
    Remove a role from the system
    @param role Role name
    @return Zero if successful, otherwise -1
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC int websRemoveRole(char *role);

/**
    Remove a user from the system
    @param name User name
    @return Zero if successful, otherwise -1
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC int websRemoveUser(char *name);

/**
    Open the authentication module
    @param minimal Reserved. Set to zero.
    @return True if the user has the required ability.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC int websOpenAuth(int minimal);

/**
    Set the password store verify callback
    @param verify WebsVerify callback function
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC void websSetPasswordStoreVerify(WebsVerify verify);

/**
    Set a password for the user
    @param username User name
    @param password Null terminated password string
    @return Zero if successful, otherwise -1.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC int websSetUserPassword(char *username, char *password);


/**
    Define the set of roles for a user
    @param username User name
    @param roles Space separated list of roles or abilities
    @return Zero if successful, otherwise -1.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC int websSetUserRoles(char *username, char *roles);

/**
    User password verification routine from a custom password back-end store.
    @param wp Webs request object
    @return True if the user password verifies.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websVerifyPasswordFromCustom(Webs *wp);

/**
    User password verification routine from auth.txt
    @param wp Webs request object
    @return True if the user password verifies.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websVerifyPasswordFromFile(Webs *wp);

#if ME_COMPILER_HAS_PAM
/**
    Verify a password using the system PAM password database.
    @param wp Webs request object
    @return True if the user password verifies.
    @ingroup WebsAuth
    @stability Stable
 */
PUBLIC bool websVerifyPasswordFromPam(Webs *wp);
#endif

#endif /* ME_GOAHEAD_AUTH */
/************************************** Sessions *******************************/
/**
    Session state storage
    @defgroup WebsSession WebsSession
 */
typedef struct WebsSession
{
    char            *id;                    /**< Session ID key */
    int             lifespan;               /**< Session inactivity timeout (secs) */
    WebsTime        expires;                /**< When the session expires */
    WebsHash        cache;                  /**< Cache of session variables */
} WebsSession;

/**
    Test if a user possesses the required ability
    @param wp Webs request object
    @param id Session ID to use. Set to null to allocate a new session ID.
    @param lifespan Lifespan of the session in seconds.
    @return Allocated session object
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC WebsSession *websAllocSession(Webs *wp, char *id, int lifespan);

/**
    Test if a user possesses the required ability
    @param wp Webs request object
    @return Allocated session object
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC WebsSession *websCreateSession(Webs *wp);

/**
    Destroy the webs session object
    @description Useful to be called as part of the user logout process
    @param wp Webs request object
    @ingroup WebsSession
    @stability Prototype
 */
PUBLIC void websDestroySession(Webs *wp);

/**
    Get the session ID
    @param wp Webs request object
    @return The session ID if session state storage is defined for this request.
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC char *websGetSessionID(Webs *wp);

/**
    Get the session state object for the current request
    @param wp Webs request object
    @param create Set to true to create a new session if one does not already exist.
    @return Session object
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC WebsSession *websGetSession(Webs *wp, int create);

/**
    Get a session variable
    @param wp Webs request object
    @param name Session variable name
    @param defaultValue Default value to return if the variable does not exist
    @return Session variable value or default value if it does not exist
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC char *websGetSessionVar(Webs *wp, char *name, char *defaultValue);

/**
    Remove a session variable
    @param wp Webs request object
    @param name Session variable name
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC void websRemoveSessionVar(Webs *wp, char *name);

/**
    Set a session variable name value
    @param wp Webs request object
    @param name Session variable name
    @param value Value to set the variable to
    @return Zero if successful, otherwise -1
    @ingroup WebsSession
    @stability Stable
 */
PUBLIC int websSetSessionVar(Webs *wp, char *name, char *value);

/************************************ Legacy **********************************/
/*
    Legacy mappings for pre GoAhead 3.X applications
    This is a list of the name changes from GoAhead 2.X to GoAhead 3.x
    To maximize forward compatibility, It is best to not use ME_GOAHEAD_LEGACY except as
    a transitional compilation aid.
 */
#if ME_GOAHEAD_LEGACY
#define B_L 0
#define a_assert assert
#define balloc walloc
#define bclose wcloseAlloc
#define bfree(loc, p) wfree(p)
#define bfreeSafe(loc, p) wfree(p)
#define bopen wopenAlloc
#define brealloc wrealloc
#define bstrdup sclone
#define emfReschedCallback websRestartEvent
#define emfSchedCallback websStartEvent
#define emfSchedProc WebsEventProc
#define emfSchedProcess websRunEvents
#define emfUnschedCallback websStopEvent
#define fmtStatic fmt
#define gassert assert
#define galloc walloc
#define gallocEntry wallocObject
#define gfree wfree
#define gFree wfreeHandle
#define grealloc wrealloc
#define gaccess access
#define gasctime asctime
#define gatoi atoi
#define gchmod chmod
#define wclose close
#define wclosedir closedir
#define gcreat creat
#define gctime ctime
#define gexecvp execvp
#define gfgets fgets
#define gfindclose _findclose
#define gfinddata_t _finddata_t
#define gfindfirst _findfirst
#define gfindnext _findnext
#define gfopen fopen
#define gfprintf fprintf
#define gfputs fputs
#define gfscanf fscanf
#define ggetcwd getcwd
#define ggetenv getenv
#define ggets gets
#define gisalnum isalnum
#define gisalpha isalpha
#define gisdigit isdigit
#define gislower islower
#define gisspace isspace
#define gisupper isupper
#define gisxdigit isxdigit
#define gloadModule loadModule
#define glseek lseek
#define gopendir opendir
#define gprintf printf
#define gread read
#define greaddir readdir
#define gremove remove
#define grename rename
#define gsprintf sprintf
#define gsscanf sscanf
#define gstat stat
#define gstrcat strcat
#define gstrchr strchr
#define gstrcmp strcmp
#define gstrcpy strcpy
#define gstrcspn strcspn
#define gstricmp strcmpci
#define gstritoa stritoa
#define gstrlen strlen
#define gstrlower strlower
#define gstrncat strncat
#define gstrncmp strncmp
#define gstrncpy strncpy
#define gstrnlen strnlen
#define gstrnset strnset
#define gstrrchr strrchr
#define gstrspn strspn
#define gstrstr strstr
#define gstrtok strtok
#define gstrtol strtol
#define gstrupper strupper
#define gtempnam websTempFile
#define gtolower tolower
#define gtoupper toupper
#define gunlink unlink
#define gvsprintf vsprintf
#define gwrite write
#define hAlloc wallocHandle
#define hAllocEntry wallocObject
#define hFree wfreeHandle
#define stritoa gstritoa
#define strlower gstrlower
#define strupper gstrupper
#define websAspDefine websDefineJst
#define websAspOpen websJstOpen
#define websAspRequest websJstRequest
#define websFormDefine websDefineAction
#define websGetDefaultDir websGetDocuments
#define websGetDefaultPage websGetIndex

#define websGetRequestDir(wp) wp->dir
#define websGetRequestIpAddr(wp) wp->ipaddr
#define websGetRequestFilename(wp) wp->filename
#define websGetRequestFlags(wp) wp->flags
#define websGetRequestLpath(wp) wp->filename
#define websGetRequestPath(wp) wp->path
#define websGetRequestPassword(wp) wp->password
#define websGetRequestUserName(wp) wp->username
#define websGetRequestWritten(wp) wp->written

#define websSetDefaultDir websSetDocuments
#define websSetDefaultPage websSetIndex
#define websSetRequestLpath websSetRequestFilename
#define websSetRequestWritten(wp, nbytes) if (1) { wp->written = nbytes; } else {}
#define websTimeoutCancel websCancelTimeout
#define websWriteDataNonBlock websWriteRaw

#define ringqOpen bufCreate
#define ringqClose bufFree
#define ringqLen bufLen
#define ringqPutc bufPutc
#define ringqInsertc bufInsertc
#define ringqPutStr bufPutStr
#define ringqGetc bufGetc
#define ringqGrow bufGrow
#define ringqPutBlk bufPutBlk
#define ringqPutBlkMax bufRoom
#define ringqPutBlkAdj bufAdjustEnd
#define ringqGetBlk bufGetBlk
#define ringqGetBlkMax bufGetBlkMax
#define ringqGetBlkAdj bufAdjustSTart
#define ringqFlush bufFlush
#define ringqCompact bufCompact
#define ringqReset bufReset
#define ringqAddNull bufAddNull

#define symCreate hashCreate
#define symClose hashFree
#define symLookup hashLookup
#define symEnter hashEnter
#define symDelete hashDelete
#define symWalk hashWalk
#define symFirst hashFirst
#define symNext hashNext

typedef Webs *webs_t;
typedef Webs WebsRec;
typedef Webs websType;
typedef WebsBuf ringq_t;
typedef WebsError websErrorType;
typedef WebsProc WebsFormProc;
typedef int (*WebsLegacyHandlerProc)(Webs *wp, char *prefix, char *dir, int flags);
typedef SocketHandler socketHandler_t;
typedef SocketAccept socketAccept_t;
typedef WebsType vtype_t;

typedef WebsHash sym_fd_t;
typedef WebsKey sym_t;
typedef WebsMime websMimeType;
typedef WebsSocket socket_t;
typedef WebsStat gstat_t;
typedef WebsValue value_t;

PUBLIC int fmtValloc(char **s, int n, char *fmt, va_list arg);
PUBLIC int fmtAlloc(char **s, int n, char *fmt, ...);
PUBLIC void websFooter(Webs *wp);
PUBLIC void websHeader(Webs *wp);
PUBLIC int websPublish(char *prefix, char *path);
PUBLIC void websSetRequestFilename(Webs *wp, char *filename);
PUBLIC int websUrlHandlerDefine(char *prefix, char *dir, int arg, WebsLegacyHandlerProc handler, int flags);

#if ME_ROM
typedef WebsRomIndex websRomIndexType;
#endif
#endif

#if ME_CUSTOMIZE
#include "customize.h"
#endif


/*-----------------------------------------add by lyj 10.29*/

typedef void(*WebsActProc)(Webs *wp);

typedef struct __go_asp_func_t
{
    int func_type;   // 1:jst 2:action
    char fn_name[32];
    WebsJstProc      jst_fn;
    WebsActProc      act_fn;
} go_asp_func_t;


typedef struct __go_asp_func_reg_t
{
   int func_num;
   go_asp_func_t  funcList[128];
} go_asp_func_reg_t;


int start_goahead(char *home, char *documents, char *endpoint, char *route, char *auth, go_asp_func_t *pFunc, int FuncNum);

/*------------------------------------------*/
WebsSession *websCheckSession(Webs *wp, char *session_ptr,  int *session_len);		//add by wenyi 2019-01-25


#ifdef __cplusplus
}
#endif
#endif /* _h_GOAHEAD */

/*
    Copyright (c) Embedthis Software. All Rights Reserved.
    This software is distributed under commercial and open source licenses.
    You may use the Embedthis GoAhead open source license or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.
 */
