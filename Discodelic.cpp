#include <TimerOne.h>
#include "DiscodelicLib.h"

// Singletons
Discodelic Discodelic1;
DiscodelicGfx DiscodelicGfx1 = Discodelic1.mDiscodelicGfx;

const uint8_t DDRC_INIT = 0x37; // PC0-2, 4-5 as outputs. All others as inputs
const uint8_t DDRB_INIT = 0x23; // PB0-1,5 as outputs. All others as inputs

const int SWITCH = 7;   // User input
const int BLANK_ = 8;  // Enable for LEDs, active low
const int LATCH = 9;

// Buffer access
enum PingPongBuffers {
  PING = 0,
  PONG,
  NUM_BUFFERS
};
Panel panels[NUM_BUFFERS][NUM_PANELS];
static volatile uint8_t loopFrameNdx;
static volatile uint8_t animateFrameNdx;
static volatile bool switchBuffers = false;
bool (*Discodelic::sCallback)();


const char * pingPongStrings[] = {
  "CURRENT",
  "NEXT"
};

const char * panelStrings[] = {
  "PANEL_BACK",
  "PANEL_TOP",
  "PANEL_LEFT",
  "PANEL_FRONT",
  "PANEL_RIGHT"
};

void Discodelic::animateFrame() {
  if (sCallback == NULL) {
    return;
  }

  // Turn off outputs (may already be off)
  int blanked = digitalRead(BLANK_);
  digitalWrite(BLANK_, HIGH);

  if ((*sCallback)()) {
    swapBuffers(false);
  }

  if (blanked == LOW) {
    // Turn output back on
    digitalWrite(BLANK_, LOW);
  }
}

void Discodelic::registerCallback(unsigned long updatePeriod, bool (*callback)()) {
  sCallback = callback;
  Timer1.initialize(updatePeriod);
  Timer1.attachInterrupt(Discodelic::animateFrame);
}

void Discodelic::setup() {

  // Set the pin directions for ports C (SCL, SDA, row select) and B (BLANK_, LAT)
  pinMode(SWITCH, INPUT_PULLUP); // Set IO7 as input
  DDRC = DDRC_INIT;
  DDRB = DDRB_INIT;

  // Set the orientation to reflect the orientation of the connecting cables.
  for (int frameNdx = 0; frameNdx < NUM_BUFFERS; ++frameNdx) {
    panels[frameNdx][PANEL_TOP].setOrientation(UP);
    panels[frameNdx][PANEL_BACK].setOrientation(DOWN);
    panels[frameNdx][PANEL_LEFT].setOrientation(DOWN);
    panels[frameNdx][PANEL_FRONT].setOrientation(UP);
    panels[frameNdx][PANEL_RIGHT].setOrientation(DOWN);
  }

  // Set each panel to a different color.
  Pixel pixel;
  for (int frameNdx = 0; frameNdx < NUM_BUFFERS; ++frameNdx) {
    for (int panelNdx = 0; panelNdx < NUM_PANELS; ++panelNdx) {
      for (int rowNdx = 0; rowNdx < NUM_ROWS; ++rowNdx) {
        Vector *pVector = panels[frameNdx][panelNdx].getRow(rowNdx);
        for (int pixelNdx = 0; pixelNdx < NUM_LEDS; ++pixelNdx) {
          // PANEL_BACK yellow, PANEL_TOP red, PANEL_LEFT green, PANEL_FRONT blue, PANEL_RIGHT purple
          pixel.red = (panelNdx == PANEL_TOP || panelNdx == PANEL_BACK || panelNdx == PANEL_RIGHT) ? MAX_BRIGHT : 0;
          pixel.green = (panelNdx == PANEL_LEFT || panelNdx == PANEL_BACK) ? MAX_BRIGHT : 0;
          pixel.blue = (panelNdx == PANEL_FRONT || panelNdx == PANEL_RIGHT) ? MAX_BRIGHT : 0;
          pVector->setLed(pixelNdx, pixel);
        }
      }
    }
  }
  // Set top left and bottom right corners to random values
  for (int panelNdx = PANEL_FIRST; panelNdx < NUM_PANELS; ++panelNdx) {
    for (int rowNdx = 0; rowNdx < NUM_ROWS; ++rowNdx) {
      for (int ledNdx = 0; ledNdx < NUM_LEDS; ++ledNdx) {
        if (((rowNdx == 0) && (ledNdx == 0)) || ((rowNdx == MAX_LED) && (ledNdx == MAX_LED))) {
          Vector *pVector = panels[PONG][panelNdx].getRow(rowNdx);
          Pixel pixel(rand(), rand(), rand());
          pVector->setLed(ledNdx, pixel);
        }
      }
    }
  }

  // Intialize frame indices
  loopFrameNdx = PONG;
  animateFrameNdx = PING;

  // Set SCLK high and BLANK low
  digitalWrite(SCL, HIGH);
  digitalWrite(BLANK_, LOW);
}

