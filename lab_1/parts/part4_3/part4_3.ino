void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(10, HIGH);
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(2, LOW);
  delay(200);
}
