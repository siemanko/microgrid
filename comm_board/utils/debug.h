#ifndef UTILS_DEBUG_H
#define UTILS_DEBUG_H

#include <stdarg.h>

#define DEBUG_MISC  0
#define DEBUG_ERROR 1

void debug(char subsystem, char* format, ...);
void debug_args(char subsystem, char* format, va_list args);

void debug_unsafe(char subsystem, char* format, ...);
void debug_unsafe_args(char subsystem, char* format, va_list args);

#endif