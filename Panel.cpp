#include "Panel.h"


Vector *Panel::getRow(int rowNdx) {
  return &rows[rowNdx];
}

Vector *Panel::getShiftRow(int rowNdx) {
  return &rows[m_orientation == UP ? rowNdx : NUM_ROWS - rowNdx - 1];
}

