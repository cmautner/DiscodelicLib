#ifndef PANEL_H
#define PANEL_H

#include "Vector.h"

const uint8_t NUM_ROWS = 8;

/*
 * A set of Vectors, one per row, corresponding to one face of the Cube.
 */
class Panel {
public:
  Panel() { }
  Vector *getRow(int rowNdx) {
    return &rows[rowNdx];
  }
  Vector *getShiftRow(int rowNdx) {
    return &rows[m_orientation == UP ? rowNdx : NUM_ROWS - rowNdx - 1];
  }
  void setOrientation(Orientation orientation) {
    m_orientation = orientation;
  }
  bool isOrientedUp() {
    return m_orientation == UP;
  }

private:
  Vector rows[NUM_ROWS];
  Orientation m_orientation = UP;
};

#endif // PANEL_H 
