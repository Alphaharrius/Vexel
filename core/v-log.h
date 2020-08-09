#ifndef V_LOG_H
#define V_LOG_H
#pragma once
/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 * 
 * v-log.h
 * A small utility header file that wraps 
 * the printf function with coloring options.
 */
#define COLOR_DEFAULT       "[0m"
#define COLOR_RED           "[0;31m"
#define COLOR_BOLD_RED      "[1;31m"
#define COLOR_GREEN         "[0;32m"
#define COLOR_BOLD_GREEN    "[1;32m"
#define COLOR_YELLOW        "[0;33m"
#define COLOR_BOLD_YELLOW   "[1;33m"
#define COLOR_BLUE          "[0;34m"
#define COLOR_BOLD_BLUE     "[1;34m"
#define COLOR_MAGENTA       "[0;35m"
#define COLOR_BOLD_MAGENTA  "[1;35m"
#define COLOR_CYAN          "[0;36m"
#define COLOR_BOLD_CYAN     "[1;36m"

#define LOG(color, head, text) \
  printf("\033%s[%s]\t%s\033%s\n", color, head, text, COLOR_DEFAULT)

#endif
