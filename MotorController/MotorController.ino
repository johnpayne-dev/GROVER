#include <BluetoothSerial.h>

#define BT_DISCOVER_TIME 5000
#define BT_REMOTE_NAME   "GROVER-Remote"
#define BT_MOTOR_NAME    "GROVER-Motor"

BluetoothSerial SerialBT;

void setup() {
	Serial.begin(115200);
	SerialBT.begin(BT_MOTOR_NAME, true);

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
	
	if (!SerialBT.connect(BT_REMOTE_NAME)) {
		Serial.printf("failed to connect to remote, restarting Arduino\n");
		ESP.restart();
	}
}

void loop() {
	while (SerialBT.available()) { Serial.write(SerialBT.read()); }
	delay(100);
}
