#ifndef RC_H
#define RC_H

#include <Arduino.h>

// starts the WiFi access point and web server, call in setup()
void setup_rc();

// handles incoming web server requests, call every loop()
void process_rc();

#endif