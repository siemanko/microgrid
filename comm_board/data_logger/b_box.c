#include "b_box.h"

#include "data_logger/data_logger.h"

DataLoggerSchema b_box_schema;

static const char* name = "B box logs";
static const uint8_t num_column = 2;
static const DataLoggerType column_type[] = {
    DATA_LOGGER_TYPE_UINT32,
    DATA_LOGGER_TYPE_FLOAT 
};
static const char* column_name[2] = {
    "number",
    "num_square",  
};

void init_b_box_data_logger() {
    init_data_logger();
    //b_box_schema = (DataLoggerSchema){ name, num_column, column_type, column_name});
    b_box_schema.name = name;
    b_box_schema.num_column = num_column;
    b_box_schema.column_type = column_type;
    b_box_schema.column_name = column_name;
    data_logger_load_schema(&b_box_schema);
}

static uint32_t last_num = 5;

void b_box_data_logger_step() {
    data_logger_log(0, last_num, 0.5 + last_num*last_num);
    ++last_num;
}