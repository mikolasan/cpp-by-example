
#include <iostream>
#include <string>

#include "show_led.hpp"

// #define FASTLED_INTERRUPT_RETRY_COUNT 1 // must be before FastLED
// #define FASTLED_ESP8266_D1_PIN_ORDER // must be before FastLED
 #include <FastLED.h>

// // LED Strip
const int numLeds = 116; // Change if your setup has more or less LED's
// const int numberOfChannels = numLeds * 3; // Total number of DMX channels you want to receive (1 led = 3 channels)
// #define DATA_PIN 4 //The data pin that the WS2812 strips are connected to.
CRGB leds[numLeds];


// void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
// {
//   if (universe == 0) {
//     for (int i = 0; i < numLeds; i++)
//     {
//       int j = i * 3;
//       leds[i] = CRGB(data[j], data[j + 1], data[j + 2]);
//     }
//     FastLED.show();
//   }
// }



// int main(int argc, char const *argv[])
// {
  
//   FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, numLeds);
//   return 0;
// }


// template<
// template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, 
// uint8_t DATA_PIN, 
// EOrder RGB_ORDER>
// static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
//   // static CHIPSET<DATA_PIN, RGB_ORDER> c;
//   QuixantController<DATA_PIN, RGB> controller;
//   return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
// }

int main(int argc, char const *argv[]) 
{
  namespace fs = std::filesystem;

  if (argc != 2) {
    std::cerr << "this program needs 'animation.pcap' as input arguments" << std::endl;
    return 1;
  }
  
  std::string show_file(argv[1]);
  
  const uint8_t DATA_PIN = 3;
  QuixantController<DATA_PIN> controller;
  FastLED.addLeds(&controller);
  // FastLED.addLeds<QuixantController, DATA_PIN, RGB>(leds, numLeds);
  

  Show* show = new Show("loop", show_file);
  show->cache();
  show->set_background(true);
  // show->send_cached();

  return 0;
}
