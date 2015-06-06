#include "data_logger.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "communication/interface.h"
#include "communication/messages.h"
#include "drivers/eeprom.h"
#include "shared/utils.h"
#include "shared/communication/message.h"
#include "storage.h"
#include "utils/debug.h"
#include "constants.h"
#include "shared/communication/utils/message_builder.h"

static DataLoggerSchema* current_schema;
static uint32_t          current_entry_size;

typedef enum {
    DLS_SCHEMA_HASH = 0,
    DLS_NUM_LOGGED = 4,     
    DLS_CURRENT_READING_POSITION = 8,       //DS:  Edit, added
    DLS_DATA = 10,      //DS:  Edit, used to be 8
} DataLoggerStorage;

static uint16_t dls(uint16_t offset) {
    return STORAGE_DATA_LOGGER + offset;
}

void dl_message_handler(Message* msg);

void init_data_logger() {
    set_message_handler(UMSG_DATA_LOGGER, dl_message_handler);
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

//DS Note:  Ignored means it's ignored because we already know num_column
void data_logger_log(int ignored, ...) {
    uint32_t offset = dls(DLS_DATA);
    uint32_t num_data = eeprom_read_uint32(dls(DLS_NUM_LOGGED));
    offset += num_data * current_entry_size;
    
    if (offset + current_entry_size >= EEPROM_TOTAL_MEMORY)
        return;
    
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
    eeprom_write_uint32(dls(DLS_NUM_LOGGED), num_data+1);       //HERE IS THE KEY PLACE 
    va_end(args);
}

void data_logger_reset() {
    eeprom_write_uint32(dls(DLS_SCHEMA_HASH), schema_hash(current_schema));
    eeprom_write_uint32(dls(DLS_NUM_LOGGED), 0);
    
    //DS:  Edit, starting the position of where you are reading from 
    eeprom_write_int(dls(DLS_CURRENT_READING_POSITION) , dls(DLS_DATA));       
}

// Message handling.

typedef enum {
    DLM_STATUS            =  1,
    DLM_EXTRACT_GENERAL   =  2,
    DLM_EXTRACT_COLUMN    =  3,
    DLM_EXTRACT_DATA      =  4,
    DLM_RESET             =  5,
} DataLoggerMessages;


void dl_message_handler(Message* msg) {
    assert(msg->length >= 2);
    if (msg->content[1] == DLM_STATUS) {
        assert(msg->length == 2);
        uint32_t num_logged = eeprom_read_uint32(dls(DLS_NUM_LOGGED));
        uint32_t num_possible =
                (EEPROM_TOTAL_MEMORY - STORAGE_DATA_LOGGER) / 
                current_entry_size - 1;
        debug(DEBUG_INFO, "Logging schema %s. Logged %l of %l possible).",
                current_schema->name, num_logged, num_possible);
    } else if (msg->content[1] == DLM_EXTRACT_GENERAL) {
        assert(msg->length == 2);
        uint32_t num_logged2 = eeprom_read_uint32(dls(DLS_NUM_LOGGED));
        MessageBuilder mb;
        make_mb(&mb, 11 + strlen(current_schema->name));
        mb_add_char(&mb, CMSG_DATA_LOGGER_REPLY);
        mb_add_char(&mb, DLM_EXTRACT_GENERAL);
        
        mb_add_char(&mb, eeprom_read_byte(STORAGE_UID));
        mb_add_uint32_noprefix(&mb, current_schema->num_column);
        mb_add_uint32_noprefix(&mb, num_logged2);
        
        mb_add_string(&mb, current_schema->name);
        send_mb(&mb, COMPUTER_UID);
    } else if (msg->content[1] == DLM_EXTRACT_COLUMN) {
        assert(msg->length == 3);
        uint8_t column = msg->content[2];
        assert(column <= current_schema->num_column);
        MessageBuilder mb2;
        make_mb(&mb2, 4 + strlen(current_schema->column_name[column]));
        mb_add_char(&mb2, CMSG_DATA_LOGGER_REPLY);
        mb_add_char(&mb2, DLM_EXTRACT_COLUMN);
        mb_add_char(&mb2, column);
        mb_add_char(&mb2, (uint8_t)current_schema->column_type[column]);
        mb_add_string(&mb2, current_schema->column_name[column]);
        send_mb(&mb2, COMPUTER_UID);
    } else if (msg->content[1] == DLM_EXTRACT_DATA) {
        
        assert(msg->length == 6);
        uint32_t entry_no = bytes_to_uint32(msg->content + 2);
        uint32_t num_logged2 = eeprom_read_uint32(dls(DLS_NUM_LOGGED));
        assert(entry_no < num_logged2);
        uint32_t offset = dls(DLS_DATA) + entry_no * current_entry_size;
        assert (offset + current_entry_size < EEPROM_TOTAL_MEMORY);
        MessageBuilder mb3;
        make_mb(&mb3, 6 + current_entry_size);
        mb_add_char(&mb3, CMSG_DATA_LOGGER_REPLY);
        mb_add_char(&mb3, DLM_EXTRACT_DATA);
        mb_add_uint32_noprefix(&mb3, entry_no);     
        
        int idx;
        for (idx = 0; idx < current_schema->num_column; ++idx) {
            DataLoggerType column_type = current_schema->column_type[idx];
            if (column_type == DATA_LOGGER_TYPE_FLOAT) {
                float f = eeprom_read_float(offset);
                mb_add_float_noprefix(&mb3, f);
                offset += 4;
            } else if (column_type == DATA_LOGGER_TYPE_UINT32) {
                uint32_t l =  eeprom_read_uint32(offset);
                mb_add_uint32_noprefix(&mb3, l);
                offset += 4;
            }
        }
        send_mb(&mb3, COMPUTER_UID);
    } else if (msg->content[1] == DLM_RESET) {
        debug(DEBUG_INFO, "Resetting data logger");
        data_logger_reset();  
    } else {
        assert(0);
    }
}