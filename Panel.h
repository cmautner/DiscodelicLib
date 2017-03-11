#ifndef PANEL_H
#define PANEL_H

#include "Vector.h"

const uint8_t NUM_ROWS = 8;
const uint8_t ROWS_MASK = NUM_ROWS - 1;

// In order from last shift register (first data shifted in) to first.
enum PanelId {
  PANEL_FIRST = 0,
  PANEL_BACK = 0,
  PANEL_TOP,
  PANEL_LEFT,
  PANEL_FRONT,
  PANEL_RIGHT,
  NUM_PANELS
};
inline PanelId operator++(PanelId& x) { return x = (PanelId)(((int)(x) + 1)); };

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
    for (int rowNdx = 0; rowNdx < NUM_ROWS; ++rowNdx) {
      rows[rowNdx].setOrientation(orientation);
    }
  }
  bool isOrientedUp() {
    return m_orientation == UP;
  }

private:
  Vector rows[NUM_ROWS];
  Orientation m_orientation = UP;
};

#endif // PANEL_H
