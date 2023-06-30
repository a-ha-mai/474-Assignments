/* University of Washington 
* ECE 474, [June/25/2023] 
* 
* [Team Member: Paria Naghavi] 
* 
* [Lab 1: Introduction to Arduino, Compilers and Development Environment (1.8)] 
* 
* Acknowledgments: The code from the examples was modified using [https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink] 
*/

// Purpose: Flashes the on-board LED every 2 seconds
// the setup function runs once when you press reset or power the board
void setup() {

  // Specifying that the LED_BUILTIN pin is the output pin.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(2000);                      // wait for two second-- halts the program for 2 seconds 
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(2000);                      // wait for two second-- halts the program for 2 seconds 
}
