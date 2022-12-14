/**
    osdep.h -- O/S abstraction for products using MakeMe. 
 */

#ifndef _h_OSDEP
#define _h_OSDEP 1

/********************************** Includes **********************************/

#include "me.h"

/******************************* Default Features *****************************/
/*
    MakeMe defaults
 */
#ifndef ME_COM_SSL
    #define ME_COM_SSL 0                /**< Build without SSL support */
#endif
#ifndef ME_DEBUG
    #define ME_DEBUG 0                  /**< Default to a debug build */
#endif
#ifndef ME_FLOAT
    #define ME_FLOAT 1                  /**< Build with floating point support */
#endif
#ifndef ME_ROM
    #define ME_ROM 0                    /**< Build for execute from ROM */
#endif

/********************************* CPU Families *******************************/
/*
    CPU Architectures
 */
#define ME_CPU_UNKNOWN     0
#define ME_CPU_ARM         1           /**< Arm */
#define ME_CPU_ITANIUM     2           /**< Intel Itanium */
#define ME_CPU_X86         3           /**< X86 */
#define ME_CPU_X64         4           /**< AMD64 or EMT64 */
#define ME_CPU_MIPS        5           /**< Mips */
#define ME_CPU_PPC         6           /**< Power PC */
#define ME_CPU_SPARC       7           /**< Sparc */
#define ME_CPU_TIDSP       8           /**< TI DSP */
#define ME_CPU_SH          9           /**< SuperH */

/*
    Byte orderings
 */
#define ME_LITTLE_ENDIAN   1           /**< Little endian byte ordering */
#define ME_BIG_ENDIAN      2           /**< Big endian byte ordering */

/*
    Use compiler definitions to determine the CPU type. 
    The default endianness can be overridden by configure --endian big|little.
 */
#if defined(__alpha__)
    #define ME_CPU "alpha"
    #define ME_CPU_ARCH ME_CPU_ALPHA
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#elif defined(__arm__)
    #define ME_CPU "arm"
    #define ME_CPU_ARCH ME_CPU_ARM
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#elif defined(__x86_64__) || defined(_M_AMD64)
    #define ME_CPU "x64"
    #define ME_CPU_ARCH ME_CPU_X64
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#elif defined(__i386__) || defined(__i486__) || defined(__i585__) || defined(__i686__) || defined(_M_IX86)
    #define ME_CPU "x86"
    #define ME_CPU_ARCH ME_CPU_X86
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#elif defined(_M_IA64)
    #define ME_CPU "ia64"
    #define ME_CPU_ARCH ME_CPU_ITANIUM
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#elif defined(__mips__) || defined(__mips64)
    #define ME_CPU "mips"
    #define ME_CPU_ARCH ME_CPU_MIPS
    #define CPU_ENDIAN ME_BIG_ENDIAN

#elif defined(__ppc__) || defined(__powerpc__) || defined(__ppc64__) || defined(__ppc)
    #define ME_CPU "ppc"
    #define ME_CPU_ARCH ME_CPU_PPC
    #define CPU_ENDIAN ME_BIG_ENDIAN

#elif defined(__sparc__)
    #define ME_CPU "sparc"
    #define ME_CPU_ARCH ME_CPU_SPARC
    #define CPU_ENDIAN ME_BIG_ENDIAN

#elif defined(_TMS320C6X)
    #define TIDSP 1
    #define ME_CPU "tidsp"
    #define ME_CPU_ARCH ME_CPU_SPARC
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#elif defined(__sh__)
    #define ME_CPU "sh"
    #define ME_CPU_ARCH ME_CPU_SH
    #define CPU_ENDIAN ME_LITTLE_ENDIAN

#else
    #error "Cannot determine CPU type in osdep.h"
#endif

/*
    Set the default endian if me.h does not define it explicitly
 */
#ifndef ME_ENDIAN
    #define ME_ENDIAN CPU_ENDIAN
#endif

/*
    Operating system defines. Use compiler standard defintions to sleuth.  Works for all except VxWorks which does not
    define any special symbol.  NOTE: Support for SCOV Unix, LynxOS and UnixWare is deprecated. 
 */
#if defined(__APPLE__)
    #define ME_OS "macosx"
    #define MACOSX 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0
    #define ME_BSD_LIKE 1
    #define HAS_USHORT 1
    #define HAS_UINT 1

#elif defined(__linux__)
    #define ME_OS "linux"
    #define LINUX 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0

#elif defined(__FreeBSD__)
    #define ME_OS "freebsd"
    #define FREEBSD 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0
    #define ME_BSD_LIKE 1

#elif defined(__OpenBSD__)
    #define ME_OS "freebsd"
    #define OPENBSD 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0
    #define ME_BSD_LIKE 1

#elif defined(_WIN32)
    #define ME_OS "windows"
    #define WINDOWS 1
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 1

