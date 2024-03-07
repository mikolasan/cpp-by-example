/// @file    ColorTemperature.ino
/// @brief   Demonstrates how to use @ref ColorTemperature based color correction
/// @example ColorTemperature.ino

#include <FastLED.h>

#define CHIPSET     WS2812
#define COLOR_ORDER GRB

#define LED_PIN_1     12
#define LED_PIN_2     13

#define NUM_LEDS_1    48
#define NUM_LEDS_2    48

CRGB leds_1[NUM_LEDS_1];
CRGB leds_2[NUM_LEDS_2];

#define BRIGHTNESS  20

// FastLED provides two color-management controls:
//   (1) color correction settings for each LED strip, and
//   (2) master control of the overall output 'color temperature' 
//
// THIS EXAMPLE demonstrates the second, "color temperature" control.
// It shows a simple rainbow animation first with one temperature profile,
// and a few seconds later, with a different temperature profile.
//
// The first pixel of the strip will show the color temperature.
//
// HELPFUL HINTS for "seeing" the effect in this demo:
// * Don't look directly at the LED pixels.  Shine the LEDs aganst
//   a white wall, table, or piece of paper, and look at the reflected light.
//
// * If you watch it for a bit, and then walk away, and then come back 
//   to it, you'll probably be able to "see" whether it's currently using
//   the 'redder' or the 'bluer' temperature profile, even not counting
//   the lowest 'indicator' pixel.
//
//
// FastLED provides these pre-conigured incandescent color profiles:
//     Candle, Tungsten40W, Tungsten100W, Halogen, CarbonArc,
//     HighNoonSun, DirectSunlight, OvercastSky, ClearBlueSky,
// FastLED provides these pre-configured gaseous-light color profiles:
//     WarmFluorescent, StandardFluorescent, CoolWhiteFluorescent,
//     FullSpectrumFluorescent, GrowLightFluorescent, BlackLightFluorescent,
//     MercuryVapor, SodiumVapor, MetalHalide, HighPressureSodium,
// FastLED also provides an "Uncorrected temperature" profile
//    UncorrectedTemperature;

#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3

void loop()
{
  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  --starthue;
  fill_rainbow( leds_1, NUM_LEDS_1, starthue, 20);
  fill_rainbow( leds_2, NUM_LEDS_2, starthue, 20);

  // Choose which 'color temperature' profile to enable.
  uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
  if( secs < DISPLAYTIME) {
    FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
//    leds_1[0] = TEMPERATURE_1; // show indicator pixel
//    leds_2[0] = TEMPERATURE_1; // show indicator pixel
  } else {
    FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
//    leds_1[0] = TEMPERATURE_2; // show indicator pixel
//    leds_2[0] = TEMPERATURE_2; // show indicator pixel
  }

  // Black out the LEDs for a few secnds between color changes
  // to let the eyes and brains adjust
  if( (secs % DISPLAYTIME) < BLACKTIME) {
    memset8( leds_1, 0, NUM_LEDS_1 * sizeof(CRGB));
    memset8( leds_2, 0, NUM_LEDS_2 * sizeof(CRGB));
  }
  
  FastLED.show();
  FastLED.delay(8);
}

void setup() {
  delay( 3000 ); // power-up safety delay
  // It's important to set the color correction for your LED strip here,
  // so that colors can be more accurately rendered through the 'temperature' profiles
  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds_1, NUM_LEDS_1).setCorrection( TypicalSMD5050 );
  FastLED.addLeds<CHIPSET, LED_PIN_2, COLOR_ORDER>(leds_2, NUM_LEDS_2).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( BRIGHTNESS );
}
