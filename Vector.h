#ifndef VECTOR_H
#define VECTOR_H

#include <Arduino.h>
#include "Pixel.h"

const uint8_t NUM_LEDS = 8;
enum Orientation { UP, DOWN };

class Vector {
  public:
    Vector() { }
#if (NUM_DIM_BITS == 4)
    uint32_t leds[NUM_COLORS];
#elif (NUM_DIM_BITS == 2)
    uint16_t leds[NUM_COLORS];
#endif
    void setLed(int ledNdx, Pixel &pixel) {
      uint8_t shiftValue = NUM_DIM_BITS * (NUM_LEDS - 1 - ledNdx);
      uint32_t mask = ~(((uint32_t)DIM_MASK) << shiftValue);
//        Serial.print("setLed ledNdx ");
//        Serial.print(ledNdx);
//        Serial.print(", mask ");
//        Serial.print(mask, HEX);
//        Serial.print(", before ");
//        Serial.print(leds[GREEN], HEX);
//        Serial.print("-");
//        Serial.print(leds[RED], HEX);
//        Serial.print("-");
//        Serial.print(leds[BLUE], HEX);
      leds[RED] &= mask;
      leds[RED] |= (uint32_t)pixel.red << shiftValue;
      leds[GREEN] &= mask;
      leds[GREEN] |= (uint32_t)pixel.green << shiftValue;
      leds[BLUE] &= mask;
      leds[BLUE] |= (uint32_t)pixel.blue << shiftValue;
//      Serial.print(", after ");
//      Serial.print(leds[GREEN], HEX);
//      Serial.print("-");
//      Serial.print(leds[RED], HEX);
//      Serial.print("-");
//      Serial.println(leds[BLUE], HEX);
    }

    void getLed(int ledNdx, Pixel &pixel) {
      uint8_t shiftValue = NUM_DIM_BITS * (NUM_LEDS - 1 - ledNdx);
      pixel.red = leds[RED] >> shiftValue;
      pixel.green = leds[GREEN] >> shiftValue;
      pixel.blue = leds[BLUE] >> shiftValue;
//        Serial.print("getLed ledNdx ");
//        Serial.print(ledNdx);
//        Serial.print(", ");
//        Serial.print(leds[GREEN], HEX);
//        Serial.print("-");
//        Serial.print(leds[RED], HEX);
//        Serial.print("-");
//        Serial.println(leds[BLUE], HEX);
    }

    void print() {
      Serial.print(leds[RED], HEX);
      Serial.print("-");
      Serial.print(leds[GREEN], HEX);
      Serial.print("-");
      Serial.println(leds[BLUE], HEX);
    }
};
/*
class Column : public Vector {
  public:
    Column(Orientation orientation, Vector left, Vector right) : Vector(orientation),
      m_left(left), m_right(right) {
      
    }

  private:
    Vector m_left, m_right;
    ColorVector[3] leds;
}

class Row : public Vector {
  public:
    Row(Orientation orientation, Vector above, Vector below) : Vector(orientation),
      m_above(above), m_below(below) {
      
    }

  private:
    Vector m_above, m_below;
    struct {
      uint8_t led0 : 4, led1 : 4, led2 : 4, led3 : 4, led4 : 4, led5 : 4, led6 : 4, led7 : 4;
    } leds[
}
}
*/

#endif // VECTOR_H