#elif defined(__OS2__)
    #define ME_OS "os2"
    #define OS2 0
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0

#elif defined(MSDOS) || defined(__DME__)
    #define ME_OS "msdos"
    #define WINDOWS 0
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0

#elif defined(__NETWARE_386__)
    #define ME_OS "netware"
    #define NETWARE 0
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0

#elif defined(__bsdi__)
    #define ME_OS "bsdi"
    #define BSDI 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0
    #define ME_BSD_LIKE 1

#elif defined(__NetBSD__)
    #define ME_OS "netbsd"
    #define NETBSD 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0
    #define ME_BSD_LIKE 1

#elif defined(__QNX__)
    #define ME_OS "qnx"
    #define QNX 0
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0

#elif defined(__hpux)
    #define ME_OS "hpux"
    #define HPUX 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0

#elif defined(_AIX)
    #define ME_OS "aix"
    #define AIX 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0

#elif defined(__CYGWIN__)
    #define ME_OS "cygwin"
    #define CYGWIN 1
    #define ME_UNIX_LIKE 1
    #define ME_WIN_LIKE 0

#elif defined(__VMS)
    #define ME_OS "vms"
    #define VMS 1
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0

#elif defined(VXWORKS)
    /* VxWorks does not have a pre-defined symbol */
    #define ME_OS "vxworks"
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0
    #define HAS_USHORT 1

#elif defined(ECOS)
    /* ECOS may not have a pre-defined symbol */
    #define ME_OS "ecos"
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0

#elif defined(TIDSP) 
    #define ME_OS "tidsp"
    #define ME_UNIX_LIKE 0
    #define ME_WIN_LIKE 0
    #define HAS_INT32 1

#endif

#if __WORDSIZE == 64 || __amd64 || __x86_64 || __x86_64__ || _WIN64 || __mips64
    #define ME_64 1
    #define ME_WORDSIZE 64
#else
    #define ME_64 0
    #define ME_WORDSIZE 32
#endif

/*
    Unicode
 */
#ifndef ME_CHAR_LEN
    #define ME_CHAR_LEN 1
#endif
#if ME_CHAR_LEN == 4
    typedef int wchar;
    #define UT(s) L ## s
    #define UNICODE 1
#elif ME_CHAR_LEN == 2
    typedef short wchar;
    #define UT(s) L ## s
    #define UNICODE 1
#else
    typedef char wchar;
    #define UT(s) s
#endif

#define ME_PLATFORM ME_OS "-" ME_CPU "-" ME_PROFILE

/********************************* O/S Includes *******************************/
/*
    Out-of-order definitions and includes. Order really matters in this section.
 */
#if WINDOWS
    #undef      _CRT_SECURE_NO_DEPRECATE
    #define     _CRT_SECURE_NO_DEPRECATE 1
    #undef      _CRT_SECURE_NO_WARNINGS
    #define     _CRT_SECURE_NO_WARNINGS 1
    #define     _WINSOCK_DEPRECATED_NO_WARNINGS 1
    #ifndef     _WIN32_WINNT
        /* Target Windows 7 by default */
        #define _WIN32_WINNT 0x601
    #endif
    /* 
        Work-around to allow the windows 7.* SDK to be used with VS 2012 
        MSC_VER 1800 2013
        MSC_VER 1900 2015
     */
    #if _MSC_VER >= 1700
        #define SAL_SUPP_H
        #define SPECSTRING_SUPP_H
    #endif
#endif

#if LINUX
    /*
        Use GNU extensions for:
            RTLD_DEFAULT for dlsym()
     */
    #define _GNU_SOURCE 1
    #if !ME_64
        #define _LARGEFILE64_SOURCE 1
        #ifdef __USE_FILE_OFFSET64
            #define _FILE_OFFSET_BITS 64
        #endif
    #endif
#endif

#if VXWORKS
    #ifndef _VSB_CONFIG_FILE
        #define _VSB_CONFIG_FILE "vsbConfig.h"
    #endif
    #include    <vxWorks.h>
#endif

#if ME_WIN_LIKE
    #include    <winsock2.h>
    #include    <windows.h>
    #include    <winbase.h>
    #include    <winuser.h>
    #include    <shlobj.h>
    #include    <shellapi.h>
    #include    <wincrypt.h>
    #include    <ws2tcpip.h>
    #include    <conio.h>
    #include    <process.h>
    #include    <windows.h>
    #include    <shlobj.h>
    #if ME_DEBUG
        #include <crtdbg.h>
    #endif
#endif

/*
    Includes in alphabetic order
 */
    #include    <ctype.h>
#if ME_WIN_LIKE
    #include    <direct.h>
#else
    #include    <dirent.h>
#endif
#if ME_UNIX_LIKE
    #include    <dlfcn.h>
#endif
    #include    <fcntl.h>
    #include    <errno.h>