// Dimming is done by looking at the R, G, or B value and then choosing to turn on the LED or not based
// on the dimmingSchedule bit mask.
static uint8_t refreshNdx;
static uint8_t rowNdx;

#define NUM_REFRESHES (6)

#if (NUM_DIM_BITS == 4)

//           cycle
//val   0123 4567 89AB CDEF =HEX
//0000  0000 0000 0000 0000 =0x0000
//0001  1000 0000 0000 0000 =0x8000
//0010  1000 0000 1000 0000 =0x8080
//0011  1000 0010 0001 0000 =0x8210
//
//0100  1000 1000 1000 1000
//0101  1000 1001 0010 0100
//0110  1001 0010 1001 0010
//0111  1001 0101 0100 1010
//
//1000  0110 1010 1011 0101
//1001  0110 1101 0110 1101
//1010  0111 0110 1101 1011
//1011  0111 0111 0111 0111
//
//1100  0111 1101 1110 1111
//1101  0111 1111 0111 1111
//1110  0111 1111 1111 1111
//1111  1111 1111 1111 1111

const uint16_t dimmingSchedule[NUM_DIM_LEVELS] = {
  0x0000, 0x8000, 0x8080, 0x8210,
  0x8888, 0x8924, 0x9292, 0x954a,
  0x6ab5, 0x6d6d, 0x76db, 0x7777,
  0x7def, 0x7f7f, 0x7fff, 0xffff
};

#elif (NUM_DIM_BITS == 2)
//           cycle
//val   012345 =HEX
//0000  000000 =0x00
//0001  100000 =0x20
//0010  100100 =0x24
//0011  111111 =0x3f

const uint16_t dimmingSchedule[NUM_DIM_LEVELS] = {
  0x00, 0x20, 0x24, 0x3f
};
#endif

/*
 * Clock out one row of data into the shift registers. Only turn the led on
 * if the R, G, or B lookup value bit is set in this refresh cycle.
 */
void Discodelic::refresh(void) {
  if (++rowNdx >= NUM_ROWS) {
    rowNdx = 0;
    // After all of the rows have been clocked out, increment the refresh cycle.
    if (++refreshNdx >= NUM_REFRESHES) {
      refreshNdx = 0;

      // Switch frame buffers at the end of a refresh cycle if needed.
      if (switchBuffers) {
        int oldLoopFrameNdx = loopFrameNdx;
        loopFrameNdx = animateFrameNdx;
        animateFrameNdx = oldLoopFrameNdx;
        switchBuffers = false;
      }
    }
  }

  // Clock out one entire row
  const int cycleBit = 1 << refreshNdx;

  for (int panelNdx = PANEL_FIRST; panelNdx < NUM_PANELS; ++panelNdx) {
    Panel *panel = &panels[loopFrameNdx][panelNdx];
    // Clock out the row starting at the far end
    Vector *pRow = panel->getShiftRow(rowNdx);

    for (PixelColor color = FIRST_COLOR; color < NUM_COLORS; ++color) {
      uint32_t leds = pRow->leds[color];
      // invariant: SCLK is low
      for (int ledNdx = 0; ledNdx < NUM_LEDS; ++ledNdx, leds >>=  NUM_DIM_BITS) {
        if (dimmingSchedule[leds & DIM_MASK] & cycleBit) {
          // Set SDAT
          PORTC |= 0x10;
        } else {
          // Clear SDAT
          PORTC &= ~0x10;
        }
        // Set SCLK
        PORTC |= 0x20;
        // Clear SCLK and SDAT
        PORTC &= ~0x30;
      }
    }
  }

  // Turn off outputs
  digitalWrite(BLANK_, HIGH);

  // Change the driven LED row and lower SCLK and SDAT
  PORTC = rowNdx;

  // Change the outputs from old shift register to new shift register
  digitalWrite(LATCH, HIGH);
  digitalWrite(LATCH, LOW);

  // Turn on outputs
  digitalWrite(BLANK_, LOW);
}

Panel *Discodelic::getPanel(FrameId frameNdx, PanelId panelNdx) {
  return &panels[frameNdx == FRAME_CURRENT ? loopFrameNdx : animateFrameNdx][panelNdx];
}

/*
 * Modify the colors of pixel1 to be the average of the colors of pixel1 and pixel2.
 * If either of the colors are the background color, just use the background color.
 */
