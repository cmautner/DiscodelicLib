#ifndef DISCODELIC_LIB_H
#define DISCODELIC_LIB_H

#include "Panel.h"


enum FrameId {
  FRAME_CURRENT,  // The panel being displayed.
  FRAME_NEXT      // The panel the animator is drawing into.
};
inline FrameId operator++(FrameId& x) { return x = (FrameId)(((int)(x) + 1)); };

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

class Discodelic {
public:

  /*
   * Must be called from Arduino setup()
   */
  void setup(void);
  /*
   * Must be called from Arduino loop()
   */
  void refresh(void);
  /*
   * Dump the specified panel ()
   */
  void dumpPanel(FrameId frameNdx, PanelId panelNdx);
  void dumpAllPanels(void);
  Panel *getPanel(FrameId frameNdx, PanelId panelNdx);
  void swapBuffers(void);

private:
};

extern const int SWITCH;  // High or low for user input

extern Discodelic Discodelic1;

#endif // DISCODELIC_LIB_H