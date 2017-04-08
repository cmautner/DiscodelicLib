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
const uint8_t TALL_PANEL_END = 2 * NUM_ROWS;

class Discodelic {
  public:
    static bool (*sCallback)();
    static void animateFrame();
    /*
     * Set the frame period and the function to call to animate.
     * updatePeriod: in microseconds.
     * callback: return true to swap frames, false to continue with same frame.
     */
    static void registerCallback(unsigned long updatePeriod, bool (*callback)());
    /*
     * Indicate to the refresh() method that the new frame is ready for presenting.
     */
    static void swapBuffers(bool immediate = false);

    class Discodelic_GFX : public Adafruit_GFX {
      private:
        PanelId xToPanelId(int16_t x) {
          if (x >= WIDE_PANEL_BACK_START) {
            return PANEL_BACK;
          } else if (x >= WIDE_PANEL_RIGHT_START) {
            return PANEL_RIGHT;
          } else if (x >= WIDE_PANEL_FRONT_START) {
            return PANEL_FRONT;
          } else {
            return PANEL_LEFT;
          }
        }

      public:
        void setGfxPanel(PanelId panelNdx) {
          pCurrentGfxPanel = mDiscodelic.getPanel(FRAME_NEXT, panelNdx);
        }

        void drawPixel(int16_t x, int16_t y, uint16_t color) {
          if (mTrace) {
            Serial.print("dp: ");
            Serial.print(x);
            Serial.print(",");
            Serial.print(y);
            Serial.print(" ");
            Serial.print(color >> (16 - NUM_DIM_BITS));
            Serial.print(",");
            Serial.print((color >> (11 - NUM_DIM_BITS)) & DIM_MASK);
            Serial.print(",");
            Serial.println((color >> (5 - NUM_DIM_BITS)) & DIM_MASK);
          }
          if ((y < 0) || (y >= mPanelHeight)) {
            return;
          }
          if (!mWrap && ((x < 0) || (x >= mPanelWidth))) {
            return;
          }

          while (x < 0) {
            x += mPanelWidth;
          }
          while (x >= mPanelWidth) {
            x -= mPanelWidth;
          }

          int16_t wideX = x;
          int16_t normX = x & LEDS_MASK;
          int16_t tallY = y;
          int16_t normY = y & ROWS_MASK;

          Panel *pPanel;
          if ((mPanelHeight == TALL_PANEL_END) && (tallY < NUM_ROWS)) {
            // Top panel wrapping down to side panel.
            pPanel = mDiscodelic.getPanel(FRAME_NEXT, PANEL_TOP);
            switch (xToPanelId(wideX)) {
              // Convert x,y coordinates to top panel coordinates.
              case PANEL_BACK:
                x = NUM_LEDS - normX - 1;
                y = NUM_ROWS - normY - 1;
                break;
              case PANEL_RIGHT:
                x = normY;
                y = NUM_LEDS - normX - 1;
                break;
              case PANEL_FRONT:
                x = normX;
                y = normY;
                break;
              case PANEL_LEFT:
                x = NUM_LEDS - normY - 1;
                y = normX;
                break;
              default:
                break;
            }
          } else if (mPanelWidth == WIDE_PANEL_END) {
            x = normX;
            y = normY;
            pPanel = mDiscodelic.getPanel(FRAME_NEXT,  xToPanelId(wideX));
          }else {
            x = normX;
            y = normY;
            pPanel = pCurrentGfxPanel;
          }

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

        /*
         * Turn tall panel mode on or off. This maps every side up and over the top.
         * Parameters:
         *  enable: true turns it on, false turns it off.
         */
        void setTallPanelMode(bool enable) {
          mPanelHeight = enable ? TALL_PANEL_END : NUM_ROWS;
        }

        void setTraceMode(bool enable) {
          mTrace = enable;
        }

        void setWrapMode(bool enable) {
          mWrap = enable;
        }

        uint16_t getTextBgColor() {
          return textbgcolor;
        }

        Discodelic_GFX(Discodelic &discodelic) : Adafruit_GFX(WIDE_PANEL_END, TALL_PANEL_END),
          mDiscodelic(discodelic), mPanelWidth(NUM_LEDS), mPanelHeight(NUM_ROWS) { }

      private:
        Panel *pCurrentGfxPanel = mDiscodelic.getPanel(FRAME_NEXT, PANEL_TOP);
        Discodelic &mDiscodelic;
        uint8_t mPanelWidth;
        uint8_t mPanelHeight;
        bool mTrace;
        bool mWrap;
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