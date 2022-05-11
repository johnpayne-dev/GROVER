#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <Wire.h>
#include <stdio.h>
#include <Adafruit_BMP280.h>
#include <mpu6050_esp32.h>
#include<math.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
// Initialize sensor that uses digital pins 13 and 12.
int timer2 = 0;
int vel_timer;
float v_totx = 0;
float v_toty = 0;
Adafruit_BMP280 bmp; // I2C
MPU6050 imu; //imu object called, appropriately, imu
char network[] = "EECS_Labs";
char password[] = "";
const float ZOOM = 9.81; //for display (converts readings into m/s^2)...used for visualizing only
const uint8_t LOOP_PERIOD = 10; //milliseconds
float x, y, z; //variables for grabbing x,y,and z values
float sample_rate = 2000; //Hz
float sample_period = (int)(1e6 / sample_rate);
double lat = 42.359034;
double lng=-71.094855;
const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 5000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 5000; //size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 3000;
char request_buffer[IN_BUFFER_SIZE]; 
char response_buffer[OUT_BUFFER_SIZE];
float pressure_reading;
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char json_body[JSON_BODY_SIZE];
const char city[]="cambridge";
const char state[]="MA";
int button_state=1;
float pres = 1031.2;
int old_button_state=1;
unsigned long timer=0;
double latitude;
double longitude;
float pressure=1013.25;
const char WEATHER_API_KEY[] = "R34HEDTHUKRM5FGVPXA6CGVRJ&";  // paste your key here
const uint8_t BUTTON = 45;
const int MAX_APS = 5;
WiFiClientSecure client; //global WiFiClient Secure object use this one for localization
WiFiClientSecure client2; //global WiFiClient Secure object use this one for weather api
const char NETWORK[] = "MIT GUEST";
const char PASSWORD[] = "";

char*  SERVER = "googleapis.com";  // Server URL
const int useLocation=0;

char ap_data[3000] = {0};
const char PREFIX[] = "{\"wifiAccessPoints\": ["; //beginning of json body
const char SUFFIX[] = "]}"; //suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; //don't change this and don't share this
int calibrated=0;

uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.
const char* google_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n" \
                      "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n" \
                      "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n" \
                      "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n" \
                      "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n" \
                      "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n" \
                      "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n" \
                      "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n" \
                      "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n" \
                      "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n" \
                      "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n" \
                      "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n" \
                      "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n" \
                      "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n" \
                      "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n" \
                      "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n" \
                      "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n" \
                      "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n" \
                      "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n" \
                      "-----END CERTIFICATE-----\n";

const char* Weather_CERT=\
                      "-----BEGIN CERTIFICATE-----\n"\
                      "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
                      "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
                      "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
                      "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
                      "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
                      "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
                      "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
                      "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
                      "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
                      "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
                      "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
                      "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
                      "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
                      "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
                      "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
                      "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
                      "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
                      "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
                      "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
                      "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
                      "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
                      "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
                      "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
                      "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
                      "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
                      "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
                      "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
                      "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
                      "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
                      "-----END CERTIFICATE-----\n";




int wifi_object_builder(char* object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t* mac_address) {
  char temp[300];//300 likely long enough for one wifi entry
  int len = sprintf(temp, "{\"macAddress\": \"%x:%x:%x:%x:%x:%x\",\"signalStrength\": %d,\"age\": 0,\"channel\": %d}",
                    mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5],
                    signal_strength, channel);
  if (len > os_len) {
    return 0;
  } else {
    return sprintf(object_string, "%s", temp);
  }
}




uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}
void do_https_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial, const char* cert) {
  client.setHandshakeTimeout(30);
  client.setCACert(cert); //set cert for https
  if (client.connect(host,443,4000)) { //try to connect to host on port 443
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    delay(10);
    Serial.println("there");
    client.print(request);
    response[0] = '\0';
    //memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      Serial.print("line 92");
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    Serial.println("line 102");
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

void setup() {
  Serial.begin(115200); //begin serial comms
  while (!Serial);
  unsigned status;
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(), 16);
    esp_restart();
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  Serial.println("bmp280 setup done");
  analogReadResolution(12); 
   if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);


  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
   char json_body[JSON_BODY_SIZE];
  int offset = sprintf(json_body, "%s", PREFIX);
  n = WiFi.scanNetworks(); //run a new scan. could also modify to use original scan from setup so quicker (though older info)
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } 
  else {// networks found
    int max_aps = max(min(MAX_APS, n), 1);
    for (int i = 0; i < max_aps; ++i) { //for each valid access point
      uint8_t* mac = WiFi.BSSID(i); //get the MAC Address
      offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE - offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); //generate the query
      if (i != max_aps - 1) {
        offset += sprintf(json_body + offset, ","); //add comma between entries except trailing.
      }
    }
    sprintf(json_body + offset, "%s", SUFFIX);
    Serial.println(json_body);
    int len = strlen(json_body);
    // Make a HTTP request:
    Serial.println("SENDING REQUEST");
    request[0] = '\0'; //set 0th byte to null
    offset = 0; //reset offset variable for sprintf-ing
    offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
    offset += sprintf(request + offset, "Host: googleapis.com\r\n");
    offset += sprintf(request + offset, "Content-Type: application/json\r\n");
    offset += sprintf(request + offset, "cache-control: no-cache\r\n");
    offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
    offset += sprintf(request + offset, "%s\r\n", json_body);
    do_https_request(SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false, google_CERT);
    Serial.println("-----------");
    Serial.println(response);
    Serial.println("-----------");
    
    DynamicJsonDocument doc(5000);
    char* starting1 = strchr(response, '{');
    char* ending1 = strrchr(response, '}');
    *(ending1 + 1) = NULL;
    DeserializationError error = deserializeJson(doc, starting1);
    // Test if parsing succeeds.
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
    } else {
       lat = doc["location"]["lat"];
       lng = doc["location"]["lng"];
       Serial.println("lat");
       Serial.println(lat);
    
   // need to not hard code the day  
   response[0] = 0;
    Serial.println("here");
    request[0] = '\0'; //set 0th byte to null
      offset = 0; //reset offset variable for sprintf-ing
      offset += sprintf(request + offset, 
    "GET https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/%lf,%lf/?unitGroup=us&elements=pressure&include=current&key=R34HEDTHUKRM5FGVPXA6CGVRJ&contentType=json HTTP/1.1\r\n",lat,lng);
      offset += sprintf(request + offset, "Host: weather.visualcrossing.com\r\n\r\n");
    do_https_request("weather.visualcrossing.com", request,response,OUT_BUFFER_SIZE,RESPONSE_TIMEOUT, true, Weather_CERT);
     
      DynamicJsonDocument doc2(5000);
      char* starting2 = strchr(response, '{');
      char* ending2 = strrchr(response, '}');
      *(ending2 + 1) = NULL;
      error = deserializeJson(doc2, starting2);
      // Test if parsing succeeds.
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
      } else {
         pres = doc2["currentConditions"]["pressure"];
         Serial.println("done");
         Serial.println(pres);
      }     
}
      response[0] = 0;
      vel_timer = millis();
      sprintf(request, "POST http://608dev-2.net/sandbox/sc/team24/GROVER/server/sensors/loc_requests.py?lat=%lf&lng=%lf HTTP/1.1\r\n",lat,lng);
      strcat(request, "Host: 608dev-2.net\r\n");
      strcat(request, "\r\n"); //header;
      do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);}
      timer2 = millis();

}

