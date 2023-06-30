/* University of Washington 
* ECE 474, [June/25/2023] 
* 
* [Team Member Paria Naghavi] 
* 
* [Lab 1: Introduction to Arduino, Compilers and Development Environment (4.3)] 
* 
* Acknowledgments: [https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink] 
*/
// Purpose: The speaker clicks when the the external and on-board LED change states alternatively 
// Output pin definitions
const int onboardLED = 13; //The pin 13 corresponds to LED_BUILTIN. We could use that variable instead of 13
const int externalLED = 10; // Pin 10 is where the external LED is connected to the board 
const int speakerPin = 2; // Pin 2 is where the speaker is connected to the board 

// Timing variables
const int clickDuration = 200; // Duration of the speaker click
// const int clickDuration = 400; // Duration of the speaker click can be insreased so the transition is seen better 

// the setup function runs once when you press reset or power the board
// Providing output variables 
void setup() {
  pinMode(onboardLED, OUTPUT);
  pinMode(externalLED, OUTPUT);
  pinMode(speakerPin, OUTPUT);
}
// Loop to alternate the state of the on-board LED, the external LED,
// and producing click sounds with the speaker when the LEDs change state
void loop() {
  digitalWrite(onboardLED, HIGH); // Turn on the on-board LED
  digitalWrite(externalLED, LOW); // Turn off the external LED
  digitalWrite(speakerPin, HIGH); // Produce a click sound
  //Delays the program execution for the clickDuration variable.
  // This creates a delay for the click sound to be heard.
  delay(clickDuration); 

  // The reversed set up for the LEDs and the speaker 
  digitalWrite(onboardLED, LOW); // Turn off the on-board LED
  digitalWrite(externalLED, HIGH); // Turn on the external LED
  digitalWrite(speakerPin, LOW); // Produce a click sound

  delay(clickDuration); //This creates a delay between LED state changes.
}
