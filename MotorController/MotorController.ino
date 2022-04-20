#include <BluetoothSerial.h>

#define BT_REMOTE_NAME   "GROVER-Remote"
#define BT_MOTOR_NAME    "GROVER-Motor"
#define BT_DISCOVER_TIME 5000

#define JOYSTICK_RESOLUTION (1 << 12)
#define RADIO_SYNC_BYTE     0x45

typedef struct MotorPinout {
	uint8_t en;
	uint8_t in1;
	uint8_t in2;
} MotorPinout;

const MotorPinout frontLeft  = { .en = 25, .in1 = 18, .in2 = 27 };
const MotorPinout backLeft   = { .en = 26, .in1 = 16, .in2 = 17 };
const MotorPinout frontRight = { .en = 33, .in1 = 21, .in2 = 19 };
const MotorPinout backRight  = { .en = 32, .in1 = 23, .in2 = 22 };
const MotorPinout motors[] = { frontLeft, backLeft, frontRight, backRight };

BluetoothSerial SerialBT;

struct Controller {
	bool useWifi;
	float xAxis;
	float yAxis;
	uint8_t button;
} controller;

void setup() {
	Serial.begin(115200);
	SerialBT.begin(BT_MOTOR_NAME, true);

	for (uint8_t i = 0; i < 4; i++) {
		pinMode(motors[i].en, OUTPUT);
		pinMode(motors[i].in1, OUTPUT);
		pinMode(motors[i].in2, OUTPUT);
		analogWrite(motors[i].en, 255);
		digitalWrite(motors[i].in1, LOW);
		digitalWrite(motors[i].in2, HIGH);
	}

	controller = (struct Controller){ .useWifi = false };
}

void readRadioInput() {
	while (SerialBT.available()) {
		uint8_t syncByte = SerialBT.read();
		if (syncByte != RADIO_SYNC_BYTE) { continue; }
		uint8_t typeByte = SerialBT.read();
		if (typeByte != 'x' && typeByte != 'y' && typeByte != 'b') { continue; }

		uint8_t msb = SerialBT.read();
		uint8_t lsb = SerialBT.read();
		uint16_t input = (msb << 8) | lsb;
		if (typeByte == 'x') { controller.xAxis = 2.0 * input / JOYSTICK_RESOLUTION - 1.0; }
		if (typeByte == 'y') { controller.yAxis = 2.0 * input / JOYSTICK_RESOLUTION - 1.0; }
		if (typeByte == 'b') { controller.button = input; }
	}
	Serial.printf("X:%f Y:%f B:%i\n", controller.xAxis, controller.yAxis, controller.button);
}

void loop() {
	if (controller.useWifi) {
		//
	} else {
		if (!SerialBT.connected()) {
			Serial.printf("trying to connect to remote...\n");
			if (!SerialBT.connect(BT_REMOTE_NAME)) { Serial.printf("failed to connect to remote, trying again...\n"); }
		} else { readRadioInput(); }
	}
	
	for (int8_t i = 0; i < 4; i++) { analogWrite(motors[i].en, controller.button ? 255 : 0); }
	delay(10);
}

// only used for debugging purposes
void searchBluetoothDevices() {
	bool foundRemote = false;
	while (!foundRemote) {
		Serial.printf("Starting device search...\n");
		BTScanResults * devices = SerialBT.discover(BT_DISCOVER_TIME);
		if (devices) {
			devices->dump(&Serial);
			for (int16_t i = 0; i < devices->getCount(); i++) {
				if (strcmp(devices->getDevice(i)->getName().c_str(), BT_REMOTE_NAME) == 0) {
					foundRemote = true;
					break;
				}
			}
			if (!foundRemote) {
				Serial.printf("unable to find remote, searching again...\n");
			} else {
				Serial.printf("found remote, ready to receive inputs\n");
			}
		} else {
			Serial.printf("error while trying to scan for devices, restarting Arduino\n");
			ESP.restart();
		}
	}
}
