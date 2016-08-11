#include "DiscodelicLib.h"

uint16_t pixel2color(Pixel pixel) {
  return (pixel.red << (16 - NUM_DIM_BITS)) |
    (pixel.green << (11 - NUM_DIM_BITS)) |
    (pixel.blue << (5 - NUM_DIM_BITS));
}

Pixel *color2pixel(uint16_t color) {
  return new Pixel(color >> (16 - NUM_DIM_BITS),
    (color >> (11 - NUM_DIM_BITS)) & DIM_MASK,
    (color >> (5 - NUM_DIM_BITS)) & DIM_MASK);
}