#if ME_FLOAT
    #include    <float.h>
    #define __USE_ISOC99 1
    #include    <math.h>
#endif
#if ME_UNIX_LIKE
    #include    <grp.h> 
#endif
#if ME_WIN_LIKE
    #include    <io.h>
#endif
#if MACOSX || LINUX
    #include    <libgen.h>
#endif
    #include    <limits.h>
#if ME_UNIX_LIKE || VXWORKS
    #include    <sys/socket.h>
    #include    <arpa/inet.h>
    #include    <netdb.h>
    #include    <net/if.h>
    #include    <netinet/in.h>
    #include    <netinet/tcp.h>
    #include    <netinet/ip.h>
#endif
#if ME_UNIX_LIKE
    #include    <pthread.h> 
    #include    <pwd.h> 
#if !CYGWIN
    #include    <resolv.h>
#endif
#endif
#if ME_BSD_LIKE
    #include    <readpassphrase.h>
#endif
    #include    <setjmp.h>
    #include    <signal.h>
    #include    <stdarg.h>
#if ME_UNIX_LIKE
    #include    <stdint.h>
#endif
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <string.h>
#if ME_UNIX_LIKE
    #include    <syslog.h>
#endif
#if !TIDSP
    #include    <sys/stat.h>
    #include    <sys/types.h>
#endif
#if ME_UNIX_LIKE
    #include    <sys/ioctl.h>
    #include    <sys/mman.h>
    #include    <sys/poll.h>
    #include    <sys/resource.h>
    #include    <sys/select.h>
    #include    <sys/time.h>
    #include    <sys/times.h>
    #include    <sys/utsname.h>
    #include    <sys/uio.h>
    #include    <sys/wait.h>
    #include    <unistd.h>
#endif
    #include    <time.h>
#if !VXWORKS && !TIDSP
    #include    <wchar.h>
#endif

/*
    Extra includes per O/S
 */
#if CYGWIN
    #include    "w32api/windows.h"
    #include    "sys/cygwin.h"
#endif
#if LINUX
    #include <linux/version.h>
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
        #include    <sys/epoll.h>
    #endif
    #include    <sys/prctl.h>
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
        #include    <sys/eventfd.h>
    #endif
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,13)
        #define HAS_INOTIFY 1
        #include    <sys/inotify.h>
    #else
        #define HAS_INOTIFY 0
    #endif
    #if !__UCLIBC__
        #include    <sys/sendfile.h>
    #endif
#endif
#if MACOSX
    #include    <stdbool.h>
    #include    <mach-o/dyld.h>
    #include    <mach-o/dyld.h>
    #include    <mach/mach_init.h>
    #include    <mach/mach_time.h>
    #include    <mach/task.h>
    #include    <sys/sysctl.h>
    #include    <libkern/OSAtomic.h>
#endif
#if VXWORKS
    #include    <vxWorks.h>
    #include    <envLib.h>
    #include    <iosLib.h>
    #include    <loadLib.h>
    #include    <selectLib.h>
    #include    <sockLib.h>
    #include    <inetLib.h>
    #include    <ioLib.h>
    #include    <pipeDrv.h>
    #include    <hostLib.h>
    #include    <sysSymTbl.h>
    #include    <sys/fcntlcom.h>
    #include    <tickLib.h>
    #include    <taskHookLib.h>
    #include    <unldLib.h>
    #if _WRS_VXWORKS_MAJOR >= 6
        #include    <wait.h>
    #endif
    #if _WRS_VXWORKS_MAJOR > 6 || (_WRS_VXWORKS_MAJOR == 6 && _WRS_VXWORKS_MINOR >= 8)
        #include    <symSync.h>
        #include    <vxAtomicLib.h>
    #endif
#endif

#if TIDSP
    #include    <mathf.h>
    #include    <netmain.h>
    #include    <nettools/inc/dnsif.h>
    #include    <socket.h>
    #include    <file.h>
#endif

/************************************** Types *********************************/
/*
    Standard types
 */
#ifndef HAS_BOOL
    #ifndef __cplusplus
        #if !MACOSX
            #define HAS_BOOL 1
            /**
                Boolean data type.
             */
            #if _MSC_VER <= 1800
                typedef char bool;
            #endif
        #endif
    #endif
#endif

#ifndef HAS_UCHAR
    #define HAS_UCHAR 1
    /**
        Unsigned char data type.
     */
    typedef unsigned char uchar;
#endif

#ifndef HAS_SCHAR
    #define HAS_SCHAR 1
    /**
        Signed char data type.
     */
    typedef signed char schar;
#endif

#ifndef HAS_CCHAR
    #define HAS_CCHAR 1
    /**
        Constant char data type.
     */
    typedef const char cchar;
#endif

#ifndef HAS_CUCHAR
    #define HAS_CUCHAR 1
    /**
        Unsigned char data type.
     */
    typedef const unsigned char cuchar;
#endif

