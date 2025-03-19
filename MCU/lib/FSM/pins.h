#define MUX_SEL_0 0
#define MUX_SEL_1 1
#define MUX_SEL_2 2

#define MUX_DISABLE_0 3    // RX
#define MUX_DISABLE_1 4    // TX
#define MUX_DISABLE_2 5    // Winch

#define WINCH_ACTIVATE 7    // change this; this is shared with the SPI CS line. Avoid pin 6; this is shared with the onboard LED and is used during flashing.