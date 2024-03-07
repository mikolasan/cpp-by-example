

// no more than 9 µs of idle time may occur during data transfer

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <ncurses.h>

#include "crgb.h"
#include "show_led.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <libqxt.h>
#include <libqxt_lp.h>

#ifdef __cplusplus
}
#endif

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

void reset() {
  std::this_thread::sleep_for(std::chrono::nanoseconds(300000));
}

void set_pin_high(const uint32_t port) {
  const unsigned int offset = (port / 8);
  const unsigned int bit_mask = (1 << (port % 8));
  qxt_dio_setbit_fast(offset, bit_mask);
}

void set_pin_low(const uint32_t port) {
  const unsigned int offset = (port / 8);
  const unsigned int bit_mask = (1 << (port % 8));
  qxt_dio_clearbit_fast(offset, bit_mask);
}

void send_bit(uint32_t port, uint8_t bit) {
  if (bit == 1) {
    set_pin_high(port);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    set_pin_low(port);
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
  } else { // 0
    set_pin_high(port);
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    set_pin_low(port);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
  }

  // if (bit == 1) {
  //   set_pin_high(port);
  //   // Wait for 0.7µs
  //   std::this_thread::sleep_for(std::chrono::milliseconds(700));
  //   set_pin_low(port);
  //   // Wait for 0.6µs
  //   std::this_thread::sleep_for(std::chrono::milliseconds(600));
  // } else {
  //   set_pin_high(port);
  //   // Wait for 0.35µs
  //   std::this_thread::sleep_for(std::chrono::milliseconds(350));
  //   set_pin_low(port);
  //   // Wait for 0.8µs
  //   std::this_thread::sleep_for(std::chrono::milliseconds(800));
  // }
}

void send_byte(uint32_t port, uint8_t byte) {
  for (int i = 7; i >= 0; --i) {
    send_bit(port, (byte >> i) & 0x01);
  }
}

void send_data(uint32_t port, const std::vector<CRGB>& colors, size_t max_length) {
  for (size_t i = 0; i < colors.size(); i++) {
    // send 24-bit packet (3x8 bits)
    send_byte(port, colors[i].g);
    send_byte(port, colors[i].r);
    send_byte(port, colors[i].b);
  }
}

std::map<uint8_t, size_t> universe_size_map = {
  {0, 10},
  {1, 10},
};

std::map<uint8_t, uint32_t> universe_port_map = {
  {0, 20},
  {1, 22},
};

int main(int argc, char const *argv[]) 
{
  // Initialize ncurses
  initscr();
  cbreak();  // Line buffering disabled, Pass on everything
  // noecho();  // Don't echo() while we do getch
  // keypad(stdscr, TRUE);  // Enable special keys

  // namespace fs = std::filesystem;

  // if (argc != 2) {
  //   std::cerr << "this program needs 'animation.pcap' as input arguments" << std::endl;
  //   return 1;
  // }

  qxt_device_initEx();
  {
    QXT_CORE_INVENTORY inv;
    unsigned int err_code = qxtCoreGetFullInventory(&inv);
    std::cout << "platformId:               " << (inv.platformId&0xff) << std::endl;
    std::cout << "platformSpecial:          " << (inv.platformSpecial&0xff) << std::endl;
    std::cout << "backplaneInfo:            " << (inv.backplaneInfo&0xff) << std::endl;
    std::cout << "capabilities:             " << (inv.capabilities&0xff) << std::endl;
    std::cout << "driverVersion:            " << inv.driverVersion << std::endl;
    std::cout << "fpgaVersion:              " << inv.fpgaVersion << std::endl;
    std::cout << "loggingProcessorVersion:  " << inv.loggingProcessorVersion << std::endl;
    std::cout << "libraryVersion:           " << inv.libraryVersion << std::endl;
    std::cout << "driverProductName:        " << inv.driverProductName << std::endl;
    std::cout << "libraryProductName:       " << inv.libraryProductName << std::endl;
    std::cout << "isLoggingProcessorOnline: " << (inv.isLoggingProcessorOnline&0xff) << std::endl;
    std::cout << "rtcType:                  " << (inv.rtcType&0xff) << std::endl;
  }


  refresh();
  std::vector<CRGB> colors = {
    CRGB::Red,
    CRGB::Red,
    CRGB::Green,
    CRGB::Green,
    CRGB::Green,
  };
  int i = 0;
  for (const auto& c : colors) {
    std::cout << i 
      << ": R " << (int)c.r 
      << " G " << (int)c.g 
      << " B " << (int)c.b 
      << std::endl;
    ++i;
  }
  refresh();
  
  uint32_t port = 22;
  size_t max_length = 100;
  bool key_pressed = false;
  while (!key_pressed) {
    // for (const auto&[universe_id, port] : universe_port_map) {
      // uint32_t max_length = universe_size_map[universe_id];
      send_data(31, colors, max_length);
      // send_data(22, colors, max_length);
      // std::reverse(colors.begin(), colors.end());
    // }
    reset();

    int ch = getch();
    if (ch == 'c' || ch == 'C') {
      key_pressed = true;
    }
  }
  
  qxt_device_shutdown();

  // std::string show_file(argv[1]);
  
  // Show* show = new Show("loop", show_file);
  // show->cache();
  // show->set_background(true);
  // // show->send_cached();
  
  // Cleanup ncurses
  endwin();

  return 0;
}