#ifndef HAS_USHORT
    #define HAS_USHORT 1
    /**
        Unsigned short data type.
     */
    typedef unsigned short ushort;
#endif

#ifndef HAS_CUSHORT
    #define HAS_CUSHORT 1
    /**
        Constant unsigned short data type.
     */
    typedef const unsigned short cushort;
#endif

#ifndef HAS_CVOID
    #define HAS_CVOID 1
    /**
        Constant void data type.
     */
    typedef const void cvoid;
#endif

#ifndef HAS_INT8
    #define HAS_INT8 1
    /**
        Integer 8 bits data type.
     */
    typedef char int8;
#endif

#ifndef HAS_UINT8
    #define HAS_UINT8 1
    /**
        Unsigned integer 8 bits data type.
     */
    typedef unsigned char uint8;
#endif

#ifndef HAS_INT16
    #define HAS_INT16 1
    /**
        Integer 16 bits data type.
     */
    typedef short int16;
#endif

#ifndef HAS_UINT16
    #define HAS_UINT16 1
    /**
        Unsigned integer 16 bits data type.
     */
    typedef unsigned short uint16;
#endif

#ifndef HAS_INT32
    #define HAS_INT32 1
    /**
        Integer 32 bits data type.
     */
    typedef int int32;
#endif

#ifndef HAS_UINT32
    #define HAS_UINT32 1
    /**
        Unsigned integer 32 bits data type.
     */
    typedef unsigned int uint32;
#endif

#ifndef HAS_UINT
    #define HAS_UINT 1
    /**
        Unsigned integer (machine dependent bit size) data type.
     */
    typedef unsigned int uint;
#endif

#ifndef HAS_ULONG
    #define HAS_ULONG 1
    /**
        Unsigned long (machine dependent bit size) data type.
     */
    typedef unsigned long ulong;
#endif

#ifndef HAS_CINT
    #define HAS_CINT 1
    /**
        Constant int data type.
     */
    typedef const int cint;
#endif

#ifndef HAS_SSIZE
    #define HAS_SSIZE 1
    #if ME_UNIX_LIKE || VXWORKS || DOXYGEN
        /**
            Signed integer size field large enough to hold a pointer offset.
         */
        typedef ssize_t ssize;
    #elif TIDSP
        typedef int ssize_t;
        typedef ssize_t ssize;
    #else
        typedef SSIZE_T ssize;
    #endif
#endif

/*
    Windows uses uint for write/read counts (Ugh!)
 */
#if ME_WIN_LIKE
    typedef uint wsize;
#else
    typedef ssize wsize;
#endif

#ifndef HAS_INT64
    #if ME_UNIX_LIKE
        __extension__ typedef long long int int64;
    #elif VXWORKS || DOXYGEN
        /**
            Integer 64 bit data type.
         */
        typedef long long int int64;
    #elif ME_WIN_LIKE
        typedef __int64 int64;
    #else
        typedef long long int int64;
    #endif
#endif

#ifndef HAS_UINT64
    #if ME_UNIX_LIKE
        __extension__ typedef unsigned long long int uint64;
    #elif VXWORKS || DOXYGEN
        typedef unsigned long long int uint64;
    #elif ME_WIN_LIKE
        typedef unsigned __int64 uint64;
    #else
        typedef unsigned long long int uint64;
    #endif
#endif

/**
    Signed file offset data type. Supports large files greater than 4GB in size on all systems.
 */
typedef int64 Offset;

#if DOXYGEN
    /** Size to hold the length of a socket address */
    typedef int Socklen;
#elif VXWORKS
    typedef int Socklen;
#else
    typedef socklen_t Socklen;
#endif

#if DOXYGEN || ME_UNIX_LIKE || VXWORKS
    /** Argument for sockets */
    typedef int Socket;
    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR -1
    #endif
    #define SOCKET_ERROR -1
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET -1
    #endif
#elif ME_WIN_LIKE
    typedef SOCKET Socket;
#elif TIDSP
    typedef SOCKET Socket;
    #define SOCKET_ERROR INVALID_SOCKET
#else
    typedef int Socket;
    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR -1
    #endif
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET -1
    #endif
#endif

typedef int64 Time;

/**
    Elapsed time data type. Stores time in milliseconds from some arbitrary start epoch.
 */
typedef int64 Ticks;

/**
    Time/Ticks units per second (milliseconds)
 */
#define TPS 1000

/*********************************** Defines **********************************/

#ifndef BITSPERBYTE
    #define BITSPERBYTE     ((int) (8 * sizeof(char)))
#endif

#ifndef BITS
    #define BITS(type)      ((int) (BITSPERBYTE * (int) sizeof(type)))
#endif

