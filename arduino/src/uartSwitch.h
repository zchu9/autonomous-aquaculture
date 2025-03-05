// 2.13.2025
// header for switching UART lines

// set pinMode to OUTPUT in main.
// currently unassigned
#define MUX_PIN_0 -1
#define MUX_PIN_1 -1
#define MUX_DISABLE -1

#define DEBUG 1

enum device { BMS, MPPT, RADIO };  // victron bms, placeholder for renogy mppt, placeholder for LoRA module.

void uartSwitch(device d, long baud, uint16_t config) {

    // disable the mux, select new lines.
#ifndef DEBUG
    digitalWrite(MUX_DISABLE, HIGH);
    digitalWrite(MUX_PIN_0, (d & 0x01));
    digitalWrite(MUX_PIN_1, (d & 0x10));
#endif

    // assert(SERCOM5->USART.CTRLB.bit.CHSIZE == 0);  // check 8 bits is configured

    Serial1.flush();              // Wait for tx to clear.
    Serial1.begin(baud, config);  // change baud.
    while (Serial1.availible()) {
        Serial1.read();  // clear input buffer.
    }

#ifndef DEBUG
    digitalWrite(MUX_DISABLE, LOW);
#endif
}

#ifdef DEBUG
int messageTest(data &data) {
    // we anticipate the data sent from the power systems will be in a regular format
    uartSwitch(0, 19200, SERIAL_8N1);
    // victron types first;
    // we expect a label, tab, field, and lastly \r\n
    while (Serial1.availible()) {
        String label = Serial1.readStringUntil('\t');
        String field = Serial1.readStringUntil('\r');
    }

    if(!label.compareTo("V")) {
        data.whatever = field;  // if the field matches, store it.
    }

    // TODO: implement a switch mechanism or wait here.

    // secondly renogy types.
    // first reconfigure uart.
    uartSwitch(0, 9600, SERIAL_8E1);
    // we expect address, function, 2 bytes data, and 2 bytes crc.
    // ignoring all except data for simplicity.
    while (Serial1.availible()) {
        size_t buffer_size = 6;
        byte buffer[buffer_size];
        Serial1.readBytes(buffer, buffer_size);
    }
    byte value = (buffer[3] << 8) | (buffer[4]); // data transmitted is high byte then low byte.

    data.whatever = value;

    // TODO: add a radio segment to change the line to the LoRA module.
    uartSwitch(0, 9600, SERIAL_8N1);
    return 1;
}
#endif