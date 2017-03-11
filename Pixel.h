#ifndef PIXEL_H
#define PIXEL_H

#include <avr/io.h>

#define NUM_DIM_BITS (2)
#define NUM_DIM_LEVELS (1 << NUM_DIM_BITS)
#define DIM_MASK (NUM_DIM_LEVELS - 1)
#define MAX_BRIGHT (DIM_MASK)

// In shift order, based on hardware.
enum PixelColor { FIRST_COLOR = 0, GREEN = FIRST_COLOR, RED, BLUE, NUM_COLORS };
inline PixelColor operator++(PixelColor& x) { return x = (PixelColor)(((int)(x) + 1)); };

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

  bool operator== (const Pixel & other) {
    if (this == &other) {
      return true;
    }
    return (red == other.red) && (green = other.green) && (blue == other.blue);
  }

  void print(const char *prefix = "") {
    Serial.print(prefix);
    Serial.print(pixel2color(*this), HEX);
    Serial.print(" ");
  }

  static uint16_t pixel2color(Pixel &pixel) {
    return (pixel.red << (16 - NUM_DIM_BITS)) |
      (pixel.green << (11 - NUM_DIM_BITS)) |
      (pixel.blue << (5 - NUM_DIM_BITS));
  }

  static Pixel *color2pixel(uint16_t color) {
    return new Pixel(color >> (16 - NUM_DIM_BITS),
      (color >> (11 - NUM_DIM_BITS)) & DIM_MASK,
      (color >> (5 - NUM_DIM_BITS)) & DIM_MASK);
  }

};

#define RGB2color(red, green, blue) \
  (((red) << (16 - NUM_DIM_BITS)) | \
    ((green) << (11 - NUM_DIM_BITS)) | \
    ((blue) << (5 - NUM_DIM_BITS)))

#endif // PIXEL_H