#if ME_FLOAT
    #ifndef MAXFLOAT
        #if ME_WIN_LIKE
            #define MAXFLOAT        DBL_MAX
        #else
            #define MAXFLOAT        FLT_MAX
        #endif
    #endif
    #if VXWORKS
        #undef isnan
        #define isnan(n)  ((n) != (n))
        #define isnanf(n) ((n) != (n))
        #define isinf(n)  ((n) == (1.0 / 0.0) || (n) == (-1.0 / 0.0))
        #define isinff(n) ((n) == (1.0 / 0.0) || (n) == (-1.0 / 0.0))
    #endif
    #if ME_WIN_LIKE
        #define isNan(f) (_isnan(f))
    #elif VXWORKS || MACOSX || LINUX
        #define isNan(f) (isnan(f))
    #else
        #define isNan(f) (fpclassify(f) == FP_NAN)
    #endif
#endif

#if ME_WIN_LIKE
    #define INT64(x)    (x##i64)
    #define UINT64(x)   (x##Ui64)
#else
    #define INT64(x)    (x##LL)
    #define UINT64(x)   (x##ULL)
#endif

#ifndef MAXINT
#if INT_MAX
    #define MAXINT      INT_MAX
#else
    #define MAXINT      0x7fffffff
#endif
#endif

#ifndef MAXUINT
#if UINT_MAX
    #define MAXUINT     UINT_MAX
#else
    #define MAXUINT     0xffffffff
#endif
#endif

#ifndef MAXINT64
    #define MAXINT64    INT64(0x7fffffffffffffff)
#endif
#ifndef MAXUINT64
    #define MAXUINT64   INT64(0xffffffffffffffff)
#endif

#if SIZE_T_MAX
    #define MAXSIZE     SIZE_T_MAX
#elif ME_64
    #define MAXSIZE     INT64(0xffffffffffffffff)
#else
    #define MAXSIZE     MAXINT
#endif

#if SSIZE_T_MAX
    #define MAXSSIZE     SSIZE_T_MAX
#elif ME_64
    #define MAXSSIZE     INT64(0x7fffffffffffffff)
#else
    #define MAXSSIZE     MAXINT
#endif

#if OFF_T_MAX
    #define MAXOFF       OFF_T_MAX
#else
    #define MAXOFF       INT64(0x7fffffffffffffff)
#endif

/*
    Word size and conversions between integer and pointer.
 */
#if ME_64
    #define ITOP(i)     ((void*) ((int64) i))
    #define PTOI(i)     ((int) ((int64) i))
    #define LTOP(i)     ((void*) ((int64) i))
    #define PTOL(i)     ((int64) i)
#else
    #define ITOP(i)     ((void*) ((int) i))
    #define PTOI(i)     ((int) i)
    #define LTOP(i)     ((void*) ((int) i))
    #define PTOL(i)     ((int64) (int) i)
#endif

#undef PUBLIC
#undef PUBLIC_DATA
#undef PRIVATE

#if ME_WIN_LIKE
    /*
        Use PUBLIC on function declarations and definitions (*.c and *.h). 
     */
    #define PUBLIC      __declspec(dllexport)
    #define PUBLIC_DATA __declspec(dllexport)
    #define PRIVATE     static
#else
    #define PUBLIC
    #define PUBLIC_DATA extern
    #define PRIVATE     static
#endif

#ifndef max
    #define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
    #define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef PRINTF_ATTRIBUTE
    #if ((__GNUC__ >= 3) && !DOXYGEN) || MACOSX
        /** 
            Use gcc attribute to check printf fns.  a1 is the 1-based index of the parameter containing the format, 
            and a2 the index of the first argument. Note that some gcc 2.x versions don't handle this properly 
         */
        #define PRINTF_ATTRIBUTE(a1, a2) __attribute__ ((format (__printf__, a1, a2)))
    #else
        #define PRINTF_ATTRIBUTE(a1, a2)
    #endif
#endif

/*
    Optimize expression evaluation code depending if the value is likely or not
 */
#undef likely
#undef unlikely
#if (__GNUC__ >= 3)
    #define likely(x)   __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
#endif

#if !__UCLIBC__ && !CYGWIN && __USE_XOPEN2K
    #define ME_COMPILER_HAS_SPINLOCK    1
#endif

#if ME_COMPILER_HAS_DOUBLE_BRACES
    #define  NULL_INIT    {{0}}
#else
    #define  NULL_INIT    {0}
#endif

#ifdef __USE_FILE_OFFSET64
    #define ME_COMPILER_HAS_OFF64 1
#else
    #define ME_COMPILER_HAS_OFF64 0
#endif

#if ME_UNIX_LIKE
    #define ME_COMPILER_HAS_FCNTL 1
#endif

#ifndef R_OK
    #define R_OK    4
    #define W_OK    2
#if ME_WIN_LIKE
    #define X_OK    R_OK
#else
    #define X_OK    1
#endif
    #define F_OK    0
#endif

#if MACOSX
    #define LD_LIBRARY_PATH "DYLD_LIBRARY_PATH"
