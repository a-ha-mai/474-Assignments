// Define the register macros for DDR and PORT of Port L
// DDR_L represents the Data Direction Register of Port L, which controls the direction (input or output) of each pin in Port L
// PORT_L represents the Output Register of Port L, which is used to set the output value (high or low) of each pin in Port L
#define DDR_L   DDRL
#define PORT_L  PORTL

// Define the bit numbers for pins 47, 48, and 49
// PIN_47 represents the bit number for pin 47 in Port L
// PIN_48 represents the bit number for pin 48 in Port L
// PIN_49 represents the bit number for pin 49 in Port L
#define PIN_47  0
#define PIN_48  1
#define PIN_49  2

void setup() {
  // Set the pins as outputs by setting the corresponding DDR bits
  // |= performs a bitwise OR operation and assigns the result back to DDR_L
  // (1 << PIN_47) sets the bit at the position of PIN_47 to 1, representing pin 47 as an output
  // (1 << PIN_48) sets the bit at the position of PIN_48 to 1, representing pin 48 as an output
  // (1 << PIN_49) sets the bit at the position of PIN_49 to 1, representing pin 49 as an output
  DDR_L |= (1 << PIN_47) | (1 << PIN_48) | (1 << PIN_49);
}

void loop() {
  // Turn on LED 47
  // |= performs a bitwise OR operation and assigns the result back to PORT_L
  // (1 << PIN_47) sets the bit at the position of PIN_47 to 1, turning on LED 47
  PORT_L |= (1 << PIN_47);
  delay(333); // Delay for 333 milliseconds

  // Turn off LED 47 and turn on LED 48
  // &= performs a bitwise AND operation and assigns the result back to PORT_L
  // ~(1 << PIN_47) sets the bit at the position of PIN_47 to 0, turning off LED 47
  // (1 << PIN_48) sets the bit at the position of PIN_48 to 1, turning on LED 48
  PORT_L &= ~(1 << PIN_47);
  PORT_L |= (1 << PIN_48);
  delay(333); // Delay for 333 milliseconds

  // Turn off LED 48 and turn on LED 49
  // &= performs a bitwise AND operation and assigns the result back to PORT_L
  // ~(1 << PIN_48) sets the bit at the position of PIN_48 to 0, turning off LED 48
  // (1 << PIN_49) sets the bit at the position of PIN_49 to 1, turning on LED 49
  PORT_L &= ~(1 << PIN_48);
  PORT_L |= (1 << PIN_49);
  delay(333); // Delay for 333 milliseconds

  // Turn off LED 49
  // &= performs a bitwise AND operation and assigns the result back to PORT_L
  // ~(1 << PIN_49) sets the bit at the position of PIN_49 to 0, turning off LED 49
  PORT_L &= ~(1 << PIN_49);
  delay(333); // Delay for 333 milliseconds
}

// The code sets the pins 47, 48, and 49 of Port L as outputs using DDR_L.
// It then loops through a sequence of turning on and off LEDs connected to those pins in a sequential pattern,
// with each LED turned on for 333 milliseconds and then turned off.