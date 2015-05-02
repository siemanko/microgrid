#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <stdint.h>

typedef enum {
    DATA_LOGGER_TYPE_FLOAT = 1,
    DATA_LOGGER_TYPE_UINT32 = 2,
} DataLoggerType;

typedef struct {
    const char* name;
    uint8_t num_column;
    const DataLoggerType* column_type;
    const char** column_name;
} DataLoggerSchema;

void init_data_logger();

void data_logger_load_schema(DataLoggerSchema*);

void data_logger_log(int ignored, ...);

void data_logger_reset();

#endif