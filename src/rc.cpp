#include "config.h"
#include "motors.h"
#include "rc.h"
#include <Arduino.h>
#include <Bluepad32.h>

static GamepadPtr myGamepad = nullptr;

// filters out tiny joystick values so the robot doesn't drift when you let go
static int apply_deadzone(int value) {
    return (abs(value) < JOYSTICK_DEADZONE) ? 0 : value;
}

// called automatically when a controller pairs
static void onConnectedGamepad(GamepadPtr gp) {
    if (myGamepad == nullptr) {
        myGamepad = gp;
        Serial.println("[RC] gamepad connected");
    } else {
        Serial.println("[RC] another gamepad tried to connect, ignoring");
    }
}

// called if the controller disconnects (battery died, out of range, etc)
static void onDisconnectedGamepad(GamepadPtr gp) {
    if (myGamepad == gp) {
        myGamepad = nullptr;
        stop_all_motors();  // don't let the robot keep going without a controller!
        Serial.println("[RC] gamepad lost, motors stopped");
    }
}

void setup_rc() {
    Serial.println("[RC] starting bluetooth...");
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys();  // clear old pairings so any controller can connect
    Serial.println("[RC] waiting for gamepad...");
}

void process_rc() {
    BP32.update();  // needs to run every loop for bluetooth to work

    if (myGamepad == nullptr || !myGamepad->isConnected()) {
        drive_robot(0, 0);  // no controller = slow down to stop
        return;
    }

    // read the left stick
    // bluepad gives values from -511 to 512
    int raw_y = myGamepad->axisY();  // up is negative which is weird
    int raw_x = myGamepad->axisX();

    raw_y = -raw_y;  // flip it so pushing up = positive = forward

    int axis_y = apply_deadzone(raw_y);
    int axis_x = apply_deadzone(raw_x);

    // scale joystick range to our PWM range
    int mapped_y = map(axis_y, -512, 512, -MAX_PWM, MAX_PWM);
    int mapped_x = map(axis_x, -512, 512, -MAX_PWM, MAX_PWM);

    // arcade drive: left = Y+X, right = Y-X
    int target_left  = constrain(mapped_y + mapped_x, -MAX_PWM, MAX_PWM);
    int target_right = constrain(mapped_y - mapped_x, -MAX_PWM, MAX_PWM);

    drive_robot(target_left, target_right);
}