#include "chassis.h"

const MotorPinout frontLeft  = { .en = 26, .in1 = 16, .in2 = 27 };
const MotorPinout backLeft   = { .en = 25, .in1 = 18, .in2 = 17 };
const MotorPinout frontRight = { .en = 32, .in1 = 22, .in2 = 23 };
const MotorPinout backRight  = { .en = 33, .in1 = 19, .in2 = 21 };
const MotorPinout motors[NUM_OF_MOTORS] = { frontLeft, backLeft, frontRight, backRight };

void initMotors() {
  for(int i = 0; i < NUM_OF_MOTORS; i++) {
    pinMode(motors[i].en, OUTPUT);
    pinMode(motors[i].in1, OUTPUT);
    pinMode(motors[i].in2, OUTPUT);
  }
}

void runMotor(MotorPinout motor, float throttle) {
  if (throttle > 1.0) { throttle = 1.0; }
  if (throttle < -1.0) { throttle = -1.0; }
  digitalWrite(motor.in1, throttle < 0.0);
  digitalWrite(motor.in2, throttle > 0.0);
  analogWrite(motor.en, 255.0 * fabsf(throttle));
}

void runLeft(float throttle) {
  runMotor(frontLeft, throttle);
  runMotor(backLeft, throttle);
}

void runRight(float throttle) {
  runMotor(frontRight, throttle);
  runMotor(backRight, throttle);
}
