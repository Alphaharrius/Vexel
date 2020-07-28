#ifndef SYS_INFO_H
#define SYS_INFO_H

#ifdef _WIN32
#define OS_WINDOWS
#elif __unix__
#define OS_UNIX
#endif

#endif
