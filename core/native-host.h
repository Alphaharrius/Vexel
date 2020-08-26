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
#include <Windows.h>
#elif defined(__linux__)
#include <sys/mman.h>
#endif

#endif
