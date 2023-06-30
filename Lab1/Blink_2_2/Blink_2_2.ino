/* University of Washington 
* ECE 474, [June/25/2023] 
* 
* [Team Member Paria Naghavi] 
* 
* [Lab 1: Introduction to Arduino, Compilers and Development Environment (2.2)] 
* 
* Acknowledgments: [https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink] 
*/
//Purpose: Flashes the on-board LED every 2ms 
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // Specifying that the LED_BUILTIN pin is the output pin.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(200);                      // wait for 200ms-- halts the program for 200 ms
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(200);                      // wait for 200 ms-- halts the program for 200 ms
}
