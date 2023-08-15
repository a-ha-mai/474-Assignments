// Task A: LED Sequence
#define DDR_L   DDRL
#define PORT_L  PORTL
#define PIN_47  0
#define PIN_48  1
#define PIN_49  2

// Task B: Timer tone output
#define SPEAKER_PIN PH3 // PH3
#define SPEAKER_PORT PORTH
#define SPEAKER_DDR DDRH

// Task C: Control the operation of Task A and Task B
#define TASK_A_DURATION 2000
#define TASK_B_DURATION 1000
#define TASK_C_DELAY 1000

void setup() {
  // Task A: Initialize LED pins as outputs
  DDR_L |= (1 << PIN_47) | (1 << PIN_48) | (1 << PIN_49);

  // Task B: Set Waveform Generation Mode (WGM) to Fast PWM mode (Mode 14)
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);

  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);

  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);

  // Task B: Configure speaker pin as an output
  bitSet(SPEAKER_DDR, SPEAKER_PIN);
}

void loop() {
  // Run Task C indefinitely
  taskC();
}

void taskA(unsigned long duration) {
  unsigned long startTime = millis();

  while (millis() - startTime < duration) {
  // Turn on LED 1  
    PORT_L |= (1 << PIN_47);
    delay(333);
  // Turn off LED 1    
    PORT_L &= ~(1 << PIN_47);
    delay(333);
  // Turn on LED 2
    PORT_L |= (1 << PIN_48);
    delay(333);
  // Turn off LED 2    
    PORT_L &= ~(1 << PIN_48);
    delay(333);
  // Turn on LED 3
    PORT_L |= (1 << PIN_49);
    delay(333);
  // Turn off LED 3    
    PORT_L &= ~(1 << PIN_49);
    delay(333);
  }
}

void taskB(unsigned long duration) {
  playTone(400, 1000);  // 400 Hz for 1 second
  playTone(250, 1000);  // 250 Hz for 1 second
  playTone(800, 1000);  // 800 Hz for 1 second
  delay(1000);
}

void taskC() {
  taskA(TASK_A_DURATION); // Task A for 2 seconds

  // Turn off LEDs before running Task B
  PORT_L &= ~((1 << PIN_47) | (1 << PIN_48) | (1 << PIN_49));

  taskB(TASK_B_DURATION); // Task B plays tone for 4 seconds

  delay(TASK_C_DELAY); // No outputs for 1 second
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
