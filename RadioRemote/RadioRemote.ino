#include <BluetoothSerial.h>

#define BT_REMOTE_NAME   "GROVER-Remote"

BluetoothSerial SerialBT;

void setup() {
	Serial.begin(115200);
	SerialBT.begin(BT_REMOTE_NAME);
}

void loop() {
	SerialBT.write('b');
	Serial.printf("sent b\n");
	delay(100);
}
