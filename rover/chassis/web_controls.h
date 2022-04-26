#ifndef WEB_CONTROLS_HEADER_GUARD
#define WEB_CONTROLS_HEADER_GUARD

#include <Arduino.h>
#include <WiFi.h>

typedef struct {
  uint8_t up;
  uint8_t down;
  uint8_t left;
  uint8_t right;
} WebControls;

void setupWifi();
uint8_t char_append(char* buff, char c, uint16_t buff_size);
void send_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial);
bool getWebControls(WebControls * controls);

#endif
