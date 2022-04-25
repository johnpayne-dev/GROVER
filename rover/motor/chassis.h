#ifndef CHASSIS_HEADER_GUARD
#define CHASSIS_HEADER_GUARD

#include <Arduino.h>

typedef struct {
  uint8_t en;
  uint8_t in1;
  uint8_t in2;
} MotorPinout;

extern const MotorPinout frontLeft;
extern const MotorPinout backLeft;
extern const MotorPinout frontRight;
extern const MotorPinout backRight;
extern const MotorPinout motors[4];

void initMotors();
void runMotor(MotorPinout motor, float throttle);
void runLeft(float throttle);
void runRight(float throttle);

#endif
