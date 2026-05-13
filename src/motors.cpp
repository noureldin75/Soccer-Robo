#include "config.h"
#include <Arduino.h>

int soft_start(int current , int target){
    if(current< target){
        current += STEP_SIZE;
        if(current > target) current = target;
    } else if(current > target){
        current -= STEP_SIZE;
        if(current < target) current = target;
    }
    return current;
}
void set_motor(int pwm_pin, int in1_pin, int in2_pin, int speed){
    if(speed > 0){
        digitalWrite(in1_pin, HIGH);
        digitalWrite(in2_pin, LOW);
        analogWrite(pwm_pin, speed);
    } else if(speed < 0){
        digitalWrite(in1_pin, LOW);
        digitalWrite(in2_pin, HIGH);
        analogWrite(pwm_pin, -speed);
    } else {
        digitalWrite(in1_pin, LOW);
        digitalWrite(in2_pin, LOW);
        analogWrite(pwm_pin, 0);
    }
}