void averagePixels(Pixel &pixel1, Pixel &pixel2, uint16_t textbgcolor) {
  if (textbgcolor == Pixel::pixel2color(pixel2)) {
    // Leave pixel1 unchanged.
    return;
  } else if (textbgcolor == Pixel::pixel2color(pixel1)) {
    // Assign pixel2 to pixel1.
    pixel1 = pixel2;
    return;
  }
  // both valid colors, average them.
  int red = (pixel1.red + pixel2.red) / 2;
  int green = (pixel1.green + pixel2.green) / 2;
  int blue = (pixel1.blue + pixel2.blue) / 2;
  pixel1.set(red, green, blue);
}

/*
 * The edges of the top panel are neighbors to the top row of each side. This function
 * assigns the colors of the neighbor pixel (or pixels for corners) of PANEL_TOP(x,y)
 * to pixel. It does nothing if x,y are not edges of PANEL_TOP.
 */
void Discodelic::getTopPanelNeighborPixel(Pixel &pixel, uint16_t xTop, uint16_t yTop) {
  Panel *pPanel;
  Vector *pRow;

  // Used if x,y specifies a corner.
  Panel *pSecondPanel = NULL;
  uint8_t secondColumn;

  if (xTop == 0) {
    // above PANEL_LEFT
    pPanel = getPanel(FRAME_NEXT, PANEL_LEFT);
    pRow = pPanel->getRow(0);
    pRow->getLed(yTop, pixel);
    if (yTop == 0) {
      // Also above PANEL_BACK
      pSecondPanel = getPanel(FRAME_NEXT, PANEL_BACK);
      secondColumn = MAX_LED;
    } else if (yTop == MAX_LED) {
      // Also above PANEL_FRONT
      pSecondPanel = getPanel(FRAME_NEXT, PANEL_BACK);
      secondColumn = 0;
    }
  } else if (xTop == MAX_LED) {
    // above PANEL_RIGHT
    pPanel = getPanel(FRAME_NEXT, PANEL_RIGHT);
    pRow = pPanel->getRow(0);
    pRow->getLed(MAX_LED - yTop, pixel);
    if (yTop == 0) {
      // Also above PANEL_BACK
      pSecondPanel = getPanel(FRAME_NEXT, PANEL_BACK);
      secondColumn = 0;
    } else if (yTop == MAX_LED) {
      // Also above PANEL_FRONT
      pSecondPanel = getPanel(FRAME_NEXT, PANEL_FRONT);
      secondColumn = MAX_LED;
    }
  } else if (yTop == 0) {
    // above PANEL_BACK
    pPanel = getPanel(FRAME_NEXT, PANEL_BACK);
    pRow = pPanel->getRow(0);
    pRow->getLed(MAX_LED - xTop, pixel);
  } else if (yTop == MAX_LED) {
    // above PANEL_FRONT
    pPanel = getPanel(FRAME_NEXT, PANEL_FRONT);
    pRow = pPanel->getRow(0);
    pRow->getLed(xTop, pixel);
  }

  if (pSecondPanel != NULL) {
    Pixel otherPixel;
    pRow = pSecondPanel->getRow(0);
    pRow->getLed(secondColumn, otherPixel);
    averagePixels(pixel, otherPixel, DiscodelicGfx1.getTextBgColor());
  }
}

void Discodelic::swapBuffers(bool immediate) {
  if (immediate) {
    int oldLoopFrameNdx = loopFrameNdx;
    loopFrameNdx = animateFrameNdx;
    animateFrameNdx = oldLoopFrameNdx;
  } else {
    switchBuffers = true;
  }
}

/*
 * Diagnostic output.
 */
void Discodelic::dumpPanel(FrameId frameNdx, PanelId panelNdx) {
  Pixel pixel;
  Serial.print("dumpPanel [");
  Serial.print(pingPongStrings[frameNdx == FRAME_CURRENT ? 0 : 1]);
  Serial.print("][");
  Serial.print(panelStrings[panelNdx]);
  Serial.println("]");
  Panel *nextPanel = &panels[frameNdx == FRAME_CURRENT ? loopFrameNdx : animateFrameNdx][panelNdx];
  for (int rowNdx = 0; rowNdx < NUM_ROWS; ++rowNdx) {
    Vector *pNextRow = nextPanel->getRow(rowNdx);
    for (int ledNdx = 0; ledNdx < NUM_LEDS; ++ledNdx) {
      pNextRow->getLed(ledNdx, pixel);
      pixel.print();
    }
    Serial.println();
  }
}

void Discodelic::dumpAllPanels() {
  for (FrameId frameNdx = FRAME_CURRENT; frameNdx <= FRAME_NEXT; ++frameNdx) {
    for (PanelId panelNdx = PANEL_FIRST; panelNdx < NUM_PANELS; ++panelNdx) {
      dumpPanel(frameNdx, panelNdx);
    }
  }
}
