// motor test - L298N
// sequence:
//   1. all 4 motors forward for 10 seconds
//   2. all 4 motors reverse for 10 seconds
//   3. each motor individually (forward 3s, reverse 3s)
//   4. all stop
//
// flash with: pio run -e test_motors -t upload
// then open serial monitor at 115200

#include <Arduino.h>
#include "config.h"
#include "motors.h"

#define TEST_SPEED     150
#define ALL_TIME_MS  10000   // 10 seconds for the group runs
#define SOLO_TIME_MS  3000   // 3 seconds per direction for individual test
#define PAUSE_MS       500

static void print_line() {
    Serial.println("----------------------------------------");
}

// -------------------------------------------------------
// STEP 1 & 2 — all motors together, forward then reverse
// -------------------------------------------------------
static void test_all_forward_reverse() {
    print_line();
    Serial.println("STEP 1: all motors FORWARD — 10 seconds");
    print_line();

    set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN,  TEST_SPEED);
    set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN,  TEST_SPEED);
    set_motor(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN,  TEST_SPEED);
    set_motor(MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN,  TEST_SPEED);
    delay(ALL_TIME_MS);

    stop_all_motors();
    delay(PAUSE_MS);

    print_line();
    Serial.println("STEP 2: all motors REVERSE — 10 seconds");
    print_line();

    set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, -TEST_SPEED);
    set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, -TEST_SPEED);
    set_motor(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN, -TEST_SPEED);
    set_motor(MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN, -TEST_SPEED);
    delay(ALL_TIME_MS);

    stop_all_motors();
    delay(PAUSE_MS);
}

// -------------------------------------------------------
// STEP 3 — each motor one at a time
// -------------------------------------------------------
static void test_individual() {
    print_line();
    Serial.println("STEP 3: each motor individually");
    print_line();

    const struct {
        const char* name;
        int pwm_ch, in1, in2;
    } motors[] = {
        {"MOT1 (Left-Front)",  MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN},
        {"MOT2 (Left-Rear)",   MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN},
        {"MOT3 (Right-Front)", MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN},
        {"MOT4 (Right-Rear)",  MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN},
    };

    for (int i = 0; i < 4; i++) {
        Serial.printf("\n>> %s\n", motors[i].name);

        Serial.println("   forward...");
        set_motor(motors[i].pwm_ch, motors[i].in1, motors[i].in2,  TEST_SPEED);
        delay(SOLO_TIME_MS);
        set_motor(motors[i].pwm_ch, motors[i].in1, motors[i].in2, 0);
        delay(PAUSE_MS);

        Serial.println("   reverse...");
        set_motor(motors[i].pwm_ch, motors[i].in1, motors[i].in2, -TEST_SPEED);
        delay(SOLO_TIME_MS);
        set_motor(motors[i].pwm_ch, motors[i].in1, motors[i].in2, 0);
        delay(PAUSE_MS);
    }
}

// -------------------------------------------------------
// STEP 4 — hard stop everything
// -------------------------------------------------------
static void test_stop() {
    print_line();
    Serial.println("STEP 4: all stop");
    print_line();

    stop_all_motors();
    Serial.println("done — all motors stopped");
}

// -------------------------------------------------------

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("=== MOTOR TEST (L298N) ===");
    Serial.println("put the robot on a stand!");
    Serial.println("starting in 3 sec...");
    delay(3000);

    setup_motors();

    test_all_forward_reverse();
    test_individual();
    test_stop();

    Serial.println();
    print_line();
    Serial.println("all tests done");
    print_line();
}

void loop() {
    // tests only run once in setup(), nothing to do here
}