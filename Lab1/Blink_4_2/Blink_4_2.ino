/* University of Washington 
* ECE 474, [June/25/2023] 
* 
* [Team Member Paria Naghavi] 
* 
* [Lab 1: Introduction to Arduino, Compilers and Development Environment (4.2)] 
* 
* Acknowledgments: [https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink] 
*/
//Purpose: Speaker clicks and external LED flashes at the same time, every 2ms. 
// Output pin definitions
const int externalLED = 10; 
const int speakerPin = 2;
// Timing variables
const int delayTime = 200; // pause the  for 200ms after toggling the state of the speaker and the external LED

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize both pin 2 and 10 as an output
  pinMode(speakerPin, OUTPUT);
  pinMode(externalLED, OUTPUT);
}

// the loop function runs over and over again forever untill the Arduino is powered 
void loop() {
  // Generate a click sound by quickly toggling the speaker on and off
  digitalWrite(speakerPin, HIGH); // turn the speaker on (HIGH is the voltage level)
  digitalWrite(externalLED, HIGH); // turn the off-board LED on 
  delayMicroseconds(delayTime); // halting the program for 200ms
  digitalWrite(speakerPin, LOW); // turn the speaker off by making the voltage LOW
  digitalWrite(externalLED, LOW); // turn the off-board LED off
  delay(delayTime); // delay to create pauses between clicks
}

