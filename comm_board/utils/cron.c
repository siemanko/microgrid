#include <stdlib.h>

#include "cron.h"

#include "drivers/timer.h"
#include "shared/algorithm/vector.h"
#include "shared/utils.h"
#include "utils/debug.h"
#include "communication/messages.h"

// Vector of functions executed every iteration.
static Vector repeat_rapid;
// Vector for functions executed every N seconds;
static Vector repeat_s;
// Vector for functions executed every N milliseconds;
static Vector repeat_ms;

typedef struct {
    void (*callback)();
    uint32_t last_repeat;
    uint32_t time_between_repeats;
} RepeatSeconds;

typedef struct {
    void (*callback)();
    uint64_t last_repeat;
    uint32_t time_between_repeats;
} RepeatMilliseconds;

void init_cron() {
    // Initial capacity 0, conserve memory.
    make_vector(&repeat_rapid, 0, 1);
    make_vector(&repeat_s, 0, 1);
    make_vector(&repeat_ms, 0, 1);
    
    set_message_handler(UMSG_CRON_STATS, cron_print_stats);
}

void cron_repeat_rapidly(void (*callback)()) {
    vector_push_back(&repeat_rapid, callback);
}

void cron_repeat_every_s(uint32_t num_s, void (*callback)()) {
    RepeatSeconds* repeat = malloc(sizeof(RepeatSeconds));
    repeat->callback = callback;
    repeat->last_repeat = 0;
    repeat->time_between_repeats = num_s;
    
    vector_push_back(&repeat_s, repeat);
    
}

void cron_repeat_every_ms(uint32_t num_ms, void(*callback)()) {
    RepeatMilliseconds* repeat = malloc(sizeof(RepeatMilliseconds));
    repeat->callback = callback;
    repeat->last_repeat = 0;
    repeat->time_between_repeats = num_ms;
    
    vector_push_back(&repeat_ms, repeat);
}

void cron_reset_timers() {
    int vidx;
    for (vidx = 0; vidx < repeat_s.size; ++vidx) {
        RepeatSeconds* rs = (RepeatSeconds*)repeat_s.buffer[vidx];
        rs->last_repeat = 0;
    }
    for (vidx = 0; vidx < repeat_ms.size; ++vidx) {
        RepeatMilliseconds* rms = (RepeatMilliseconds*)repeat_s.buffer[vidx];
        rms->last_repeat = 0;
    }
    
}

void cron_step() {
    int vidx;
    for (vidx = 0; vidx < repeat_rapid.size; ++vidx) {
        void (*callback)();
        callback = (void (*)())repeat_rapid.buffer[vidx];
        callback();
    }

    for (vidx = 0; vidx < repeat_s.size; ++vidx) {
        RepeatSeconds* rs = (RepeatSeconds*)repeat_s.buffer[vidx];
        if (rs->last_repeat + rs->time_between_repeats <= time_seconds_since_epoch()) {
            rs->callback();
            rs->last_repeat = time_seconds_since_epoch();
        }
    }

    for (vidx = 0; vidx < repeat_ms.size; ++vidx) {
        RepeatMilliseconds* rms = 
                (RepeatMilliseconds*)repeat_ms.buffer[vidx];
        if (rms->last_repeat + rms->time_between_repeats <= 
                time_milliseconds_since_epoch()) {
            rms->callback();
            rms->last_repeat = time_milliseconds_since_epoch();
        }
    }

}

void cron_print_stats() {
    debug(DEBUG_ERROR, "You must define CRON_STATS to use this feature.");
}