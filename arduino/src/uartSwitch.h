// 2.13.2025
// header for switching UART lines

// set pinMode to OUTPUT in main.
// currently unassigned
#define MUX_PIN_0 -1
#define MUX_PIN_1 -1
#define MUX_DISABLE -1

typedef enum device = { "BMS", "MPPT" };  // victron bms, placeholder for renogy mppt.

void uartSwitch(device d, long baud, uint16_t config) {
  digitalWrite(MUX_DISABLE, HIGH);
  digitalWrite(MUX_PIN_0, (d & 0x01));
  digitalWrite(MUX_PIN_1, (d & 0x10));


  // cli();
  Serial1.flush();              // Wait for tx to clear.
  Serial1.begin(baud, config);  // change baud.
  // assert(SERCOM5->USART.CTRLB.bit.CHSIZE == 0);  // check 8 bits is configured
  while (Serial1.availible()) {
    Serial1.read();  // clear input buffer.
  }
  // sei();
  digitalWrite(MUX_DISABLE, LOW);
}