#ifndef NATIVE_HOST_H
#define NATIVE_HOST_h
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef NULL
#define NULL ((void *) 0)
#endif

#if defined(_WIN32)
#define WINDOWS
#include <Windows.h>
#elif defined(__unix__)
#define UNIX
#include <sys/mman.h>
#endif

#endif
