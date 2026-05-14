#ifndef RC_H
#define RC_H

#include <Arduino.h>

// sets up bluetooth and registers the gamepad callbacks, call in setup()
void setup_rc();

// reads the gamepad, does the arcade drive math, and sends speeds to the motors
void process_rc();

#endif
