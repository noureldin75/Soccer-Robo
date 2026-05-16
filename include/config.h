#ifndef CONFIG_H
#define CONFIG_H

// --- speed and timing ---
#define MAX_PWM       200      // L298N drops ~2V across the bridge, so cap slightly lower than TB6612
#define STEP_SIZE     5        // how much we change speed each loop cycle
#define LOOP_DELAY_MS 10       // ~100Hz loop
#define PWM_FREQ      20000    // 20kHz so you don't hear the motors whining
#define PWM_RESOLUTION 8       // 0-255 range

// --- driver protection ---
#define MIN_PWM              20    // L298N needs a bit more to overcome its internal drop vs TB6612
#define DIRECTION_BRAKE_MS   20    // small brake gap when reversing direction to avoid voltage spikes
#define WATCHDOG_TIMEOUT_MS  500   // if nothing calls drive_robot() for this long, kill the motors

// --- joystick ---
#define JOYSTICK_DEADZONE 20  // ignore small stick drift

// -----------------------------------------------------------------------
// L298N wiring - two L298N modules, one per side
//
// Left side  → L298N module #1
//   Motor A (MOT1): ENA = PWM speed, IN1/IN2 = direction
//   Motor B (MOT2): ENB = PWM speed, IN3/IN4 = direction
//
// Right side → L298N module #2
//   Motor A (MOT3): ENA = PWM speed, IN1/IN2 = direction
//   Motor B (MOT4): ENB = PWM speed, IN3/IN4 = direction
//
// NOTE: Remove the ENA/ENB jumpers on the L298N board or PWM won't work!
// -----------------------------------------------------------------------

// --- left side motor pins (L298N module #1) ---
#define MOT1_PWM_PIN  4    // ENA on module 1
#define MOT1_IN1_PIN  17   // IN1
#define MOT1_IN2_PIN  16   // IN2

#define MOT2_PWM_PIN  21   // ENB on module 1
#define MOT2_IN1_PIN  18   // IN3
#define MOT2_IN2_PIN  19   // IN4

// --- right side motor pins (L298N module #2) ---
#define MOT3_PWM_PIN  32   // ENA on module 2
#define MOT3_IN1_PIN  25   // IN1
#define MOT3_IN2_PIN  33   // IN2

#define MOT4_PWM_PIN  14   // ENB on module 2
#define MOT4_IN1_PIN  26   // IN3
#define MOT4_IN2_PIN  27   // IN4

// --- LEDC channels (one per motor, ESP32 thing) ---
#define MOT1_PWM_CH   0
#define MOT2_PWM_CH   1
#define MOT3_PWM_CH   2
#define MOT4_PWM_CH   3

// L298N has no STBY pin - the module is always active when powered

#endif