void loop() {  
    Serial.println("online pressure reading");
    Serial.println(pres);
    float alt = bmp.readAltitude(pres);
    Serial.println("alt");
    Serial.println(alt);
    float temperature; //variable for temperature
    float pressure;    //variable for pressure
    float altitude; 
    uint16_t raw_reading = analogRead(A0);
    Serial.println("Audio readings");
    Serial.println(raw_reading);
    Serial.println("Temperature readings");
    temperature = bmp.readTemperature();
    Serial.println(temperature);
    pressure = bmp.readPressure();
    Serial.println("pressure");
    Serial.println(pressure);
    float f = pressure/100.0;
    Serial.println("f");
    Serial.println(f);
    imu.readAccelData(imu.accelCount);
    imu.readGyroData(imu.gyroCount);
    float a = imu.gyroCount[0] * imu.gRes;
    float b = imu.gyroCount[1] * imu.gRes;
    Serial.println("gyro readings");
    Serial.println(a);
    Serial.println(b);
    x = ZOOM * imu.accelCount[0] * imu.aRes;
    y = ZOOM * imu.accelCount[1] * imu.aRes;
    float dt = millis() - vel_timer;
    dt =  dt/1000.0;
    Serial.println(dt);
    if ( abs(x) > .75){
      v_totx = dt*x +v_totx;}
    if ( abs(y) > .75){
    v_toty = dt*y + v_toty;}
    Serial.println("velocitys");
    Serial.println(v_totx);
    Serial.println(v_toty);
    vel_timer = millis();
    Serial.println("accel readings");
    Serial.println(x);
    Serial.println(y);
    //Serial.println(temperature);
    Serial.println("done");
    sprintf(request, "POST http://608dev-2.net/sandbox/sc/team24/GROVER/server/sensors/s_request.py?temp=%f&press=%f&accelx=%f&accely=%f&sound=%f&a=%f&b=%f&dis=0&alt=%f&vx=%f&vy=%f HTTP/1.1\r\n",temperature, pressure,x,y,raw_reading,a,b,alt,v_totx,v_toty);
    strcat(request, "Host: 608dev-2.net\r\n");
    strcat(request, "\r\n"); //header;
    do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
    Serial.println(response);
    Serial.println("&&&&&&&&&&");
    if (millis() -  timer2 > 30000){
      timer2 = millis();
      char json_body[JSON_BODY_SIZE];
      int offset = sprintf(json_body, "%s", PREFIX);
      int n = WiFi.scanNetworks(); //run a new scan. could also modify to use original scan from setup so quicker (though older info)
      Serial.println("scan done");
      if (n == 0) {
        Serial.println("no networks found");
      } 
      else {// networks found
        int max_aps = max(min(MAX_APS, n), 1);
        for (int i = 0; i < max_aps; ++i) { //for each valid access point
          uint8_t* mac = WiFi.BSSID(i); //get the MAC Address
          offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE - offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); //generate the query
          if (i != max_aps - 1) {
            offset += sprintf(json_body + offset, ","); //add comma between entries except trailing.
          }
        }
        sprintf(json_body + offset, "%s", SUFFIX);
        Serial.println(json_body);
        int len = strlen(json_body);
        // Make a HTTP request:
        Serial.println("SENDING REQUEST");
        request[0] = '\0'; //set 0th byte to null
        offset = 0; //reset offset variable for sprintf-ing
        offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
        offset += sprintf(request + offset, "Host: googleapis.com\r\n");
        offset += sprintf(request + offset, "Content-Type: application/json\r\n");
        offset += sprintf(request + offset, "cache-control: no-cache\r\n");
        offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
        offset += sprintf(request + offset, "%s\r\n", json_body);
        do_https_request(SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false, google_CERT);
        Serial.println("-----------");
        Serial.println(response);
        Serial.println("-----------");
        
        DynamicJsonDocument doc(5000);
        char* starting1 = strchr(response, '{');
        char* ending1 = strrchr(response, '}');
        *(ending1 + 1) = NULL;
        DeserializationError error = deserializeJson(doc, starting1);
        // Test if parsing succeeds.
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.f_str());
        } else {
           lat = doc["location"]["lat"];
           lng = doc["location"]["lng"];
           Serial.println("lat");
           Serial.println(lat);
           sprintf(request, "POST http://608dev-2.net/sandbox/sc/team24/GROVER/server/sensors/loc_requests.py?lat=%lf&lng=%lf HTTP/1.1\r\n",lat,lng);
           strcat(request, "Host: 608dev-2.net\r\n");
           strcat(request, "\r\n"); //header;
           do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
        }
    
}}}
