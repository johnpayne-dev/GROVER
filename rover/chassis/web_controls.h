#ifndef WEB_CONTROLS_HEADER_GUARD
#define WEB_CONTROLS_HEADER_GUARD

#include <Arduino.h>
#include <WiFi.h>

#define GET_WEB_CONTROLS_INTERVAL 100
extern uint32_t prevGETWebControlsTime;

#define IN_BUFFER_SIZE 500
#define OUT_BUFFER_SIZE 500
extern char request_buffer[IN_BUFFER_SIZE];
extern char response_buffer[OUT_BUFFER_SIZE];
#define RESPONSE_TIMEOUT 6000

extern struct WebControls_s WebControls;

uint8_t char_append(char* buff, char c, uint16_t buff_size);
void send_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial);
void getWebControls();

#endif
