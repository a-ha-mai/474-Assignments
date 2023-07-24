// Define the register macros for DDR and PORT of Port L
#define DDR_L   DDRL
#define PORT_L  PORTL

// Define the bit number for pin 49 (PL0)
#define PIN_49  0

int timerCounter = 0; // Initialize the timerCounter
void setup() {
  // Set pin 49 as an output by setting the corresponding DDR bit
  bit_set(DDR_L, PIN_49);
}

void loop() {
  while (1) {
    // Call the flashExternalLED function
    flashExternalLED();
    // Increment the timerCounter
    timerCounter++;
    // Delay for 1 second
    delay(1); // 10ms delay
  }
}

// Function for flashing the external LED
void flashExternalLED() {
  const int onInterval = 250;
  int interval = 1000;   // Total interval (1s)
  //const int offInterval = 750;
  //int interval = onInterval + offInterval;
// The expression timerCounter % interval calculates the remainder of the division of timerCounter by interval. 
// This value will cycle from 0 to (interval - 1) and then back to 0.
  if (timerCounter % interval <= onInterval) {
    bit_set(PORT_L, PIN_49);
    
  } else {
    bit_clear(PORT_L, PIN_49);
  }
  //timerCounter++;
}
// set the bit to 1 given bit position and register name
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
// set the bit to 0 given bit position and register name
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

