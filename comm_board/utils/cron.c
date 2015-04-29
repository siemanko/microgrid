#include "cron.h"

#include <stdlib.h>

#include "drivers/timer.h"
#include "shared/algorithm/vector.h"
#include "shared/utils.h"
#include "utils/debug.h"
#include "communication/messages.h"

#define CRON_AVERAGE(x,y) (4LL*(x)/5 + (y))

// Vector of functions executed every iteration.
static Vector repeat_rapid;
// Vector for functions executed every N seconds;
static Vector repeat_s;
// Vector for functions executed every N milliseconds;
static Vector repeat_ms;

typedef struct {
    void (*callback)();
#ifdef CRON_STATS
    char* name;
    uint32_t avg_execution_us;
#endif    
} RepeatRapid;

typedef struct {
    void (*callback)();
    uint32_t last_repeat;
    uint32_t time_between_repeats;
#ifdef CRON_STATS
    char* name;
    uint32_t avg_execution_us;
#endif
} RepeatSeconds;

typedef struct {
    void (*callback)();
    uint64_t last_repeat;
    uint32_t time_between_repeats;
#ifdef CRON_STATS
    char* name;
    uint32_t avg_execution_us;
#endif
} RepeatMilliseconds;

void init_cron() {
    // Initial capacity 0, conserve memory.
    make_vector(&repeat_rapid, 0, 1);
    make_vector(&repeat_s, 0, 1);
    make_vector(&repeat_ms, 0, 1);
    
    set_message_handler(UMSG_CRON_STATS, cron_print_stats);
}


void cron_repeat_rapidly_internal(void (*callback)(), char* name) {
    RepeatRapid* repeat = malloc(sizeof(RepeatRapid));
    repeat->callback = callback;
    #ifdef CRON_STATS
        repeat->name = name;
        repeat->avg_execution_us = 0;
    #endif
    vector_push_back(&repeat_rapid, repeat);
}

// This one is recommended over repeat_every_ms, as it consumes
// less memory.
void cron_repeat_every_s_internal(uint32_t num_s, void (*callback)(), char* name) {
    RepeatSeconds* repeat = malloc(sizeof(RepeatSeconds));
    repeat->callback = callback;
    repeat->last_repeat = 0;
    repeat->time_between_repeats = num_s;
    #ifdef CRON_STATS
        repeat->name = name;
        repeat->avg_execution_us = 0;
    #endif
    vector_push_back(&repeat_s, repeat);
}

void cron_repeat_every_ms_internal(uint32_t num_ms, void(*callback)(), char* name) {
    RepeatMilliseconds* repeat = malloc(sizeof(RepeatMilliseconds));
    repeat->callback = callback;
    repeat->last_repeat = 0;
    repeat->time_between_repeats = num_ms;
    #ifdef CRON_STATS
        repeat->name = name;
        repeat->avg_execution_us = 0;
    #endif
    vector_push_back(&repeat_ms, repeat);
}

void cron_reset_timers() {
    int vidx;
    for (vidx = 0; vidx < repeat_s.size; ++vidx) {
        RepeatSeconds* rs = (RepeatSeconds*)repeat_s.buffer[vidx];
        rs->last_repeat = 0;
    }
    for (vidx = 0; vidx < repeat_ms.size; ++vidx) {
        RepeatMilliseconds* rms = (RepeatMilliseconds*)repeat_ms.buffer[vidx];
        rms->last_repeat = 0;
    }

}

uint32_t steps_this_second = 0;
uint32_t average_steps_per_second =0;
uint32_t last_step_measurement;

void cron_step() {
    ++steps_this_second;
    if (last_step_measurement != time_seconds_since_epoch()) {
        average_steps_per_second = 
                CRON_AVERAGE(average_steps_per_second, steps_this_second);
        steps_this_second = 0;
        last_step_measurement = time_seconds_since_epoch();
    }
    
    int vidx;
    for (vidx = 0; vidx < repeat_rapid.size; ++vidx) {
        RepeatRapid* rr = (RepeatRapid*)repeat_rapid.buffer[vidx];
        #ifdef CRON_STATS
            stopwatch_start();
        #endif
        rr->callback();
        #ifdef CRON_STATS
            uint32_t cur_exec = stopwatch_stop_us();
            rr->avg_execution_us = CRON_AVERAGE(rr->avg_execution_us,cur_exec); 
        #endif
    }

    for (vidx = 0; vidx < repeat_s.size; ++vidx) {
        RepeatSeconds* rs = (RepeatSeconds*)repeat_s.buffer[vidx];
        if (rs->last_repeat + rs->time_between_repeats <= time_seconds_since_epoch()) {
            #ifdef CRON_STATS
                stopwatch_start();
            #endif
            rs->callback();
            rs->last_repeat = time_seconds_since_epoch();
            #ifdef CRON_STATS
                uint32_t cur_exec = stopwatch_stop_us();
                rs->avg_execution_us = CRON_AVERAGE(rs->avg_execution_us,cur_exec);
            #endif
        }
    }

    for (vidx = 0; vidx < repeat_ms.size; ++vidx) {
        RepeatMilliseconds* rms = 
                (RepeatMilliseconds*)repeat_ms.buffer[vidx];
        if (rms->last_repeat + rms->time_between_repeats <= 
                time_milliseconds_since_epoch()) {
            #ifdef CRON_STATS
                stopwatch_start();
            #endif
            rms->callback();
            #ifdef CRON_STATS
               uint32_t cur_exec = stopwatch_stop_us();
               rms->avg_execution_us = CRON_AVERAGE(rms->avg_execution_us,cur_exec);
            #endif
            rms->last_repeat = time_milliseconds_since_epoch();
        }
    }

}

void cron_print_stats() {
    debug(DEBUG_CRON, " average of %l steps per second.",
        average_steps_per_second);
    #ifdef CRON_STATS
        int vidx;
        for (vidx = 0; vidx < repeat_rapid.size; ++vidx) {
            RepeatRapid* rr = (RepeatRapid*)repeat_rapid.buffer[vidx];
            debug(DEBUG_CRON, "%s avg exec time = %l us", rr->name, rr->avg_execution_us);
        }
        for (vidx = 0; vidx < repeat_s.size; ++vidx) {
            RepeatSeconds* rs = (RepeatSeconds*)repeat_s.buffer[vidx];
            debug(DEBUG_CRON, "%s avg exec time = %l us", rs->name, rs->avg_execution_us);
        }
        for (vidx = 0; vidx < repeat_ms.size; ++vidx) {
            RepeatMilliseconds* rms = (RepeatMilliseconds*)repeat_ms.buffer[vidx];
            debug(DEBUG_CRON, "%s avg exec time = %l us", rms->name, rms->avg_execution_us);
        }
    #else
        debug(DEBUG_ERROR, "To get detailed statistics define CRON_STATS.");
    #endif
}
