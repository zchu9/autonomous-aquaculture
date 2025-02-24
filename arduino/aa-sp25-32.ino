void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200, SERIAL_8N1);
}

void loop() {
  // put your main code here, to run repeatedly:
  String data = "\r\nV\tThis is a sample message.";
  int count = 0;
  while (Serial.available()) {
    for (int i = 0; i < data.length(); i++) {
      Serial.write(data[i]);
    }
    count++;
  }
}
