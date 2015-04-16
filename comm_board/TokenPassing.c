#include "TokenPassing.h"

#include "DateAndTime.h"
#include "NodalCommunicationDriver.h"
#include "debugging.h"
#include "LibraryFunctions.h"
#include "set.h"

// If this value is 10 we will attempt connections once every 10
// ACCEPTING_CONNECTIONS messages.
#define CONNECTION_ATTEMPT_FREQUENCY           5

// how many loops do we allow for connections.
// this is a hack and should be later changed to async
// timer.
// Better approach would be to allow connections less frequently
// and wait for singals for longer.
#define ROUNDS_FOR_CONNECTION                  40

#define ROUNDS_BEFORE_DISCONNECT               40
// 0 - slave, 1 - master
int tp_mode = 0;

uint8_t master_uid = 255;

// num rounds waited for connections
unsigned int rounds_in_current_state = 0;

Set connected_nodes_set;
uint8_t last_token_destination;


SlaveState slave_state = MASTER_UNJAMMING;
MasterState master_state = SLAVE_DISCONNECTED;

void initTokenPassing(int as_master) {
    tp_mode = as_master;

    slave_state = MASTER_UNJAMMING;
    master_state = SLAVE_DISCONNECTED;

    make_set(&connected_nodes_set);

    master_uid = 255;

    rounds_in_current_state = 0;
}

void resetTokenPassing(void) {
    if (tp_mode == TP_MODE_SLAVE) {
        slave_state = SLAVE_DISCONNECTED;
    } else if (tp_mode == TP_MODE_MASTER) {
        master_state = MASTER_UNJAMMING;
    }
}

void considerReset() {
    // TODO: In case of master reset if token has not returned for some time,
    // also disconnect the token owner.
    // TODO: In case of slave reset if token has not come for long time.
    // EASY!
}

int getTPState() {
    if (tp_mode == TP_MODE_MASTER) return master_state;
    if (tp_mode == TP_MODE_SLAVE) return slave_state;

}

void tokenPassingLogic(void) {
    ++rounds_in_current_state;
    int old_state = getTPState();

    if (tp_mode == TP_MODE_SLAVE) {
        slaveTokenPassingLogic();
    } else if (tp_mode == TP_MODE_MASTER) {
        masterTokenPassingLogic();
    }

    if (getTPState() != old_state) rounds_in_current_state = 0;
}

void masterTokenPassingLogic(void) {

    if (master_state == MASTER_UNJAMMING) {
        unjamming();
        master_state = MASTER_CONNECTIONS;
    } else if (master_state == MASTER_CONNECTIONS) {
        send_node_mb_unsafe(NODAL_BOARDCAST_ADDRESS,
                            NODALMSG_ACCEPTING_CONNECTIONS, 0);
        master_state = MASTER_AWAITING_CONNECTIONS;
    } else if (master_state == MASTER_AWAITING_CONNECTIONS) {
        if (rounds_in_current_state >= ROUNDS_FOR_CONNECTION)
            master_state = MASTER_IHAZTOKEN; // MASTER_IHAZTOKEN;
    } else if (master_state == MASTER_IHAZTOKEN) {
        onNodalSendOpportunity();
        // TODO: reset token sending logic.
        last_token_destination = 255;
        master_state = MASTER_TOKEN_IN;
    } else if (master_state == MASTER_TOKEN_OUT) {
        if (rounds_in_current_state >= ROUNDS_BEFORE_DISCONNECT) {
            set_remove(&connected_nodes_set, last_token_destination);
            debug(DEBUG_TOKENPASSING, "NODE %d DISCONNECTED", last_token_destination);

            master_state = MASTER_UNJAMMING;
        }
    } else if (master_state == MASTER_TOKEN_IN) {
        uint8_t next_guy;
        int found;
        if (last_token_destination == 255) {
            found = set_lower_bound(&connected_nodes_set, 0, &next_guy);
        } else {
            found = set_lower_bound(&connected_nodes_set,
                                    last_token_destination + 1,
                                    &next_guy);
        }
        if (found) {
            debug(DEBUG_TOKENPASSING, "Sent token to %d", next_guy);
            last_token_destination = next_guy;
            send_node_mb_unsafe(next_guy, NODALMSG_TOKEN_IN, 0);
            master_state = MASTER_TOKEN_OUT;
        } else {
            master_state = MASTER_UNJAMMING;
        }
    }
}

void slaveTokenPassingLogic(void) {
    if (slave_state == SLAVE_CONNECTED &&
        rounds_in_current_state >= ROUNDS_BEFORE_DISCONNECT) {
        debug(DEBUG_TOKENPASSING, "DISCONNECTED FROM NETWORK");
        slave_state = SLAVE_DISCONNECTED;
    }
}

void on_token_passing_message(NetworkMessage* msg) {
    if (tp_mode == TP_MODE_MASTER) {
        onTokenMessageMaster(msg);
    } else if (tp_mode == TP_MODE_SLAVE) {
        onTokenMessageSlave(msg);
    }
}

void tpPingConnectedNodes() {
    int i;
    for(i=0; i<connected_nodes_set.size; ++i) {
        send_ping(connected_nodes_set.contents[i]);
    }
}
void onTokenMessageSlave(NetworkMessage* msg) {
    if (msg->type == NODALMSG_ACCEPTING_CONNECTIONS &&
            slave_state == SLAVE_DISCONNECTED) {
        uint16_t ticket = getRandomInt();
        if (ticket % CONNECTION_ATTEMPT_FREQUENCY == 0) {
            send_node_mb_unsafe(msg->source, NODALMSG_CONNECTION_REQUEST, 0);
        }
    } else if (msg->type == NODALMSG_TOKEN_IN) {
        debug(DEBUG_TOKENPASSING, "GOT TOKEN");
        if (slave_state == SLAVE_DISCONNECTED) {
            debug(DEBUG_TOKENPASSING, "CONNECTED TO NETWORK");
            master_uid = msg->source;
            slave_state = SLAVE_CONNECTED;
        }
        onNodalSendOpportunity();
        send_node_mb_unsafe(msg->source, NODALMSG_TOKEN_OUT, 0);
    }
}

void tpConnections(void) {
    debug(DEBUG_NETWORK, "CONNECTED NODES:");
    set_print_to_computer(&connected_nodes_set, DEBUG_NETWORK);
}

void onTokenMessageMaster(NetworkMessage* msg) {
    if (msg->type == NODALMSG_CONNECTION_REQUEST) {
        set_add(&connected_nodes_set, msg->source);
        debug(DEBUG_TOKENPASSING, "MASTER ACCETED CONNECTION FROM %d", msg->source);
    } else if (msg->type == NODALMSG_TOKEN_OUT) {
        master_state = MASTER_TOKEN_IN;
    }
}

uint8_t getMasterUid(void) {
    return master_uid;
}

int isConnected(void) {
    if (tp_mode == TP_MODE_MASTER) {
        return 1;
    } else if (tp_mode == TP_MODE_SLAVE) {
        return slave_state == SLAVE_CONNECTED;
    }
}