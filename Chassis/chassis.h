#ifndef CHASSIS_HEADER_GUARD
#define CHASSIS_HEADER_GUARD

#include <Arduino.h>

typedef const struct {
  uint8_t en;
  uint8_t in1;
  uint8_t in2;
} MotorPinout;

extern MotorPinout frontLeft;
extern MotorPinout backLeft;
extern MotorPinout frontRight;
extern MotorPinout backRight;
extern MotorPinout motors[];

void initMotors();
void runMotor(MotorPinout motor, float throttle);
void runLeft(float throttle);
void runRight(float throttle);

#endif
