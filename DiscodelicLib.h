#ifndef DISCODELIC_LIB_H
#define DISCODELIC_LIB_H

#include "Adafruit_GFX.h"
#include "Panel.h"


enum FrameId {
  FRAME_CURRENT,  // The panel being displayed.
  FRAME_NEXT      // The panel the animator is drawing into.
};
inline FrameId operator++(FrameId& x) { return x = (FrameId)(((int)(x) + 1)); };

class Discodelic {
  public:

    class Discodelic_GFX : public Adafruit_GFX {

      public:
        void setGfxPanel(PanelId panelNdx) {
          pCurrentGfxPanel = mDiscodelic.getPanel(FRAME_NEXT, panelNdx);
        }

        void drawPixel(int16_t x, int16_t y, uint16_t color) {
          if ((x < 0) || (x >= NUM_LEDS) || (y < 0) || (y >= NUM_ROWS)) {
            return;
          }
          pCurrentGfxPanel->getRow(y)->setLed(x, color);


//          Serial.print("x=");
//          Serial.print(x);
//          Serial.print(",y=");
//          Serial.print(y);
//          Serial.print(",color=");
//          Serial.print(color, HEX);
//          Serial.print(",UP=");
//          Serial.println(pCurrentGfxPanel->isOrientedUp());

        }

        Discodelic_GFX(Discodelic &discodelic, int width = NUM_LEDS, int height = NUM_ROWS) : Adafruit_GFX(width, height), mDiscodelic(discodelic) {}

      private:
        Panel *pCurrentGfxPanel = mDiscodelic.getPanel(FRAME_NEXT, PANEL_TOP);
        Discodelic &mDiscodelic;
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
//    Discodelic_GFX mDiscodelicGfxWidePanel((NUM_PANELS - 1) * NUM_PIXELS, NUM_ROWS);

};

typedef Discodelic::Discodelic_GFX DiscodelicGfx;

extern const int SWITCH;  // High or low for user input

extern Discodelic Discodelic1;

#endif // DISCODELIC_LIB_H