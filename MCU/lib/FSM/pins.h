/**
 * @brief Defines the usage of Arduino pins
 *
 */
// = = = = = = = = = = = = = =
// DIGITAL PINS
// = = = = = = = = = = = = = =

// output is equvalent to the binary value; ie output 5 = 0b101.
#define MUX_SEL_0 1
#define MUX_SEL_1 0
#define MUX_SEL_2 A3 // previously competed with temp sensors; not applicable to the prototype.

// DS18B20 OneWire data pin
#define ONE_WIRE_BUS 2

// mux enable-n pins; HIGH means off.
#define MUX_DISABLE_0 3 // RX mux
#define MUX_DISABLE_1 4 // TX mux
#define MUX_DISABLE_2 5 // Winch mux

// Camera Chip Select
#define CS_PIN 7

// PINS OVER 7 are reserved for SPI, I2C and UART respectively.
#define RX_INTERRUPT 5
#define RX_PIN 13

// = = = = = = = = = = = = = =
//  ANALOG PINS
// = = = = = = = = = = = = = =

// Potentiometer analog input
#define POT_PIN A0

// driving pins for the winch relays.
#define LIFT_PIN A1
#define LOWER_PIN A2