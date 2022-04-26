#include "web_controls.h"

uint32_t prevGETControlsTime = 0;
char request_buffer[IN_BUFFER_SIZE] = {0};
char response_buffer[OUT_BUFFER_SIZE] = {0};

struct WebControls_s {
  uint8_t up;
  uint8_t down;
  uint8_t left;
  uint8_t right;
} WebControls = { .up = 0, .down = 0, .left = 0, .right = 0 };

void getWebControls() {
  if(millis() - prevGETWebControlsTime > GET_WEB_CONTROLS_INTERVAL) {
    sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/team24/chassis/server.py?controls HTTP/1.1\r\nHost: 608dev-2.net\r\n\r\n");
    send_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
    WebControls.up = response_buffer[0] - '0';
    WebControls.down = response_buffer[1] - '0';
    WebControls.left = response_buffer[2] - '0';
    WebControls.right = response_buffer[3] - '0';
    prevGETWebControlsTime = millis();
  }
}

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void send_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client2;
  if (client2.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client2.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client2.connected()) { //while we remain connected read out data coming back
      client2.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client2.available()) { //read out remaining text (body of response)
      char_append(response, client2.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client2.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client2.stop();
  }
}
