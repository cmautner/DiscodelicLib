#ifndef PIXEL_H
#define PIXEL_H

#include <avr/io.h>

#define NUM_DIM_BITS 2
#define DIM_MASK ((1 << NUM_DIM_BITS) - 1)
#define MAX_BRIGHT DIM_MASK

// In shift order, based on hardware.
enum PixelColor { FIRST_COLOR = 0, GREEN = FIRST_COLOR, RED, BLUE, NUM_COLORS };
class Pixel {
public:
  Pixel(uint8_t _red = 0, uint8_t _green = 0, uint8_t _blue = 0) : red(_red), green(_green), blue(_blue) { }

  void set(uint8_t _red, uint8_t _green, uint8_t _blue) {
    red = _red; green = _green; blue = _blue;
  }

  uint8_t red : NUM_DIM_BITS, green : NUM_DIM_BITS, blue : NUM_DIM_BITS;

  Pixel & operator= (const Pixel & other) {
    if (this != &other) {
      red = other.red;
      green = other.green;
      blue = other.blue;
    }
    return *this;
  }
  void print(const char *prefix = "") {
    Serial.print(prefix);
    Serial.print(green, HEX);
    Serial.print("-");
    Serial.print(red, HEX);
    Serial.print("-");
    Serial.print(blue, HEX);
    Serial.print(" ");
  }
};

#endif // PIXEL_H
