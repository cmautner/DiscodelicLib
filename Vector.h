#ifndef VECTOR_H
#define VECTOR_H

#include <Arduino.h>
#include "Pixel.h"

const uint8_t NUM_LEDS = 8;

// The direction that the cables cause the LED array to be oriented. Some
// panels are oriented up, some down.
enum Orientation { UP, DOWN };

/*
 * A row of LEDs. Some day this may be a column for moving data left/right as well
 * as top/bottom
 */
class Vector {
  public:
    Vector() { }
#if (NUM_DIM_BITS == 4)
    uint32_t leds[NUM_COLORS];
#elif (NUM_DIM_BITS == 2)
    uint16_t leds[NUM_COLORS];
#endif
    void setLed(int ledNdx, Pixel &pixel) {
      uint8_t shiftPosition = m_orientation == UP ? (NUM_LEDS - 1 - ledNdx) : ledNdx;
      uint8_t shiftValue = NUM_DIM_BITS * shiftPosition;
      uint32_t mask = ~(((uint32_t)DIM_MASK) << shiftValue);
      leds[RED] &= mask;
      leds[RED] |= (uint32_t)pixel.red << shiftValue;
      leds[GREEN] &= mask;
      leds[GREEN] |= (uint32_t)pixel.green << shiftValue;
      leds[BLUE] &= mask;
      leds[BLUE] |= (uint32_t)pixel.blue << shiftValue;
    }

    void getLed(int ledNdx, Pixel &pixel) {
      uint8_t shiftValue = NUM_DIM_BITS * (NUM_LEDS - 1 - ledNdx);
      pixel.red = leds[RED] >> shiftValue;
      pixel.green = leds[GREEN] >> shiftValue;
      pixel.blue = leds[BLUE] >> shiftValue;
    }

    void setOrientation(Orientation orientation) {
      m_orientation = orientation;
    }

    bool isOrientedUp() {
      return m_orientation == UP;
    }

    void print() {
      Serial.print(leds[RED], HEX);
      Serial.print("-");
      Serial.print(leds[GREEN], HEX);
      Serial.print("-");
      Serial.println(leds[BLUE], HEX);
    }

  private:
    Orientation m_orientation = UP;
};

#endif // VECTOR_H
