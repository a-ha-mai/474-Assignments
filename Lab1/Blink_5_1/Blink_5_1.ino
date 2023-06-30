/* University of Washington
 * ECE 474, [June/25/2023]
 *
 * [Team Member Paria Naghavi]
 *
 * [Lab 1: Introduction to Arduino, Compilers and Development Environment (5.1)]
 *
 * Acknowledgments: [https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink] and ChatGPT
 */
// Purpose: The speaker produces a 250Hz tone as the external and on-board LED flash alternatively 
// Output pin definitions
const int speakerPin = 2;
const int externalLED = 10;

int frequency = 250; // 250 Hz tone
float period = 1.0 / frequency; // Period for 250 Hz
bool LEDState = false; // This variable will control the LED state
unsigned long previousMillis = 0; // set the timing variable 
unsigned long speakerMillis = 0; // set the timing variable 
const long LEDinterval = 200; // interval for LED state change (200ms)
const long speakerInterval = 3000; // interval for speaker to turn off (3 seconds)
// set the output pins for the speaket, external LED and on-board LED
void setup() {
  pinMode(speakerPin, OUTPUT);
  pinMode(externalLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize previousMillis to a time earlier than the current time
  previousMillis = millis() - LEDinterval;
}

void loop() {
  // timer variable is initilized 
  unsigned long currentMillis = millis();

  // Check if 200 ms has passed since the last LED state change
  if (currentMillis - previousMillis >= LEDinterval) {
    previousMillis = currentMillis;

    LEDState = !LEDState; // flip the state of the LED
    digitalWrite(externalLED, LEDState); // set the LED pin to the current state
    digitalWrite(LED_BUILTIN, !LEDState); // set the built-in LED to the opposite state
  }

  // Check if 3 seconds have passed since the speaker turned on
  if (currentMillis - speakerMillis < speakerInterval) {
    // Generate a 250Hz tone on the speaker continuously
    unsigned long elapsedMillis = currentMillis - speakerMillis;
    if (elapsedMillis % 4 < 2) { // obtain the reminder of elapsed time by period to compare it to half the period 
      digitalWrite(speakerPin, HIGH); // turn the speaker on (HIGH is the voltage level)
    } else {
      digitalWrite(speakerPin, LOW); // turn the speaker off by making the voltage LOW
    }
  } else {
    digitalWrite(speakerPin, LOW); // turn the speaker off after 3 seconds
  }
}