#else
    #define LD_LIBRARY_PATH "LD_LIBRARY_PATH"
#endif

#if VXWORKS
/*
    Old VxWorks can't do array[]
 */
    #define ARRAY_FLEX 0
#else
    #define ARRAY_FLEX
#endif

/*
    Deprecated API warnings
 */
#if ((__GNUC__ >= 3) || MACOSX) && !VXWORKS
    #define ME_DEPRECATED(MSG) __attribute__ ((deprecated(MSG)))
#else
    #define ME_DEPRECATED(MSG) 
#endif

/********************************** Tunables *********************************/
/*
    These can be defined in main.bit settings (pascal case) to override. E.g.

    settings: {
        maxPath: 4096
    }
 */
#ifndef ME_MAX_FNAME
    #define ME_MAX_FNAME        256         /**< Reasonable filename size */
#endif
#ifndef ME_MAX_PATH
    #define ME_MAX_PATH         1024        /**< Reasonable filename size */
#endif
#if DEPRECATE || 1
/* This is not a maximum, but a default size */
#ifndef ME_MAX_BUFFER
    #define ME_MAX_BUFFER       4096        /**< Reasonable size for buffers */
#endif
#endif
#ifndef ME_BUFSIZE
    #define ME_BUFSIZE          4096        /**< Reasonable size for buffers */
#endif
#ifndef ME_MAX_ARGC
    #define ME_MAX_ARGC         32          /**< Maximum number of command line args if using MAIN()*/
#endif
#ifndef ME_DOUBLE_BUFFER
    #define ME_DOUBLE_BUFFER    (DBL_MANT_DIG - DBL_MIN_EXP + 4)
#endif
#ifndef ME_MAX_IP
    #define ME_MAX_IP           1024
#endif


#ifndef ME_STACK_SIZE
#if ME_COMPILER_HAS_MMU && !VXWORKS
    /*
        If the system supports virtual memory, then stack size should use system default. Only used pages will
        actually consume memory
    */
    #define ME_STACK_SIZE    0               /**< Default thread stack size (0 means use system default) */
#else
    /*
        No MMU, so the stack size actually consumes memory. Set this as low as possible.
        NOTE: php and ejs use stack heavily.
    */
    #define ME_STACK_SIZE    (128 * 1024)    /**< Default thread stack size (0 means use system default) */
#endif
#endif

/*********************************** Fixups ***********************************/

#ifndef ME_INLINE
    #if ME_WIN_LIKE
        #define ME_INLINE __inline
    #else
        #define ME_INLINE inline
    #endif
#endif

#if ECOS
    #define     LIBKERN_INLINE          /* to avoid kernel inline functions */
#endif /* ECOS */

#if ME_UNIX_LIKE || VXWORKS || TIDSP
    #define FILE_TEXT        ""
    #define FILE_BINARY      ""
#endif

#if !TIDSP
    #define ME_COMPILER_HAS_MACRO_VARARGS 1
#else
    #define ME_COMPILER_HAS_MACRO_VARARGS 1
#endif

#if ME_UNIX_LIKE
    #define closesocket(x)  close(x)
    #ifndef PTHREAD_MUTEX_RECURSIVE_NP
        #define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
    #endif
    #ifndef PTHREAD_MUTEX_RECURSIVE
        #define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
    #endif
#endif

#if !ME_WIN_LIKE && !CYGWIN
    #ifndef O_BINARY
        #define O_BINARY    0
    #endif
    #ifndef O_TEXT
        #define O_TEXT      0
    #endif
#endif

#if !LINUX
    #define __WALL          0
    #if !CYGWIN && !defined(MSG_NOSIGNAL)
        #define MSG_NOSIGNAL 0
    #endif
#endif

#if MACOSX
    /*
        Fix for MAC OS X - getenv
     */
    #if !HAVE_DECL_ENVIRON
        #ifdef __APPLE__
            #include <crt_externs.h>
            #define environ (*_NSGetEnviron())
        #else
            extern char **environ;
        #endif
    #endif
#endif

#if SOLARIS
    #define INADDR_NONE     ((in_addr_t) 0xffffffff)
#endif

#if VXWORKS
    #ifndef SHUT_RDWR
        #define SHUT_RDWR 2
    #endif
    #define HAVE_SOCKLEN_T
    #if _DIAB_TOOL
        #define inline __inline__
        #define MPR_INLINE __inline__
    #endif
    #ifndef closesocket
        #define closesocket(x)  close(x)
    #endif
    #ifndef va_copy
        #define va_copy(d, s) ((d) = (s))
    #endif
    #ifndef strcasecmp
        #define strcasecmp scaselesscmp
    #endif
    #ifndef strncasecmp
        #define strncasecmp sncaselesscmp
    #endif
#endif

