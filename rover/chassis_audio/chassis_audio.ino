#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
//WiFiClientSecure is a big library. It can take a bit of time to do that first compile

TFT_eSPI tft = TFT_eSPI();

// const int DELAY = 1000;
const int SAMPLE_FREQ = 8000;                          // Hz, telephone sample rate
const int SAMPLE_DURATION = 5;       //WAS ORIGINALLY 5                 // duration of fixed sampling (seconds)
const int NUM_SAMPLES = SAMPLE_FREQ * SAMPLE_DURATION;  // number of samples
const int ENC_LEN = (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4;  // Encoded length of clip

float time_between_samples = 1000000 / SAMPLE_FREQ; //micros

const uint16_t RESPONSE_TIMEOUT = 6000;
// const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
const uint16_t OUT_BUFFER_SIZE = 50000; //size of buffer to hold HTTP response



char* response; //char array buffer to hold HTTP response
// char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

/* CONSTANTS */
//Prefix to POST request:
const char PREFIX[] = "{\"encoding\": \"";
const char SUFFIX[] = "\"}"; //suffix to POST request
const int AUDIO_IN = 1; //pin where microphone is connected

const uint8_t PIN_1 = 45; //button to toggle listening mode

/* Global variables*/
uint32_t time_since_sample;      // used for microsecond timing

// const char audio_file[100] = {111,234,2,}
// char* speech_data;
char speech_data[ENC_LEN + 200] = {0}; //global used for collecting speech data
const char* NETWORK     = "EECS_Labs";     // your network SSID (name of wifi network)
// const char* NETWORK     = "MIT";     // your network SSID (name of wifi network)
const char* PASSWORD = ""; // your network password
// const char*  SERVER = "speech.google.com";  // Server URL
const char*  SERVER = "608dev-2.net";  // Server URL

// WiFiClientSecure client; //global WiFiClient Secure object
WiFiClient client; //global WiFiClient Secure object

bool listening = false;

bool blackout = true;

enum press {DOWN, UP};
press state_45 = UP;

bool quit = false;

const uint8_t BUTTON_45 = 45;

int old = 0;
int older = 0;

bool loud = false;

uint32_t timer = millis();
uint32_t loud_timer = millis();
uint32_t check_timer = millis();

int threshold = 3000;

// uint8_t AUDIO_TRANSDUCER = 18; //DAC speaker
uint8_t AUDIO_TRANSDUCER = 17; //DAC speaker
// uint8_t AUDIO_TRANSDUCER = 37; //speaker
// uint8_t AUDIO_TRANSDUCER = 14; //buzzer
uint8_t AUDIO_PWM = 1;

void setup() {
  Serial.begin(115200);               // Set up serial port
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  analogReadResolution(12);       // initialize the analog resolution

  delay(100); //wait a bit (100 ms)
  pinMode(PIN_1, INPUT_PULLUP);

  WiFi.begin(NETWORK, PASSWORD); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  check_timer = millis();


  pinMode(AUDIO_TRANSDUCER, OUTPUT);
   //set up AUDIO_PWM which we will control in this lab for music:
  ledcSetup(AUDIO_PWM, 200, 12);//12 bits of PWM precision
  ledcWrite(AUDIO_PWM, 0); //0 is a 0% duty cycle for the NFET
  ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);

  response = (char*)ps_malloc(200000); //FREE IS NEVER CALLED?!!???!!
  // speech_data = (char*)ps_malloc(ENC_LEN + 200); //FREE IS NEVER CALLED?!!???!!
  // speech_data = {0};

  Serial.println("PSRAM");
  Serial.print("Heap Size: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("PSRAM Size: ");
  Serial.println(ESP.getPsramSize());
  Serial.print("Free PSRAM: ");
  Serial.println(ESP.getFreePsram());  
}

//main body of code
void loop() {
    if (millis() - check_timer > 3000) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0,0,1);
        tft.println("Checking for new controller audio..."); //UNCOMMENT LATER
        // tft.println("Checking for new rover audio...");
        blackout = true;

        Serial.print("\nStarting connection to server...");
        delay(300);
        bool conn = false;
        for (int i = 0; i < 10; i++) {
            int val = (int)client.connect(SERVER, 80);
            Serial.print(i); Serial.print(": "); Serial.println(val);
            if (val != 0) {
                conn = true;
                break;
            }
            Serial.print(".");
            delay(300);
        }
        if (!conn) {
            Serial.println("Connection failed!");
            return;
        } else {
            Serial.println("Connected to server!");
            Serial.println(client.connected());

            client.print("POST /sandbox/sc/team24/GROVER/server/audio/from_controller.py?new"); client.print(" HTTP/1.1\r\n"); //UNOMMMENT LATER
            // client.print("POST /sandbox/sc/team24/GROVER/server/audio/from_rover.py?new"); client.print(" HTTP/1.1\r\n");
            client.print("Host: 608dev-2.net\r\n");
            client.print("Content-Type: application/json\r\n");
            client.print("Content-Length: "); client.print(2);
            client.print("\r\n\r\n");
            client.print("{}");
            client.print("\r\n");
            //Serial.print("\r\n\r\n");
            Serial.println("Through send...");


            // start comment block for speed



            unsigned long count = millis();
            while (client.connected()) {
                Serial.println("IN!");
                String line = client.readStringUntil('\n');
                Serial.print(line);
                if (line == "\r") { //got header of response
                    Serial.println("headers received");
                    break;
                }
            if (millis() - count > RESPONSE_TIMEOUT) break;
            }
            Serial.println("");
            Serial.println("Response...");
            count = millis();
            while (!client.available()) {
                delay(100);
                Serial.print(".");
                if (millis() - count > RESPONSE_TIMEOUT) break;
            }
            Serial.println();
            Serial.println("-----------");
            memset(response, 0, sizeof(response));


            //end comment block




            while (client.available()) {
                char_append(response, client.read(), OUT_BUFFER_SIZE);
            }
            Serial.println(response); //comment this out if needed for debugging
            char* trans_id = strstr(response, "transcript");
            if (trans_id != NULL) {
                char* foll_coll = strstr(trans_id, ":");
                char* starto = foll_coll + 2; //starting index
                char* endo = strstr(starto + 1, "\""); //ending index
                int transcript_len = endo - starto + 1;
                char transcript[100] = {0};
                strncat(transcript, starto, transcript_len);
                Serial.println(transcript);
            }
            Serial.println("-----------");
            Serial.println();
            client.stop();
            Serial.println("done");
        }
        
        if (response[0] != '!') {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,0,1);
            tft.println("New audio from controller!");
            
            int rl = strlen(response);
            // Serial.println("decoded:");
            // Serial.println(response);
            base64_decode(response, response, rl);
            
            uint32_t now = millis();
            for (int i = 0; i < rl * 3 / 4; i++) {
                dacWrite(AUDIO_TRANSDUCER, response[i]);
                while (micros() - now < time_between_samples);
                now = micros();
            }
            dacWrite(AUDIO_TRANSDUCER, 0);

            //reset audio after playing sound
            old = 0;
            older = 0;

            while (micros() - now < 1000000);
        }
        check_timer = millis();
    }
    
    int input_45 = digitalRead(BUTTON_45);
    switch(state_45) { //toggle listen
        case UP:
            if (! input_45) { //pressed
                state_45 = DOWN;
            }
            break;
        case DOWN:
            if (input_45) { //released
                state_45 = UP;
                listening = !(listening);
                blackout = true;
            }
            break;
    }

    if (! listening) {
        if (blackout) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,0,1);
            tft.println("Not listening for sound.");
            blackout = false;
        }
    }
    else {
        if (blackout) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,0,1);
            tft.println("Listening for sound.");
            blackout = false;
        }
        int value = analogRead(AUDIO_IN);
        int avg = (value + old + older) / 3;
        older = old;
        old = value;

        if (value > threshold) {
            // Serial.println("avg:");
            // Serial.println(avg);
            // Serial.println("value:");
            // Serial.println(value);

            old = 0;
            older = 0;
            blackout = true;
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,0,1);
            tft.println("Recording Sound");

            Serial.println("recording...");
            record_audio();
            if (quit) {
                quit = false;
                listening = false;
                blackout = true;
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0,0,1);
                tft.println("Recording aborted");
                Serial.println("Recording aborted.");
                uint32_t now = millis();
                while (millis() - now < 1000); //wait for a second after aborting
                return;
            }

            Serial.print("\nStarting connection to server...");
            delay(300);
            bool conn = false;
            for (int i = 0; i < 10; i++) {
                int val = (int)client.connect(SERVER, 80);
                Serial.print(i); Serial.print(": "); Serial.println(val);
                if (val != 0) {
                    conn = true;
                    break;
                }
                Serial.print(".");
                delay(300);
            }
            if (!conn) {
                Serial.println("Connection failed!");
                return;
            } else {
                Serial.println("Connected to server!");
                Serial.println(client.connected());

                int len = strlen(speech_data);

                client.print("POST /sandbox/sc/team24/GROVER/server/audio/from_rover.py"); client.print(" HTTP/1.1\r\n");
                client.print("Host: 608dev-2.net\r\n");
                client.print("Content-Type: application/json\r\n");
                client.print("Content-Length: "); client.print(len);
                client.print("\r\n\r\n");

                int ind = 0;
                int jump_size = 1000;
                char temp_holder[jump_size + 10] = {0};
                Serial.println("sending data");
                while (ind < len) {
                    delay(80);//experiment with this number!
                    //if (ind + jump_size < len) client.print(speech_data.substring(ind, ind + jump_size));
                    strncat(temp_holder, speech_data + ind, jump_size);
                    client.print(temp_holder);
                    ind += jump_size;
                    memset(temp_holder, 0, sizeof(temp_holder));
                }
                client.print("\r\n");
                //Serial.print("\r\n\r\n");
                Serial.println("Through send...");
                unsigned long count = millis();

                //start comment block for speed
                while (client.connected()) {
                    Serial.println("IN!");
                    String line = client.readStringUntil('\n');
                    Serial.print(line);
                    if (line == "\r") { //got header of response
                        Serial.println("headers received");
                        break;
                    }
                if (millis() - count > RESPONSE_TIMEOUT) break;
                }
                Serial.println("");
                Serial.println("Response...");
                count = millis();
                while (!client.available()) {
                    delay(100);
                    Serial.print(".");
                    if (millis() - count > RESPONSE_TIMEOUT) break;
                }
                Serial.println();
                Serial.println("-----------");
                memset(response, 0, sizeof(response));
                while (client.available()) {
                    char_append(response, client.read(), OUT_BUFFER_SIZE);
                }
                Serial.println(response); //comment this out if needed for debugging
                char* trans_id = strstr(response, "transcript");
                if (trans_id != NULL) {
                    char* foll_coll = strstr(trans_id, ":");
                    char* starto = foll_coll + 2; //starting index
                    char* endo = strstr(starto + 1, "\""); //ending index
                    int transcript_len = endo - starto + 1;
                    char transcript[100] = {0};
                    strncat(transcript, starto, transcript_len);
                    Serial.println(transcript);
                }
                Serial.println("-----------");
                client.stop();
                Serial.println("done");

                //end comment block
            }
        }
    }
}

