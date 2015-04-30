#include "message.h"

 void message_free(Message* msg){
    if (msg != 0) {
        if (msg->content != 0) {
            free(msg->content);
        }
        free(msg);
    }
}