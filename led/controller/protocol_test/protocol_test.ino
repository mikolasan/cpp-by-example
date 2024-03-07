
#define FASTLED_ESP8266_D1_PIN_ORDER
#include <FastLED.h>

#define CHIPSET     WS2812
#define COLOR_ORDER GRB

#define LED_PIN_1     0
#define LED_PIN_2     1

#define NUM_STRIPS 1
#define NUM_LEDS_PER_STRIP 170
#define TEST_STRIP_LENGTH 40
CRGB leds[NUM_STRIPS][TEST_STRIP_LENGTH];
#define BRIGHTNESS  20

String start_seq = "Woop woop woop";
int start_seq_len = 0;
String tmp_start = "";
int message_length = 0;
#define MAX_MESSAGE_LENGTH 550
unsigned char message[MAX_MESSAGE_LENGTH];

int message_id;
int led_id;
int color_id;

uint16_t message_crc;

const char COMM_STATE_INIT = 0;
const char COMM_STATE_LENGTH_1 = 1;
const char COMM_STATE_LENGTH_2 = 2;
const char COMM_STATE_MESSAGE = 3;
const char COMM_STATE_CRC_1 = 4;
const char COMM_STATE_CRC_2 = 5;
char current_state = COMM_STATE_INIT;

uint16_t crc16(uint8_t* s, size_t len, uint16_t crcval = 0) {
  uint32_t c, q;
  for (; len; len--) {
    c = *s++;
    q = (crcval ^ c) & 017;
    crcval = (crcval >> 4) ^ (q * 010201);
    q = (crcval ^ (c >> 4)) & 017;
    crcval = (crcval >> 4) ^ (q * 010201);
  }
  return (crcval);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay( 2000 ); // power-up safety delay

  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds[0], TEST_STRIP_LENGTH);
//  FastLED.addLeds<CHIPSET, LED_PIN_2, COLOR_ORDER>(leds[1], NUM_LEDS_PER_STRIP);

  FastLED.setBrightness( BRIGHTNESS );

  start_seq_len = start_seq.length();
  tmp_start.reserve(start_seq_len);
  current_state = COMM_STATE_INIT;
  message_length = 0;

  for (int i = 0; i < TEST_STRIP_LENGTH; ++i) {
    leds[0][i] = CRGB::Black;
  }
}

void read_serial() {
  while (Serial.available() > 0) {
    // init
    if (current_state == COMM_STATE_INIT) {
      char s = (char)Serial.read();
      tmp_start += s;
      if (tmp_start.length() < start_seq_len) {
        continue;
      } else if (tmp_start.length() > start_seq_len) {
        // improbable to be here
        // but better to make a simple check instead of full string comparison
        Serial.println("improbable sliding");
        tmp_start.remove(0, 1);
      } else {
        if (start_seq.equals(tmp_start)) {
          current_state = COMM_STATE_LENGTH_1;
//          Serial.println("got init");
          tmp_start = "";
        } else {
//          tmp_start.remove(0, 1);
          tmp_start = "";
        }
      }
    // length
    } else if (current_state == COMM_STATE_LENGTH_1) {
      uint8_t length_high = (uint8_t)Serial.read();
      message_length = (uint16_t)(length_high << 8);
      current_state = COMM_STATE_LENGTH_2;
    } else if (current_state == COMM_STATE_LENGTH_2) {
      uint8_t length_low = (uint8_t)Serial.read();
      message_length |= (uint16_t)length_low;
      if (message_length > MAX_MESSAGE_LENGTH) {
        Serial.println("length error");
        current_state = COMM_STATE_INIT;
      } else {
//        Serial.println("got length");
        current_state = COMM_STATE_MESSAGE;
        
        message_id = 0;
        led_id = 0;
        color_id = 0;
      }
    // message
    } else if (current_state == COMM_STATE_MESSAGE) {
      unsigned char s = (unsigned char)Serial.read();
      
      if (led_id < TEST_STRIP_LENGTH) {
        int universe_id = 0;
//        if (color_id == 0) {
//          leds[universe_id][led_id][color_id] = 150;
//        }
        leds[universe_id][led_id][color_id] = s;
        color_id++;
        if (color_id >= 3) {
          led_id++;
          color_id = 0;
        }
      }
      //message_id++;
      message[message_id++] = s;
      if (message_id >= message_length) {
//        Serial.println("got message");
//        Serial.println(message_id, DEC);
        current_state = COMM_STATE_CRC_1;
      }
    // crc
    } else if (current_state == COMM_STATE_CRC_1) {
      uint8_t crc_high = (uint8_t)Serial.read();
      message_crc = (uint16_t)(crc_high << 8);
      current_state = COMM_STATE_CRC_2;
    } else if (current_state == COMM_STATE_CRC_2) {
      uint8_t crc_low = (uint8_t)Serial.read();
      message_crc |= (uint16_t)crc_low;
      tmp_start = "";
      current_state = COMM_STATE_INIT;


//      uint16_t check_crc = crc16(message, message_length);
//      if (check_crc == message_crc) {
//        Serial.println("correct CRC");
        
        break;

//      } else {
//        Serial.println("wrong CRC");
//        Serial.println(check_crc, HEX);
//      }
      
    } else {
      Serial.println("wrong state");
      current_state = COMM_STATE_INIT;
    }
  }
}

void loop() {
  read_serial();
  FastLED.show();
  //delay(100);
  Serial.println("ready");
}
