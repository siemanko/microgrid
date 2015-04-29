#include "misc.h"

#include <stdarg.h>

#include "drivers/timer.h"
#include "constants.h"
#include "utils/debug.h"
#include "shared/utils.h"

void display_assert_message(char* format, va_list args) {
    if (asserts_enabled) {
        while(1) {
            // TODO(szymon): also display on LCD?
            debug_unsafe_args(DEBUG_ERROR, format, args);
            delay_ms(1000);
        }
    }
}

void init_assert() {
    set_assert_callback(display_assert_message);
}