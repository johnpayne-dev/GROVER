// This program requires the ArduCAM V4.0.0 (or later) library and ArduCAM ESP32 2MP/5MP camera
// and use Arduino IDE 1.8.1 compiler or above
// BASE64
#include <WiFi.h>
#include <Wire.h>
#include <ESP32WebServer.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

#if !(defined ESP32 )
#error Please select the ArduCAM ESP32 UNO board in the Tools/Board
#endif
//This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined (OV2640_MINI_2MP)||defined (OV5640_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP_PLUS) \
    || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) \
    ||(defined (ARDUCAM_SHIELD_V2) && (defined (OV2640_CAM) || defined (OV5640_CAM) || defined (OV5642_CAM))))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

// set GPIO17 as the slave select :
const int CS = 34;
const int CAM_POWER_ON = 10;
#if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
  ArduCAM myCAM(OV2640, CS);
#elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
  ArduCAM myCAM(OV5640, CS);
#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
  ArduCAM myCAM(OV5642, CS);
#endif

//you can change the value of wifiType to select Station or AP mode.
//Default is AP mode.
int wifiType = 0; // 0:Station  1:AP

//AP mode configuration
//Default is arducam_esp8266.If you want,you can change the AP_aaid  to your favorite name
const char *AP_ssid = "arducam_esp32"; 
//Default is no password.If you want to set password,put your password here
const char *AP_password = NULL;

uint8_t channel = 1; //network channel on 2.4 GHz
// byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.
byte bssid[] = {0x78, 0x4f, 0x43, 0x94, 0x13, 0x05}; //6 byte MAC address of AP you're targeting.
// 78:4f:43:94:13:05
//Station mode you should put your ssid and password
const char *ssid = "EECS_Labs"; // Put your SSID here
const char *password = ""; // Put your PASSWORD here

const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 41000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response
const uint16_t DATA_SIZE = 40300;
char data[DATA_SIZE] = "data=";
int offset = 0;
bool initd = false;
static const size_t bufferSize = 2048;
static uint8_t buffer[bufferSize] = {0xFF};

uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;
int loc = 0;
bool succ = true;
bool complete = false;
char value[15]; 
ESP32WebServer server(80);

void start_capture(){
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM){
  WiFiClient client = server.client();
  uint32_t len  = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE) //8M
  {
    Serial.println(F("Over size."));
  }
  if (len == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst(); 
  if (!client.connected()) return;
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-len: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  initd = false;
  loc = 0;
  i = 0;
  succ = false;
  while ( len-- )
  {

    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buffer[i++] = temp;  //save the last  0XD9     
      //Write the remain bytes in the buffer
      if (!client.connected()) break;
      client.write(&buffer[0], i);
      is_header = false;
      loc = i;
      succ = true;
      CheckValues();
      i = 0;
      myCAM.CS_HIGH();
      break; 
    }  
    if (is_header == true)
    { 
      //Write image data to buffer if not full
      if (i < bufferSize)
      buffer[i++] = temp;
      else
      {
        //Write bufferSize bytes image data to file
        if (!client.connected()) break;
        client.write(&buffer[0], bufferSize);
        loc = i;
        succ = true;
        CheckValues();
        i = 0;
        buffer[i++] = temp;
      }        
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buffer[i++] = temp_last;
      buffer[i++] = temp;   
    } 
  } 
}

void serverCapture(){
  delay(1000);
  start_capture();
  Serial.println(F("CAM Capturing"));

  int total_time = 0;

  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);

  total_time = 0;

  Serial.println(F("CAM Capture Done."));
  total_time = millis();
  camCapture(myCAM);
  // CheckValues();
  insert_answer();
  total_time = millis() - total_time;
  Serial.print(F("send total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  Serial.println(F("CAM send Done."));
}


void insert_answer(){
  response[0] = '\0';
  request[0] = '\0'; //set 0th byte to null
  // body[0] = '\0';
  offset = 0; //reset offset variable for sprintf-ing
  // http://608dev-2.net/sandbox/sc/your_kerberos/cat1/hey.py
  
  // Serial.println(data);
  int len = strlen(data);
  Serial.println(len);
  Serial.println("\n");
  Serial.println(data);
  // sprintf(body, "data=%s", data);
  // Serial.println(body);
  // int len = strlen(body);
  offset += sprintf(request + offset, "POST http://www.608dev-2.net/sandbox/sc/team24/GROVER/server/camera/server.py  HTTP/1.1\r\n");
  // offset += sprintf(request + offset, "POST http://www.608dev-2.net/sandbox/sc/aponce/grover_server.py  HTTP/1.1\r\n");
  offset += sprintf(request + offset, "Host: 608dev-2.net\r\n");
  offset += sprintf(request + offset, "Content-Type: application/x-www-form-urlencoded\r\n");
  offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
  offset += sprintf(request + offset, "%s\r\n", data);

  do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response);
}

