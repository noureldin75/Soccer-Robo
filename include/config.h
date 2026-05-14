#ifndef CONFIG_H
#define CONFIG_H

// --- speed and timing ---
#define MAX_PWM       230      // don't go above this, keeps the motors safe
#define STEP_SIZE     5        // how much we change speed each loop cycle
#define LOOP_DELAY_MS 10       // ~100Hz loop
#define PWM_FREQ      20000    // 20kHz so you don't hear the motors whining
#define PWM_RESOLUTION 8       // 0-255 range

// --- driver protection ---
#define MIN_PWM              15    // below this the motor just buzzes and pulls a lot of current for nothing
#define DIRECTION_BRAKE_MS   20    // small brake gap when reversing direction to avoid voltage spikes
#define WATCHDOG_TIMEOUT_MS  500   // if nothing calls drive_robot() for this long, kill the motors

// --- joystick ---
#define JOYSTICK_DEADZONE 20  // ignore small stick drift

// --- left side motor pins (driver #1) ---
#define MOT1_PWM_PIN  4
#define MOT1_IN1_PIN  17
#define MOT1_IN2_PIN  16

#define MOT2_PWM_PIN  21
#define MOT2_IN1_PIN  18
#define MOT2_IN2_PIN  19

// --- right side motor pins (driver #2) ---
#define MOT3_PWM_PIN  32
#define MOT3_IN1_PIN  25
#define MOT3_IN2_PIN  33

#define MOT4_PWM_PIN  14
#define MOT4_IN1_PIN  26
#define MOT4_IN2_PIN  27

// --- LEDC channels (one per motor, ESP32 thing) ---
#define MOT1_PWM_CH   0
#define MOT2_PWM_CH   1
#define MOT3_PWM_CH   2
#define MOT4_PWM_CH   3

// STBY pins are wired straight to 5V on the board, so no need to touch them here

#endif