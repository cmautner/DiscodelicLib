#ifndef DISCODELIC_LIB_H
#define DISCODELIC_LIB_H

#include "Adafruit_GFX.h"
#include "Panel.h"


enum FrameId {
  FRAME_CURRENT,  // The panel being displayed.
  FRAME_NEXT      // The panel the animator is drawing into.
};
inline FrameId operator++(FrameId& x) { return x = (FrameId)(((int)(x) + 1)); };

// For wrapping around entire cube:
// To write to PANEL_TOP: setWidePanelMode(false); setGfxPanel(PANEL_TOP);
// To write to sides: call setWidePanelMode(true) then x values are interpreted as follows:
// 0 >= x <= 7 PANEL_LEFT
// 8 >= x <= 15 PANEL_FRONT
// 16 >= x <= 23 PANEL_RIGHT
// 24 >= x <= 31 PANEL_BACK
const uint8_t WIDE_PANEL_LEFT_START = 0;
const uint8_t WIDE_PANEL_FRONT_START = WIDE_PANEL_LEFT_START + NUM_LEDS;
const uint8_t WIDE_PANEL_RIGHT_START = WIDE_PANEL_FRONT_START + NUM_LEDS;
const uint8_t WIDE_PANEL_BACK_START = WIDE_PANEL_RIGHT_START + NUM_LEDS;
const uint8_t WIDE_PANEL_END = WIDE_PANEL_BACK_START + NUM_LEDS;

class Discodelic {
  public:

    class Discodelic_GFX : public Adafruit_GFX {

      public:
        void setGfxPanel(PanelId panelNdx) {
          pCurrentGfxPanel = mDiscodelic.getPanel(FRAME_NEXT, panelNdx);
        }

        void drawPixel(int16_t x, int16_t y, uint16_t color) {
          if ((x < 0) || (y < 0) || (x >= mPanelWidth) || (y >= NUM_ROWS)) {
            return;
          }
          Panel *pPanel;
          PanelId panelId;

          if (mPanelWidth == WIDE_PANEL_END) {
            if (x >= WIDE_PANEL_BACK_START) {
              panelId = PANEL_BACK;
            } else if (x >= WIDE_PANEL_RIGHT_START) {
              panelId = PANEL_RIGHT;
            } else if (x >= WIDE_PANEL_FRONT_START) {
              panelId = PANEL_FRONT;
            } else {
              panelId = PANEL_LEFT;
            }
            pPanel = mDiscodelic.getPanel(FRAME_NEXT, panelId);
          } else {
            pPanel = pCurrentGfxPanel;
          }

          x = x & LEDS_MASK;
          pPanel->getRow(y)->setLed(x, color);

//          Serial.print("x=");
//          Serial.print(x);
//          Serial.print(",y=");
//          Serial.print(y);
//          Serial.print(",color=");
//          Serial.print(color, HEX);
//          Serial.print(",UP=");
//          Serial.print(pCurrentGfxPanel->isOrientedUp());
//          Serial.println();
        }

        /*
         * Turn wide panel mode on or off.
         * Parameters:
         *  enable: true turns it on, false turns it off.
         */
        void setWidePanelMode(bool enable) {
          mPanelWidth = enable ? WIDE_PANEL_END : NUM_LEDS;
        }

        uint16_t getTextBgColor() {
          return textbgcolor;
        }

        Discodelic_GFX(Discodelic &discodelic) : Adafruit_GFX(WIDE_PANEL_END, NUM_ROWS),
          mDiscodelic(discodelic), mPanelWidth(NUM_LEDS) { }

      private:
        Panel *pCurrentGfxPanel = mDiscodelic.getPanel(FRAME_NEXT, PANEL_TOP);
        Discodelic &mDiscodelic;
        uint8_t mPanelWidth;
    };

    Discodelic_GFX mDiscodelicGfx;

    Discodelic() : mDiscodelicGfx(*this) { }

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
     * Get the pixel immediately below the specified pixel of the top panel. Works only for
     * edge pixels on the top panel. For corner pixel, averages the two corner pixels below
     * unless one of the pixels is the text background color.
     */
    void getTopPanelNeighborPixel(Pixel &pixel, uint16_t x, uint16_t y);
    /*
     * Indicate to the refresh() method that the new frame is ready for presenting.
     */
    void swapBuffers(bool immediate = false);
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

  private:
};

typedef Discodelic::Discodelic_GFX DiscodelicGfx;

extern const int SWITCH;  // High or low for user input

extern Discodelic Discodelic1;
extern DiscodelicGfx DiscodelicGfx1;

#endif // DISCODELIC_LIB_H