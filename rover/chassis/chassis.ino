#include "motor.h"
#include "bt_controls.h"
#include <HCSR04.h>
const byte triggerPin = 13;
const byte echoPin =  34;
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);
float runl;
float runr;
float distance;
struct Controller {
  bool useWifi;
  float xAxis;
  float yAxis;
  uint8_t button;
} controller = { .useWifi = false };

void setup() {
  Serial.begin(115200);
  if (controller.useWifi) { /* setup wifi */ }
  else { setupRadio(); }
  initMotors();
  runLeft(0.0);
  runRight(0.0);
}

void loop() {
  if (controller.useWifi) {
    //
  } else {
    BTControls input;
    bool success = readRadioInput(&input);
    controller.xAxis = success ? input.x : 0;
    controller.yAxis = success ? -input.y : 0;
    controller.button = success ? input.b : 0;
  }
  distance = distanceSensor.measureDistanceCm();
  if (distance > 0 && distance < 15){
    runl = controller.yAxis + controller.xAxis;
    runr = controller.yAxis - controller.xAxis;
    if (runl > 0 && runr > 0){
      runLeft(0);
      runRight(0);
    }
    else{
      runLeft(controller.yAxis + controller.xAxis);
      runRight(controller.yAxis - controller.xAxis);
    }
  }
  else{
  runLeft(controller.yAxis + controller.xAxis);
  runRight(controller.yAxis - controller.xAxis);
  }
  delay(50);
  Serial.printf("X:%f Y:%f B:%i\n", controller.xAxis, controller.yAxis, controller.button);
}
