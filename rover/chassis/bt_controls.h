#ifndef BT_CONTROLS_H
#define BT_CONTROLS_H

typedef struct {
	float x;
	float y;
	uint8_t b;
} BTControls;

void setupRadio();
bool readRadioInput(BTControls * controls);
void searchBluetoothDevices();

#endif
