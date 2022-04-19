#include <BluetoothSerial.h>

#define BT_REMOTE_NAME   "GROVER-Remote"
#define BT_MOTOR_NAME    "GROVER-Motor"
#define BT_DISCOVER_TIME 5000

#define JOYSTICK_RESOLUTION (1 << 12)
#define RADIO_SYNC_BYTE     0x45

BluetoothSerial SerialBT;

struct Controller
{
	bool useWifi;
	float xAxis;
	float yAxis;
	uint8_t button;
} controller;

// only used for debugging purposes
void searchDevices() {
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

void setup() {
	Serial.begin(115200);
	SerialBT.begin(BT_MOTOR_NAME, true);

	controller = (struct Controller){ .useWifi = false };
	if (!SerialBT.connect(BT_REMOTE_NAME)) {
		Serial.printf("failed to connect to remote, trying again...\n");
		ESP.restart();
	}
	
}

void loop() {
	while (SerialBT.available()) {
		uint8_t syncByte = SerialBT.read();
		if (syncByte != RADIO_SYNC_BYTE) { continue; }
		uint8_t typeByte = SerialBT.read();
		if (typeByte != 'x' && typeByte != 'y' && typeByte != 'b') { continue; }

		uint8_t msb = SerialBT.read();
		uint8_t lsb = SerialBT.read();
		uint16_t value = (msb << 8) | lsb;
		if (typeByte == 'x') { controller.xAxis = 2.0 * value / JOYSTICK_RESOLUTION - 1.0; }
		if (typeByte == 'y') { controller.yAxis = 2.0 * value / JOYSTICK_RESOLUTION - 1.0; }
		if (typeByte == 'b') { controller.button = value; }
	}
	Serial.printf("X:%f Y:%f B:%i\n", controller.xAxis, controller.yAxis, controller.button);
}
