#define LED_PIN_1 47
#define LED_PIN_2 48
#define LED_PIN_3 49

void setup() {
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_3, LOW);
  delay(333);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, HIGH);
  delay(333);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, HIGH);
  delay(333);
}