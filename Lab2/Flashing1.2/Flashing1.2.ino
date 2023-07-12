// Define the LED pin numbers
const int LED_1_PIN = 47;
const int LED_2_PIN = 48;
const int LED_3_PIN = 49;

void setup() {
  // Initialize the LED pins as outputs
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
}

void loop() {
  // Flash the LEDs in a sequential pattern
  digitalWrite(LED_1_PIN, HIGH);
  delay(333);
  digitalWrite(LED_1_PIN, LOW);

  digitalWrite(LED_2_PIN, HIGH);
  delay(333);
  digitalWrite(LED_2_PIN, LOW);

  digitalWrite(LED_3_PIN, HIGH);
  delay(333);
  digitalWrite(LED_3_PIN, LOW);
}
