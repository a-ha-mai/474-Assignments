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
  digitalWrite(LED_1_PIN, HIGH); // Pin 47 on for 333ms 
  delay(333);
  digitalWrite(LED_1_PIN, LOW);// Pin 47 on for 666ms 

  digitalWrite(LED_2_PIN, HIGH);// Pin 48 on for 333ms 
  delay(333);
  digitalWrite(LED_2_PIN, LOW);// Pin 48 on for 666ms 

  digitalWrite(LED_3_PIN, HIGH);// Pin 49 on for 333ms 
  delay(333);
  digitalWrite(LED_3_PIN, LOW);// Pin 49 on for 666ms 
}
