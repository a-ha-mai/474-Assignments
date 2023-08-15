#define SPEAKER_PIN PH3 // PH3 is pin 6 on the device 

#define SPEAKER_PORT PORTH
#define SPEAKER_DDR DDRH

void setup() {
  // Set up the timer's Waveform Generation Mode (WGM) to Fast PWM mode (Mode 14)
  // WGM41, WGM42, and WGM43 are bit positions that control the Waveform Generation Mode for Timer/Counter4. 
  // Each bit corresponds to a specific mode of operation for Timer/Counter4. By setting these bits to either 0 or 1, you can select the desired mode.*/
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // When set to 1, enables the toggling of the output on timer based on a compare match event.
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
  
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
}

void loop() {
  playTone(400, 1000);  // 400 Hz for 1 second
  playTone(250, 1000);  // 250 Hz for 1 second
  playTone(800, 1000);  // 800 Hz for 1 second
  delay(1000);

}
//set a specific bit in a register
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
//clears a specific bit in a register by setting it to 0.
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
//generate a square wave tone of a specific frequency for a given duration.
//F_CPU represents the CPU clock frequency- specifies the operating frequency- if F_CPU is defined as 16 MHz, it means the CPU is running at a frequency of 16 million cycles per second.
//64UL is the prescaler value. It represents a division factor applied to the CPU clock frequency to determine the timer's input clock frequency. In this case, 64UL represents a prescaler value of 64.
//determine the number of clock cycles required for each cycle of the waveform based on the CPU clock frequency
void playTone(unsigned int freq, unsigned long duration) {
  unsigned long period = F_CPU / (64UL * freq); // The division by 64 is based on the prescaler value used in the timer configuration.
  unsigned long startTime = millis();
//generate the tone for the specified duration  
  while (millis() - startTime < duration) {
    // Set the compare match value for half the period///;;;;;;;;;;;;;
    OCR4A = period / 2;
    delayMicroseconds(period);
  }
  
  // Turn off the speaker by setting the compare match value to 0
  OCR4A = 0;
}
//cpu clock speed- how quicly the processor can process- in assembely- x=b+5
// cpu is set to all instruction take the same time
//clock speed in consistenet- how fast - prescalr
