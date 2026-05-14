// quick motor test - no bluetooth needed
// just spins each motor one by one so you can check they're all wired right
//
// flash with: pio run -e test_motors -t upload
// then open serial monitor at 115200

#include <Arduino.h>
#include "config.h"
#include "motors.h"

#define TEST_SPEED      150
#define TEST_RAMP_SPEED 200
#define SPIN_TIME_MS    2000
#define PAUSE_TIME_MS   1000
#define RAMP_STEP_DELAY 20

static void print_line() {
    Serial.println("----------------------------------------");
}

static void pause(const char* msg) {
    Serial.println(msg);
    delay(PAUSE_TIME_MS);
}

// runs one motor at a given speed, prints result
static void test_single_motor(const char* label,
                              int pwm_ch, int in1, int in2,
                              int speed, unsigned long duration_ms) {
    Serial.printf("  %-18s  speed=%+4d ...", label, speed);
    set_motor(pwm_ch, in1, in2, speed);
    delay(duration_ms);
    set_motor(pwm_ch, in1, in2, 0);
    Serial.println(" done");
}

// test 1: spin each motor forward then reverse
static void test_individual() {
    print_line();
    Serial.println("TEST 1: one motor at a time");
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

        test_single_motor("FWD", motors[i].pwm_ch,
                          motors[i].in1, motors[i].in2,
                          TEST_SPEED, SPIN_TIME_MS);
        delay(500);

        test_single_motor("REV", motors[i].pwm_ch,
                          motors[i].in1, motors[i].in2,
                          -TEST_SPEED, SPIN_TIME_MS);
        pause("  ---");
    }
}

// test 2: all 4 motors at once
static void test_all_together() {
    print_line();
    Serial.println("TEST 2: all motors together");
    print_line();

    Serial.println("  all forward...");
    set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, TEST_SPEED);
    set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, TEST_SPEED);
    set_motor(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN, TEST_SPEED);
    set_motor(MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN, TEST_SPEED);
    delay(SPIN_TIME_MS);

    stop_all_motors();
    pause("  ---");

    Serial.println("  all reverse...");
    set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, -TEST_SPEED);
    set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, -TEST_SPEED);
    set_motor(MOT3_PWM_CH, MOT3_IN1_PIN, MOT3_IN2_PIN, -TEST_SPEED);
    set_motor(MOT4_PWM_CH, MOT4_IN1_PIN, MOT4_IN2_PIN, -TEST_SPEED);
    delay(SPIN_TIME_MS);

    stop_all_motors();
    pause("  ---");
}

// test 3: checks that soft start actually ramps smoothly
static void test_ramp() {
    print_line();
    Serial.println("TEST 3: soft start ramp (left side only)");
    print_line();

    int current = 0;
    int target  = TEST_RAMP_SPEED;

    Serial.println("  ramping up...");
    while (current != target) {
        current = soft_start(current, target);
        set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, current);
        set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, current);
        delay(RAMP_STEP_DELAY);
    }
    delay(500);

    Serial.println("  ramping down to 0...");
    target = 0;
    while (current != target) {
        current = soft_start(current, target);
        set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, current);
        set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, current);
        delay(RAMP_STEP_DELAY);
    }
    delay(500);

    Serial.println("  ramping into reverse...");
    target = -TEST_RAMP_SPEED;
    while (current != target) {
        current = soft_start(current, target);
        set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, current);
        set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, current);
        delay(RAMP_STEP_DELAY);
    }
    delay(500);

    Serial.println("  back to 0...");
    target = 0;
    while (current != target) {
        current = soft_start(current, target);
        set_motor(MOT1_PWM_CH, MOT1_IN1_PIN, MOT1_IN2_PIN, current);
        set_motor(MOT2_PWM_CH, MOT2_IN1_PIN, MOT2_IN2_PIN, current);
        delay(RAMP_STEP_DELAY);
    }

    stop_all_motors();
    pause("  ramp test done");
}

// test 4: simulates actual driving commands without needing a gamepad
static void test_drive_commands() {
    print_line();
    Serial.println("TEST 4: drive simulation");
    print_line();

    const struct {
        const char* label;
        int left_speed, right_speed;
    } commands[] = {
        {"FORWARD",    TEST_SPEED,   TEST_SPEED},
        {"TURN LEFT",  0,            TEST_SPEED},
        {"TURN RIGHT", TEST_SPEED,   0},
        {"REVERSE",   -TEST_SPEED,  -TEST_SPEED},
        {"STOP",       0,            0},
    };

    for (int i = 0; i < 5; i++) {
        Serial.printf("  %-12s  L=%+4d  R=%+4d\n",
                      commands[i].label,
                      commands[i].left_speed,
                      commands[i].right_speed);

        // keep calling drive_robot so the soft start can do its thing
        unsigned long start = millis();
        while (millis() - start < SPIN_TIME_MS) {
            drive_robot(commands[i].left_speed, commands[i].right_speed);
            delay(LOOP_DELAY_MS);
        }
        delay(300);
    }

    stop_all_motors();
    pause("  drive test done");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("MOTOR TEST");
    Serial.println("put the robot on a stand!");
    Serial.println("starting in 3 sec...");
    delay(3000);

    setup_motors();

    test_individual();
    test_all_together();
    test_ramp();
    test_drive_commands();

    Serial.println();
    print_line();
    Serial.println("all tests done");
    Serial.println("check above for any motor that didn't spin");
    print_line();
}

void loop() {
    // tests only run once, nothing to do here
}