#if ME_WIN_LIKE
    typedef int     uid_t;
    typedef void    *handle;
    typedef char    *caddr_t;
    typedef long    pid_t;
    typedef int     gid_t;
    typedef ushort  mode_t;
    typedef void    *siginfo_t;
    typedef int     socklen_t;

    #define HAVE_SOCKLEN_T
    #define MSG_NOSIGNAL    0
    #define FILE_BINARY     "b"
    #define FILE_TEXT       "t"

    /*
        Error codes 
     */
    #define EPERM           1
    #define ENOENT          2
    #define ESRCH           3
    #define EINTR           4
    #define EIO             5
    #define ENXIO           6
    #define E2BIG           7
    #define ENOEXEC         8
    #define EBADF           9
    #define ECHILD          10
    #define EAGAIN          11
    #define ENOMEM          12
    #define EACCES          13
    #define EFAULT          14
    #define EOSERR          15
    #define EBUSY           16
    #define EEXIST          17
    #define EXDEV           18
    #define ENODEV          19
    #define ENOTDIR         20
    #define EISDIR          21
    #define EINVAL          22
    #define ENFILE          23
    #define EMFILE          24
    #define ENOTTY          25
    #define EFBIG           27
    #define ENOSPC          28
    #define ESPIPE          29
    #define EROFS           30
    #define EMLINK          31
    #define EPIPE           32
    #define EDOM            33
    #define ERANGE          34

    #ifndef EWOULDBLOCK
    #define EWOULDBLOCK     EAGAIN
    #define EINPROGRESS     36
    #define EALREADY        37
    #define ENETDOWN        43
    #define ECONNRESET      44
    #define ECONNREFUSED    45
    #define EADDRNOTAVAIL   49
    #define EISCONN         56
    #define EADDRINUSE      46
    #define ENETUNREACH     51
    #define ECONNABORTED    53
    #endif
    #ifndef ENOTCONN
        #define ENOTCONN    126
    #endif
    #ifndef EPROTO
        #define EPROTO      134
    #endif

    #undef SHUT_RDWR
    #define SHUT_RDWR       2

    #define TIME_GENESIS UINT64(11644473600000000)
    #ifndef va_copy
        #define va_copy(d, s) ((d) = (s))
    #endif

    #if !WINCE
    #define access      _access
    #define chdir       _chdir
    #define chmod       _chmod
    #define close       _close
    #define fileno      _fileno
    #define fstat       _fstat
    #define getcwd      _getcwd
    #define getpid      _getpid
    #define gettimezone _gettimezone
    #define lseek       _lseek
    #define mkdir(a,b)  _mkdir(a)
    #define open        _open
    #define putenv      _putenv
    #define read        _read
    #define rmdir(a)    _rmdir(a)
    #define stat        _stat
    #define strdup      _strdup
    #define tempnam     _tempnam
    #define umask       _umask
    #define unlink      _unlink
    #define write       _write
    PUBLIC void         sleep(int secs);
    #endif
    #define strcasecmp scaselesscmp
    #define strncasecmp sncaselesscmp
    #pragma comment( lib, "ws2_32.lib" )
#endif /* WIN_LIKE */

#if WINCE
    typedef void FILE;
    typedef int off_t;

    #ifndef EOF
        #define EOF        -1
    #endif
    #define O_RDONLY        0
    #define O_WRONLY        1
    #define O_RDWR          2
    #define O_NDELAY        0x4
    #define O_NONBLOCK      0x4
    #define O_APPEND        0x8
    #define O_CREAT         0x100
    #define O_TRUNC         0x200
    #define O_TEXT          0x400
    #define O_EXCL          0x800
    #define O_BINARY        0x1000
    /*
        stat flags
     */
    #define S_IFMT          0170000 
    #define S_IFDIR         0040000
    #define S_IFCHR         0020000         /* character special */
    #define S_IFIFO         0010000
    #define S_IFREG         0100000
    #define S_IREAD         0000400
    #define S_IWRITE        0000200
    #define S_IEXEC         0000100

    #ifndef S_ISDIR
        #define S_ISDIR(X) (((X) & S_IFMT) == S_IFDIR)
    #endif
    #ifndef S_ISREG
        #define S_ISREG(X) (((X) & S_IFMT) == S_IFREG)
    #endif

    #define STARTF_USESHOWWINDOW 0
    #define STARTF_USESTDHANDLES 0

    #define BUFSIZ   ME_MAX_BUFFER
    #define PATHSIZE ME_MAX_PATH
    #define gethostbyname2(a,b) gethostbyname(a)
    #pragma comment( lib, "ws2.lib" )
#endif /* WINCE */

#if TIDSP
    #define EINTR   4
    #define EAGAIN  11
    #define INADDR_NONE 0xFFFFFFFF
    #define PATHSIZE ME_MAX_PATH
    #define NBBY 8
    #define hostent _hostent
    #define NFDBITS ((int) (sizeof(fd_mask) * NBBY))
    typedef long fd_mask;
    typedef int Socklen;
    struct sockaddr_storage { char pad[1024]; };
