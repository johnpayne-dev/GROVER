#include <HCSR04.h>
#include "motor.h"
#include "bt_controls.h"
#include "web_controls.h"

#define ULTRASONIC_TRIGGER_PIN 13
#define ULTRASONIC_ECHO_PIN    34
UltraSonicDistanceSensor distanceSensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);
#define AUTO_BREAKING_THRESHOLD 15

#define WIFI_MODE_PIN 35
#define LED_PIN       2

struct Controller {
  bool useWifi;
  float xAxis;
  float yAxis;
  uint8_t button;
} controller;

void setup() {
	Serial.begin(115200);
	pinMode(LED_PIN, OUTPUT);
	pinMode(WIFI_MODE_PIN, INPUT_PULLUP);
	controller = (struct Controller){ .useWifi = !digitalRead(WIFI_MODE_PIN) };
	if (controller.useWifi) { setupWifi(); }
	else { setupRadio(); }
	initMotors();
	runLeft(0.0);
	runRight(0.0);
}

void loop() {
	if (controller.useWifi) {
		WebControls input;
		bool success = getWebControls(&input);
		controller.xAxis = success ? input.right - input.left : 0;
		controller.yAxis = success ? input.up - input.down : 0;
	} else {
		BTControls input;
		bool success = readRadioInput(&input);
		controller.xAxis = success ? input.x : 0;
		controller.yAxis = success ? -input.y : 0;
		controller.button = success ? input.b : 0;
	}
	
	float distance = distanceSensor.measureDistanceCm();
	float runl = controller.yAxis + controller.xAxis;
	float runr = controller.yAxis - controller.xAxis;
	if (distance > 0 && distance < AUTO_BREAKING_THRESHOLD && runl > 0 && runr > 0) {
		runLeft(0);
		runRight(0);
	} else {
		runLeft(runl);
		runRight(runr);
	}

	if (!digitalRead(WIFI_MODE_PIN) != controller.useWifi) {
		Serial.printf("remote mode changed, restarting...\n");
		ESP.restart();
	}

	Serial.printf("X:%f Y:%f B:%i\n", controller.xAxis, controller.yAxis, controller.button);
	delay(50);
}
