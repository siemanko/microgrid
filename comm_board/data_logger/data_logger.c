#include "data_logger.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "drivers/eeprom.h"
#include "shared/utils.h"
#include "storage.h"
#include "utils/debug.h"

static DataLoggerSchema* current_schema;
static uint32_t          current_entry_size;

typedef enum {
    DLS_SCHEMA_HASH = 0,
    DLS_NUM_LOGGED = 4,
    DLS_DATA = 8,
} DataLoggerStorage;


static uint16_t dls(uint16_t offset) {
    return STORAGE_DATA_LOGGER + offset;
}

void init_data_logger() {
    // TODO(szymon): init message_handler etc.
}

// computes hash that allows to compare different schemas.
static uint32_t schema_hash(DataLoggerSchema* schema) {
    uint32_t res = 0;
    res = checksum(res, (uint8_t*)schema->name, strlen(schema->name));
    res = checksum(res, &(schema->num_column), 1);
    int idx;
    for (idx = 0; idx < schema->num_column; ++idx) {
        uint8_t column_type = (uint8_t)schema->column_type[idx];
        res = checksum(res, &(column_type), 1);
        res = checksum(res, 
                       (uint8_t*)schema->column_name[idx],
                       strlen(schema->column_name[idx]));
    }
    return res;
}

// computes size of single log entry given schema.
static uint32_t entry_size() {
    uint32_t res = 0;
    int idx;
    for (idx = 0; idx < current_schema->num_column; ++idx) {
        DataLoggerType column_type = current_schema->column_type[idx];
        if (column_type == DATA_LOGGER_TYPE_FLOAT) {
            res += 4;
        } else if (column_type == DATA_LOGGER_TYPE_UINT32) {
            res += 4;
        }
    }
    return res;
}

void data_logger_load_schema(DataLoggerSchema* schema) {
    current_schema = schema;
    uint32_t current_hash = eeprom_read_uint32(dls(DLS_SCHEMA_HASH));
    uint32_t new_hash = schema_hash(schema);
    if (current_hash != new_hash) {
        debug(DEBUG_INFO, "Schema changed reseting logs");
        data_logger_reset();
    }
    current_entry_size = entry_size(schema);
}

void data_logger_log(int ignored, ...) {
    uint32_t offset = dls(DLS_DATA);
    uint32_t num_data = eeprom_read_uint32(dls(DLS_NUM_LOGGED));
    offset += num_data * current_entry_size;

    va_list args;
    va_start(args, ignored);
    int idx;
    for (idx = 0; idx < current_schema->num_column; ++idx) {
        DataLoggerType column_type = current_schema->column_type[idx];
        if (column_type == DATA_LOGGER_TYPE_FLOAT) {
            float f = va_arg(args, float);
            eeprom_write_float(offset, f);
            offset += 4;
        } else if (column_type == DATA_LOGGER_TYPE_UINT32) {
            uint32_t l = va_arg(args, uint32_t);
            eeprom_write_uint32(offset, l);
            offset += 4;
        }
    }
    eeprom_write_uint32(dls(DLS_NUM_LOGGED), num_data+1);
    va_end(args);
}

void data_logger_reset() {
    eeprom_write_uint32(dls(DLS_SCHEMA_HASH), schema_hash(current_schema));
    eeprom_write_uint32(dls(DLS_NUM_LOGGED), 0);
}