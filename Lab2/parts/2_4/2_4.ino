#define SPEAKER_PIN PH3 // PH3

#define SPEAKER_PORT PORTH
#define SPEAKER_DDR DDRH

void setup() {
  // Set Waveform Generation Mode (WGM) to Fast PWM mode (Mode 14)
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
  
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
}

void loop() {
  playTone(400, 1000);  // 400 Hz for 1 second
  playTone(250, 1000);
  playTone(800, 1000);
  delay(1000);
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

void playTone(unsigned int freq, unsigned long duration) {
  unsigned long period = F_CPU / (64UL * freq); // Calculate the period
  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    // Set the compare match value for half the period
    OCR4A = period / 2;
    delayMicroseconds(period);
  }
  
  // Turn off the speaker by setting the compare match value to 0
  OCR4A = 0;
}

