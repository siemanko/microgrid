#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include <stdarg.h>

#define DEBUG_INFO  0
#define DEBUG_ERROR 1
#define DEBUG_CRON  2

void debug(char subsystem, const char* format, ...);
void debug_args(char subsystem, const char* format, va_list args);

void debug_unsafe(char subsystem, const char* format, ...);
void debug_unsafe_args(char subsystem, const char* format, va_list args);

#endif