#endif /* TIDSP */

/*********************************** Externs **********************************/

#ifdef __cplusplus
extern "C" {
#endif

#if LINUX
    extern int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict __attr, int *__restrict __kind) __THROW;
    extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind) __THROW;
    extern char **environ;
#endif

#if VXWORKS
    #if _WRS_VXWORKS_MAJOR < 6 || (_WRS_VXWORKS_MAJOR == 6 && _WRS_VXWORKS_MINOR < 9)
        PUBLIC int gettimeofday(struct timeval *tv, struct timezone *tz);
    #endif
    PUBLIC char *strdup(const char *);
    PUBLIC int sysClkRateGet();

    #if _WRS_VXWORKS_MAJOR < 6
        #define NI_MAXHOST      128
        extern STATUS access(cchar *path, int mode);
        typedef int     socklen_t;
        struct sockaddr_storage {
            char        pad[1024];
        };
    #else
        /*
            This may or may not be necessary - let us know dev@embedthis.com if your system needs this (and why).
         */
        #if _DIAB_TOOL
            #if ME_CPU_ARCH == ME_CPU_PPC
                #define __va_copy(dest, src) memcpy((dest), (src), sizeof(va_list))
            #endif
        #endif
        #define HAVE_SOCKLEN_T
    #endif
#endif  /* VXWORKS */

#if ME_WIN_LIKE
    struct timezone {
      int  tz_minuteswest;      /* minutes W of Greenwich */
      int  tz_dsttime;          /* type of dst correction */
    };
    PUBLIC int  getuid(void);
    PUBLIC int  geteuid(void);
    PUBLIC int  gettimeofday(struct timeval *tv, struct timezone *tz);
    PUBLIC long lrand48(void);
    PUBLIC long nap(long);
    PUBLIC void srand48(long);
    PUBLIC long ulimit(int, ...);
#endif

#if WINCE
    struct stat {
        int     st_dev;
        int     st_ino;
        ushort  st_mode;
        short   st_nlink;
        short   st_uid;
        short   st_gid;
        int     st_rdev;
        long    st_size;
        time_t  st_atime;
        time_t  st_mtime;
        time_t  st_ctime;
    };
    extern int access(cchar *filename, int flags);
    extern int chdir(cchar   dirname);
    extern int chmod(cchar *path, int mode);
    extern int close(int handle);
    extern void exit(int status);
    extern long _get_osfhandle(int handle);
    extern char *getcwd(char* buffer, int maxlen);
    extern char *getenv(cchar *charstuff);
    extern pid_t getpid();
    extern long lseek(int handle, long offset, int origin);
    extern int mkdir(cchar *dir, int mode);
    extern time_t mktime(struct tm *pt);
    extern int _open_osfhandle(int *handle, int flags);
    extern uint open(cchar *file, int mode,...);
    extern int read(int handle, void *buffer, uint count);
    extern int rename(cchar *from, cchar *to);
    extern int rmdir(cchar   dir);
    extern uint sleep(uint secs);
    extern int stat(cchar *path, struct stat *stat);
    extern char *strdup(char *s);
    extern int write(int handle, cvoid *buffer, uint count);
    extern int umask(int mode);
    extern int unlink(cchar *path);
    extern int errno;

    #undef CreateFile
    #define CreateFile CreateFileA
    WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile);

    #undef CreateProcess
    #define CreateProcess CreateProcessA

    #undef FindFirstFile
    #define FindFirstFile FindFirstFileA
    WINBASEAPI HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

    #undef FindNextFile
    #define FindNextFile FindNextFileA
    WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

    #undef GetModuleFileName
    #define GetModuleFileName GetModuleFileNameA
    WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

    #undef GetModuleHandle
    #define GetModuleHandle GetModuleHandleA
    WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);

    #undef GetProcAddress
    #define GetProcAddress GetProcAddressA

    #undef GetFileAttributes
    #define GetFileAttributes GetFileAttributesA
    extern DWORD GetFileAttributesA(cchar *path);

    extern void GetSystemTimeAsFileTime(FILETIME *ft);

    #undef LoadLibrary
    #define LoadLibrary LoadLibraryA
    HINSTANCE WINAPI LoadLibraryA(LPCSTR lpLibFileName);

    #define WSAGetLastError GetLastError

    #define _get_timezone getTimezone
    extern int getTimezone(int *secs);

    extern struct tm *localtime_r(const time_t *when, struct tm *tp);
    extern struct tm *gmtime_r(const time_t *t, struct tm *tp);
#endif /* WINCE */

#ifdef __cplusplus
}
#endif

#endif /* _h_OSDEP */

/*
    Copyright (c) Michael O'Brien. All Rights Reserved.
    This is proprietary software and requires a commercial license from the author.
 */
