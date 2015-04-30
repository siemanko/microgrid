#include "misc.h"

#include <stdarg.h>
#include <stdlib.h>

#include "drivers/timer.h"
#include "constants.h"
#include "utils/debug.h"
#include "shared/utils.h"

// This chunk of memory is used to reserve some space
// on the heap for handling assert failure -
// one of the cause of assert failure can be 
// out of memory error
static void *reserved; 

void display_assert_message(char* format, va_list args) {
    if (ASSERTS_ENABLED) {
        while(1) {
            free(reserved);
            // TODO(szymon): also display on LCD?
            debug_unsafe_args(DEBUG_ERROR, format, args);
            delay_ms(1000);
        }
    }
}

void init_assert() {
    if (ASSERTS_ENABLED) {
        set_assert_callback(display_assert_message);
        reserved = safe_malloc(350);
    }
}