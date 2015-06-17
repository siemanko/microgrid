#include "balance.h"
#include "../utils/debug.h"
#include "constants.h"
#include "demand_response/common.h"
#include "demand_response/b_box.h"total_balance_set
#include "../drivers/timer.h"
#include "storage.h"
#include "../drivers/button.h"
#include <stdio.h>
#include <stdint.h>

static uint32_t balance = INITIAL_BALANCE;
static uint32_t last_price = 0;

uint32_t balance_get() {
    return balance;
}

void balance_set(uint32_t value) {
    balance = value;
}

uint32_t get_total_balance(){
    return eeprom_read_uint32(STORAGE_TOTAL_INITIAL_BALANCE);
}


void total_balance_set(uint32_t value){
    eeprom_write_uint32( STORAGE_TOTAL_INITIAL_BALANCE , value);
}

int32_t balance_estimated_time_remaining() {
    if (last_price == 0) {
        return -1;
    } else {        
        return balance / last_price;
    }
}


void balance_step() {    
  
    //debug(DEBUG_INFO, "g s : %s ", dr_state_as_string(b_box_demand_reponse_current_state()));
    if (balance > 0) {
        if (b_box_readings_ready()) {
            
            //debug(DEBUG_GRID_STATE, "here %d %f",sizeof(float), power);
            uint32_t price = (int)b_box_get_power();
            price *= state_to_price_coefficient(
                    b_box_demand_reponse_current_state());
            last_price = price;
            if (price > balance_get()) {
                balance_set(0);
            } else {
                balance_set(balance_get() - price);                
            }
        }
    }
    
    //Reset balance if we're at midnight
    if( ulink_local_time()->tm_hour == 0 && ulink_local_time()->tm_min == 0 ){
        balance_set(get_total_balance());
    }        
    
}