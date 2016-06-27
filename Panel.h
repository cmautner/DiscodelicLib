#ifndef PANEL_H
#define PANEL_H

#include "Vector.h"

const uint8_t NUM_ROWS = 8;

class Panel {
public:
  Panel() { }
  Vector *getRow(int index);
  Vector *getShiftRow(int index);
  void setOrientation(Orientation orientation) {
    m_orientation = orientation;
  }
  bool isOrientedUp() {
    return m_orientation == UP;
  }
/*
  setNeighbors(Vector &above, Vector &below, Vector &left, Vector &right) : 
    m_above(above), m_below(below), m_left(left), m_right(right) { }
  Pixel& getPixel(int x, int y);
  Column& getColumn(int index);
  Column& getColumn(Orientation orientation);
  Vector& getRow(Orientation orientation);
*/

private:
  Vector rows[NUM_ROWS];
  Orientation m_orientation = UP;
//  Vector &m_above, &m_below, &m_left, &m_right;
};

#endif // PANEL_H 
