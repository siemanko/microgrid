#ifndef DRIVERS_BUTTON_H
#define DRIVERS_BUTTON_H

void init_button();

// returns true if button was recently pressed.
int button_check();

void button_reset();

void button_step();

void display_button_status();

#endif