void CheckValues(){
  if (succ){
    if (!initd){
      memset(data, 0, sizeof(data));
      strcat(data, "data=");
      offset = 5;
      initd = true;
    }
    
    // memset(data, 0, sizeof(data)); //write 0 (or '\0') to all bytes in char array!
    Serial.println(loc);
    for (int j = 0; j < loc; j++){
      // sprintf(value, "%c", buffer[j]);

      offset += sprintf(data + offset, "%02X", buffer[j]);
      // Serial.println(value);      
    }
    // Serial.println("This is the loc");
    // Serial.println(loc);
  }
  else{
    Serial.println("Failed capture");
  }
}

void serverStream(){
  start_capture();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  size_t len = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE) //8M
  {
    Serial.println(F("Over size."));
    return;
  }
  if (len == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
    return;
  } 
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  memset(data, 0, sizeof(data));
  strcat(data, "data=");
  i = 0;
  loc = 5;
  initd = false;
  succ = false;
  complete = false;
  is_header = false;
  char holder[5] = {0};
  char raw_samples[3];

  while ( len-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);

    //Read JPEG data from FIFO
    if (is_header){
      if (loc + 2 >= DATA_SIZE){
        Serial.println("Image too large");
      }
      // i+= sprintf(data + i, "%02X", temp);
      raw_samples[i % 3] = temp;
      i++;
      if (i % 3 == 0){
        base64_encode(holder, raw_samples, 3);
        loc += sprintf(data + loc, "%s", holder);
        i = 0;
      }
      if ((temp == 0xD9) && (temp_last == 0xFF)) //If find the end ,break while,
      {
        complete = true;
        break;
      }        
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      // i+= sprintf(data + i, "%02X", temp_last);
      // i+= sprintf(data + i, "%02X", temp);
      raw_samples[0] = temp_last;
      raw_samples[1] = temp;
      i = 2;
    } 
  }
  if (i!=0){
    base64_encode(holder, raw_samples, i);
    loc += sprintf(data + loc, "%s", holder);
  }
  myCAM.CS_HIGH(); 
  if (!complete) {
    Serial.println("incomplete image, returning");
    return;
  }
  insert_answer();
}

void setup() {
  uint8_t vid, pid;
  uint8_t temp;
  //set the CS as an output:
  pinMode(CS,OUTPUT);
  pinMode(CAM_POWER_ON , OUTPUT);
  digitalWrite(CAM_POWER_ON, HIGH);
  #if defined(__SAM3X8E__)
  Wire1.begin();
  #else
  Wire.begin();
  #endif
  Serial.begin(115200);
  Serial.println(F("ArduCAM Start!"));

  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000); //4MHz

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println(F("SPI1 interface Error!"));
    while(1);
  }

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println(F("SPI1 interface Error!"));
    while(1);
  }
  #if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
  //Check if the camera module type is OV2640
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
  Serial.println(F("Can't find OV2640 module!"));
  else
  Serial.println(F("OV2640 detected."));
  #elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
  //Check if the camera module type is OV5640
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5640_CHIPID_LOW, &pid);
  if((vid != 0x56) || (pid != 0x40))
  Serial.println(F("Can't find OV5640 module!"));
  else
  Serial.println(F("OV5640 detected."));
  #elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
  //Check if the camera module type is OV5642
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  if((vid != 0x56) || (pid != 0x42)){
    Serial.println(F("Can't find OV5642 module!"));
  }
  else
  Serial.println(F("OV5642 detected."));
  #endif

  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  #if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
  myCAM.OV2640_set_JPEG_size(OV2640_160x120);
  #elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5640_320x240);
  #elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5642_320x240);  
  #endif

  myCAM.clear_fifo_flag();
  if (wifiType == 0){
    if(!strcmp(ssid,"SSID")){
      Serial.println(F("Please set your SSID"));
      while(1);
    }
    if(!strcmp(password,"PASSWORD")){
      Serial.println(F("Please set your PASSWORD"));
      while(1);
    }
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    // WiFi.begin(ssid, password, channel, bssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
    }
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(WiFi.localIP());
  }
  else if (wifiType == 1){
    Serial.println();
    Serial.println();
    Serial.print(F("Share AP: "));
    Serial.println(AP_ssid);
    Serial.print(F("The password is: "));
    Serial.println(AP_password);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    Serial.println("");
    Serial.println(WiFi.softAPIP());
  }

}

void loop() {
  serverStream();
  // delay(1000);
  // while(true);
}


