#include <BluetoothSerial.h>

#define BT_REMOTE_NAME "GROVER-Remote"

#define JOYSTICK_VRX_PIN    25
#define JOYSTICK_VRY_PIN    33
#define JOYSTICK_RESOLUTION (1 << 12)
#define JOYSTICK_SW_PIN     32
#define JOYSTICK_DEADZONE   0.2

#define SYNC_BYTE 0x45

BluetoothSerial SerialBT;

void setup() {
	Serial.begin(115200);
	SerialBT.begin(BT_REMOTE_NAME);
	pinMode(JOYSTICK_VRX_PIN, INPUT);
	pinMode(JOYSTICK_VRY_PIN, INPUT);
	pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
}

void loop() {
	uint16_t vrx = analogRead(JOYSTICK_VRX_PIN);
	uint16_t vry = analogRead(JOYSTICK_VRY_PIN);
	uint8_t sw = digitalRead(JOYSTICK_SW_PIN);
	
	float x = 2.0 * vrx / JOYSTICK_RESOLUTION - 1.0;
	float y = 2.0 * vry / JOYSTICK_RESOLUTION - 1.0;
	if (fabsf(x) < JOYSTICK_DEADZONE) { vrx = JOYSTICK_RESOLUTION / 2; }
	if (fabsf(y) < JOYSTICK_DEADZONE) { vry = JOYSTICK_RESOLUTION / 2; }

	SerialBT.write(SYNC_BYTE);
	SerialBT.write('x');
	SerialBT.write(vrx >> 8);
	SerialBT.write(vrx & 0xFF);

	SerialBT.write(SYNC_BYTE);
	SerialBT.write('y');
	SerialBT.write(vry >> 8);
	SerialBT.write(vry & 0xFF);

	SerialBT.write(SYNC_BYTE);
	SerialBT.write('b');
	SerialBT.write(0);
	SerialBT.write(1 - sw);
	
	Serial.printf("X:%f Y:%f B:%f\r\n", x, y, (float)sw);
}
