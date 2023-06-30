/* University of Washington 
* ECE 474, [June/25/2023] 
* 
* [Team Member Paria Naghavi] 
* 
* [Lab 1: Introduction to Arduino, Compilers and Development Environment (3.3)] 
* 
* Acknowledgments: [https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink] 
*/
//Purpose: Flashes the external LED every 2ms 

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // Specifying that the LED_BUILTIN pin is the output pin.
  pinMode(10, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(10, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(200);                      // wait for a second-- halts the program for 200 ms
  digitalWrite(10, LOW);   // turn the LED off by making the voltage LOW
  delay(200);                      // wait for a second-- halts the program for 200 ms
}
