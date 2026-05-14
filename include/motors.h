#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

// call once in setup() to configure all the motor pins and PWM channels
void setup_motors();

// ramps current speed toward target by STEP_SIZE, also handles direction reversals
int soft_start(int current, int target);

// controls a single motor: positive speed = forward, negative = reverse, 0 = stop
void set_motor(int pwm_channel, int in1_pin, int in2_pin, int speed);

// takes target speeds for left and right sides, handles soft start and protection stuff internally
void drive_robot(int target_left_speed, int target_right_speed);

// hard stop, no ramping, just kills everything immediately
void stop_all_motors();

// checks if drive_robot() has been called recently, stops motors if the loop seems frozen
void check_watchdog();

#endif
