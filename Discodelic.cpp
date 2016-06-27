#include "DiscodelicLib.h"

// Singleton
Discodelic Discodelic1;

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
volatile uint8_t loopFrameNdx;
volatile uint8_t animateFrameNdx;
volatile bool switchBuffers = false;

const char * pingPongStrings[] = {
  "PING",
  "PONG"
};

const char * panelStrings[] = {
  "PANEL_BACK",
  "PANEL_TOP",
  "PANEL_LEFT",
  "PANEL_FRONT",
  "PANEL_RIGHT"
};

void Discodelic::setup () {
	
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
        if (((rowNdx == 0) && (ledNdx == 0)) || ((rowNdx == 7) && (ledNdx == 7))) {
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


static uint8_t refreshNdx;
static uint8_t rowNdx;

#define NUM_REFRESHES (1 << NUM_DIM_BITS)

#if (NUM_DIM_BITS == 4)

//           cycle
//val   0123 4567 89AB CDEF
//0000  0000 0000 0000 0000
//0001  1000 0000 0000 0000
//0010  1000 0000 1000 0000
//0011  1000 0010 0001 0000
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

const uint16_t dimmingSchedule[16] = {
  0x0000, 0x8000, 0x8080, 0x8210,
  0x8888, 0x8924, 0x9292, 0x954a,
  0x6ab5, 0x6d6d, 0x76db, 0x7777,
  0x7def, 0x7f7f, 0x7fff, 0xffff};

#elif (NUM_DIM_BITS == 2)
//           cycle
//val   0123
//0000  0000
//0001  1000
//0010  1010
//0011  1111

const uint16_t dimmingSchedule[4] = {
  0x0, 0x8, 0xa, 0xf};
#endif


void Discodelic::refresh(void) {
  if (++rowNdx >= NUM_ROWS) {
    rowNdx = 0;
    if (++refreshNdx >= NUM_REFRESHES) {
      refreshNdx = 0;

      if (switchBuffers) {
        int oldLoopFrameNdx = loopFrameNdx;
        loopFrameNdx = animateFrameNdx;
        animateFrameNdx = oldLoopFrameNdx;
        switchBuffers = false;
      }
    }
  }

  uint8_t frameNdx = loopFrameNdx;
  // Clock out one entire row
  const int cycleBit = 1 << refreshNdx;
  
  for (int panelNdx = PANEL_FIRST; panelNdx < NUM_PANELS; ++panelNdx) {
    Panel *panel = &panels[frameNdx][panelNdx];
    // Clock out the row starting at the far end
    Vector *pRow = panel->getShiftRow(rowNdx);

    for (int color = FIRST_COLOR; color < NUM_COLORS; ++color) {
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


void Discodelic::dumpPanel(FrameId frameNdx, PanelId panelNdx) {
  Pixel pixel;
  Serial.print("dumpPanel [");
  Serial.print(pingPongStrings[frameNdx]);
  Serial.print("][");
  Serial.print(panelStrings[panelNdx]);
  Serial.println("]");
  Panel *nextPanel = &panels[frameNdx][panelNdx];
  for (int rowNdx = 0; rowNdx < NUM_ROWS; ++rowNdx) {
    Vector *pNextRow = nextPanel->getRow(rowNdx);
//    Serial.println((int)pNextRow, HEX);
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

Panel *Discodelic::getPanel(FrameId frameNdx, PanelId panelNdx) {
  return &panels[frameNdx == FRAME_CURRENT ? loopFrameNdx : animateFrameNdx][panelNdx];
}

void Discodelic::swapBuffers(void) {
  switchBuffers = true;
}
