#include "web_controls.h"

#define RESPONSE_TIMEOUT 6000
#define IN_BUFFER_SIZE 500
#define OUT_BUFFER_SIZE 500
static char request_buffer[IN_BUFFER_SIZE] = {0};
static char response_buffer[OUT_BUFFER_SIZE] = {0};

#define WIFI_NETWORK  "EECS_Labs" // change to "MIT" if outside lab
#define WIFI_PASSWORD ""

#define LED_PIN 2

void setupWifi() {
	WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
}

bool getWebControls(WebControls * controls) {
  if (!WiFi.isConnected()) {
    Serial.printf("trying to connect to wifi...\n");
    digitalWrite(LED_PIN, LOW);
    delay(250);
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    return false;
  }
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/team24/GROVER/server/chassis/server.py HTTP/1.1\r\nHost: 608dev-2.net\r\n\r\n");
  send_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  for (int i = 0; i < 4; i++) {
    if (response_buffer[i] != '0' && response_buffer[i] != '1') { return false; }
  }
  controls->up = response_buffer[0] - '0';
  controls->down = response_buffer[1] - '0';
  controls->left = response_buffer[2] - '0';
  controls->right = response_buffer[3] - '0';
  return true;
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
