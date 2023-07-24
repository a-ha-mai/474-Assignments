#define LED_DDR   DDRL
#define LED_PORT  PORTL
#define LED_PIN     PL0 // pin 49

#define SPEAKER_PORT PORTH
#define SPEAKER_DDR  DDRH
#define SPEAKER_PIN  PH3 // pin 6

#define E 659
#define C 523
#define G 784
#define g 392
#define R 0

unsigned long timerCounter = 0; // Initialize the timerCounter
void setup() {
  Serial.begin(9600);
  // Set pins as outputs to corresponding DDR
  bit_set(LED_DDR, LED_PIN);
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  // Set Waveform Generation bits (WGM) to Fast PWM mode to timer 4
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
}

void loop() {
  while (1) {
    // Call the flashExternalLED + playSpeaker function
    flashExternalLED();
    playSpeaker();
    // Increment the timerCounter
    timerCounter++;
    // Delay for 1 second
    delay(1); // 1ms delay
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
    bit_set(LED_PORT, LED_PIN);
    
  } else {
    bit_clear(LED_PORT, LED_PIN);
  }
}

int songCycle() {
  int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
  int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 15};
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  static long currentTime = timerCounter;
  static int noteIndex;
  
  int noteDuration = 50 * beats[noteIndex];
  int freq = melody[noteIndex];
  int playTime;
  
  if (timerCounter - currentTime >= noteDuration) {
    noteIndex = (noteIndex + 1) % melodyLength;
    currentTime = timerCounter;
  }
  return freq;
}

/**
 * Function for playing the theme song on the speaker
 */
void playSpeaker() {
  // Implement the logic for playing the notes of the song based on the timerCounterSong
  int freq = songCycle();
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = (F_CPU / (64UL * freq)) / 2; //The value of OCR4A is set according to the frequency returned from speakerCycle()
  }
}

// set the bit to 1 given bit position and register name
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
// set the bit to 0 given bit position and register name
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

