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
   * Call from Arduino setup() to initialize Discodelic Cube.
   */
  void setup(void);
  /*
   * Call from Arduino loop() to update LEDs.
   */
  void refresh(void);
  /*
   * Retrieve a pointer to the desired Panel. The pointer can then be used to retrieve
   * rows of type Vector, and LEDs of type Pixel.
   * Parameters:
   *  frameNdx: FrameId indicating which animation frame to retrieve the Panel from.
   *  panelNdx: PanelId indicating which Panel to retrieve.
   * Return:
   *  Panel *
   */
  Panel *getPanel(FrameId frameNdx, PanelId panelNdx);
  /*
   * Indicate to the refresh() method that the new frame is ready for presenting.
   */
  void swapBuffers(void);
  /*
   * Diagnostic dump of the specified panel.
   * Parameters:
   *  frameNdx: FrameId indicating which animation frame to dump.
   *  panelNdx: PanelId indicating which panel to dump.
   */
  void dumpPanel(FrameId frameNdx, PanelId panelNdx);
  /*
   * Dump all the panels.
   */
  void dumpAllPanels(void);
};

extern const int SWITCH;  // High or low for user input

extern Discodelic Discodelic1;

#endif // DISCODELIC_LIB_H