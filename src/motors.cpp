#include "config.h"
#include "motors.h"
#include <Arduino.h>

// keeps track of where we are in the soft start ramp
static int current_left_speed  = 0;
static int current_right_speed = 0;

// remembering which way each side was going so we know when it flips
static int prev_left_dir  = 0;   // -1, 0, or +1
static int prev_right_dir = 0;

// for the watchdog - when did we last call drive_robot()
static unsigned long last_drive_time_ms = 0;
static bool watchdog_tripped = false;

// just returns -1, 0, or 1
static inline int sign(int x) {
    return (x > 0) - (x < 0);
}

// sets up one motor's pins and PWM channel
static void init_motor(int pwm_pin, int pwm_channel, int in1_pin, int in2_pin) {
    pinMode(in1_pin, OUTPUT);
    pinMode(in2_pin, OUTPUT);
    digitalWrite(in1_pin, LOW);
    digitalWrite(in2_pin, LOW);

    // ESP32 doesn't have analogWrite, gotta use LEDC instead
    ledcSetup(pwm_channel, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(pwm_pin, pwm_channel);
    ledcWrite(pwm_channel, 0);
}

// cuts power to one motor
static void brake_motor(int pwm_channel, int in1_pin, int in2_pin) {
    digitalWrite(in1_pin, LOW);
    digitalWrite(in2_pin, LOW);
    ledcWrite(pwm_channel, 0);
}

void setup_motors() {
    // STBY is hardwired to 5V on the PCB so we skip that

    // left side
    init_motor(MOT1_PWM_PIN, MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN);
    init_motor(MOT2_PWM_PIN, MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN);

    // right side
    init_motor(MOT3_PWM_PIN, MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN);
    init_motor(MOT4_PWM_PIN, MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN);

    last_drive_time_ms = millis();
    Serial.println("[MOT] motors ready");
}

// if we're going forward and suddenly asked to go backward (or vice versa),
// we don't jump straight there. we ramp down to 0 first, then ramp back up
// in the new direction. protects the driver from sudden current spikes.
int soft_start(int current, int target) {
    // check if we're trying to reverse direction
    if ((current > 0 && target < 0) || (current < 0 && target > 0)) {
        // gotta go to 0 first before switching
        if (current > 0) {
            current -= STEP_SIZE;
            if (current < 0) current = 0;
        } else {
            current += STEP_SIZE;
            if (current > 0) current = 0;
        }
        return current;
    }

    // normal case, just step toward target
    if (current < target) {
        current += STEP_SIZE;
        if (current > target) current = target;
    } else if (current > target) {
        current -= STEP_SIZE;
        if (current < target) current = target;
    }
    return current;
}

// sends speed to one motor. handles direction pins and clamps the PWM.
// also filters out really low duty cycles that would just make the motor
// buzz without actually spinning (bad for the driver)
void set_motor(int pwm_channel, int in1_pin, int in2_pin, int speed) {
    int duty = min(abs(speed), MAX_PWM);

    // too low to actually spin, just treat as 0
    if (duty > 0 && duty < MIN_PWM) {
        duty = 0;
        speed = 0;
    }

    if (speed > 0) {
        digitalWrite(in1_pin, HIGH);
        digitalWrite(in2_pin, LOW);
    } else if (speed < 0) {
        digitalWrite(in1_pin, LOW);
        digitalWrite(in2_pin, HIGH);
    } else {
        digitalWrite(in1_pin, LOW);
        digitalWrite(in2_pin, LOW);
        duty = 0;
    }

    ledcWrite(pwm_channel, duty);
}

// brakes both motors on one side with a small delay to let back-EMF die down
static void brake_side(int ch_a, int in1_a, int in2_a,
                       int ch_b, int in1_b, int in2_b) {
    brake_motor(ch_a, in1_a, in2_a);
    brake_motor(ch_b, in1_b, in2_b);
    delay(DIRECTION_BRAKE_MS);
}

void drive_robot(int target_left_speed, int target_right_speed) {
    // we're alive, reset the watchdog
    last_drive_time_ms = millis();
    watchdog_tripped = false;

    // ramp toward the targets
    current_left_speed  = soft_start(current_left_speed,  target_left_speed);
    current_right_speed = soft_start(current_right_speed, target_right_speed);

    // figure out if direction actually changed on either side
    int new_left_dir  = sign(current_left_speed);
    int new_right_dir = sign(current_right_speed);

    // if left side just flipped direction, brake briefly first
    if (prev_left_dir != 0 && new_left_dir != 0 && prev_left_dir != new_left_dir) {
        brake_side(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN,
                   MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN);
    }

    // same for right side
    if (prev_right_dir != 0 && new_right_dir != 0 && prev_right_dir != new_right_dir) {
        brake_side(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN,
                   MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN);
    }

    prev_left_dir  = new_left_dir;
    prev_right_dir = new_right_dir;

    // actually send it to the motors
    set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, current_left_speed);
    set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, current_left_speed);
    set_motor(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN, current_right_speed);
    set_motor(MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN, current_right_speed);
}

void stop_all_motors() {
    current_left_speed  = 0;
    current_right_speed = 0;
    prev_left_dir  = 0;
    prev_right_dir = 0;

    set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, 0);
    set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, 0);
    set_motor(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN, 0);
    set_motor(MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN, 0);
}

// if drive_robot() hasn't been called in a while, something probably crashed
// so we just stop everything to be safe
void check_watchdog() {
    if (watchdog_tripped) return;

    unsigned long now = millis();
    if ((now - last_drive_time_ms) > WATCHDOG_TIMEOUT_MS) {
        Serial.println("[MOT] watchdog triggered, stopping motors");
        stop_all_motors();
        watchdog_tripped = true;
    }
}