//function used to record audio at sample rate for a fixed number of samples
void record_audio() {
  quit = false;
  int sample_num = 0;    // counter for samples
  int enc_index = strlen(PREFIX) - 1;  // index counter for encoded samples
  int value = 0;
  char raw_samples[3];   // 8-bit raw sample data array
  memset(speech_data, 0, sizeof(speech_data));
  sprintf(speech_data, "%s", PREFIX);
  char holder[5] = {0};

  uint32_t text_index = enc_index;
  uint32_t start = millis();
  time_since_sample = micros();
  loud_timer = millis();
  while (millis() - loud_timer < 1000 && sample_num < NUM_SAMPLES) { //read in NUM_SAMPLES worth of audio data
    if (!digitalRead(BUTTON_45)) { //quits recording if listening mode is toggled mid audio
        quit = true;
        listening = false;
        
        break;
    }
    value = analogRead(AUDIO_IN);  //make measurement

    if (value > threshold) {
        loud_timer = millis();
    }
    int8_t mulaw = mulaw_encode(value - 1800);

    raw_samples[sample_num % 3] = mulaw; //remove 1.5ishV offset (from 12 bit reading)
    sample_num++;
    if (sample_num % 3 == 0) {
      base64_encode(holder, raw_samples, 3);
      strncat(speech_data + text_index, holder, 4);
      text_index += 4;
    }

    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
  }
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,1);
  tft.println("Transmitting audio.");
  
  if (sample_num == NUM_SAMPLES) {
      Serial.println("Max length recording reached.");
  }

//   Serial.println(millis() - start);
  sprintf(speech_data + strlen(speech_data), "%s", SUFFIX);
  Serial.println("out");
  Serial.println(speech_data);
}


int8_t mulaw_encode(int16_t sample) {
  const uint16_t MULAW_MAX = 0x1FFF;
  const uint16_t MULAW_BIAS = 33;
  uint16_t mask = 0x1000;
  uint8_t sign = 0;
  uint8_t position = 12;
  uint8_t lsb = 0;
  if (sample < 0)
  {
    sample = -sample;
    sign = 0x80;
  }
  sample += MULAW_BIAS;
  if (sample > MULAW_MAX)
  {
    sample = MULAW_MAX;
  }
  for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
      ;
  lsb = (sample >> (position - 4)) & 0x0f;
  return (~(sign | ((position - 5) << 4) | lsb));
}

