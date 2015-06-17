#include "a_box.h"

#include "demand_response/common.h"
#include "communication/messages.h"
#include "communication/interface.h"
#include "shared/utils.h"
#include "utils/debug.h"
#include "demand_response/state_of_charge.h"
#include "constants.h"
#include "storage.h"

static int override_active;
static DemandResponeState override_state;
static DemandResponeState a_box_current_state;


float off_threshold    =  DEFAULT_OFF_THRESHOLD;
float red_threshold    =  DEFAULT_RED_THRESHOLD;
float yellow_threshold =  DEFAULT_YELLOW_THRESHOLD;


static void override_demand_reponse_handler(Message* msg) {
    assert(0 <= msg->content[1] && msg->content[1] <= 1);
    assert(0 <= msg->content[2] && msg->content[2] < DR_STATE_TOTAL);
    override_active = msg->content[1];
    override_state = msg->content[2];
    if (override_active) {
        //debug(DEBUG_INFO, "Overriding demand response state to %s.",
        //        dr_state_as_string(override_state));
    } else {
        //debug(DEBUG_INFO, "Demand response state override disabled.");
    }
}

static void set_thresholds_handler(Message* msg) {
    assert(msg->length == 13);
    off_threshold =    bytes_to_float(msg->content + 1);
    red_threshold =    bytes_to_float(msg->content + 5);
    yellow_threshold = bytes_to_float(msg->content + 9);
    
    eeprom_write_float(STORAGE_OFF_THRESHOLD, off_threshold);
    eeprom_write_float(STORAGE_RED_THRESHOLD, red_threshold);
    eeprom_write_float(STORAGE_YELLOW_THRESHOLD, yellow_threshold);

}

void init_a_box_demand_response() {
    override_active = 0;
    a_box_current_state = DR_STATE_GREEN;
    set_message_handler(UMSG_OVERRIDE_DEMAND_REPONSE,
            override_demand_reponse_handler);
    set_message_handler(UMSG_SET_THRESHOLDS,
            set_thresholds_handler);
    init_state_of_charge();
}

//DS:  Edit, CURRENT ERROR IS HERE
DemandResponeState a_box_demand_reponse_current_state() {
     
    if (override_active) {
        return override_state;
    } else {
       float soc = get_state_of_charge_percentage(); 
       
       if( a_box_current_state == DR_STATE_OFF && soc > off_threshold + GRID_STATE_HYSTERESIS_PERCENT ){
           a_box_current_state = DR_STATE_RED;        
           
       } else if( a_box_current_state == DR_STATE_RED && soc > red_threshold + GRID_STATE_HYSTERESIS_PERCENT ){
           a_box_current_state = DR_STATE_YELLOW;   
           
       } else if( a_box_current_state == DR_STATE_RED && soc < off_threshold - GRID_STATE_HYSTERESIS_PERCENT ){
           a_box_current_state = DR_STATE_OFF;        
           
       } else if( a_box_current_state == DR_STATE_YELLOW && soc > yellow_threshold + GRID_STATE_HYSTERESIS_PERCENT ){
           a_box_current_state = DR_STATE_GREEN;       
           
       } else if( a_box_current_state == DR_STATE_YELLOW && soc < red_threshold - GRID_STATE_HYSTERESIS_PERCENT){
           a_box_current_state = DR_STATE_RED;
           
       } else if( a_box_current_state == DR_STATE_GREEN && soc < yellow_threshold - GRID_STATE_HYSTERESIS_PERCENT){
           a_box_current_state = DR_STATE_YELLOW;
           
       }    
       return a_box_current_state;
    }
 }

static void broadcast_state() {
    MessageBuilder mb;
    make_mb(&mb, 2);
    mb_add_char(&mb, UMSG_DEMAND_REPONSE);
    
    //DS:  Edit, debug.  PRINT OUT THE STATE OF CHARGE
    //debug(DEBUG_INFO, "a box demand response is : %s", dr_state_as_string(a_box_demand_reponse_current_state()) );
            
    mb_add_char(&mb, (char)a_box_demand_reponse_current_state());
    send_mb(&mb, BROADCAST_UID);
}


void a_box_demand_response_step() {
    state_of_charge_step();
    broadcast_state();      
}