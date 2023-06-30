void setup() {
  pinMode(10, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(10, HIGH);
  delay(200);
  digitalWrite(10, LOW);
  delay(200);
}
