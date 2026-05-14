#include <Arduino.h>
#include "config.h"
#include "motors.h"
#include "rc.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Soccer Robot starting up...");

    setup_motors();
    setup_rc();

    Serial.println("ready to go");
}

void loop() {
    process_rc();
    check_watchdog();
    delay(LOOP_DELAY_MS);
}