// LED variables 
#define LED_DDR   DDRL
#define LED_PORT  PORTL
#define LED_PIN     PL0 // pin 49
// Speaker variables 
#define SPEAKER_PORT PORTH
#define SPEAKER_DDR  DDRH
#define SPEAKER_PIN  PH3 // pin 6
// Musical notes and their  frequencies
#define E 659
#define C 523
#define G 784
#define g 392
#define R 0

unsigned long timerCounter = 0; // Initialize the timerCounter
void setup() {
  // Set LED pins as outputs on the corresponding DDR
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
// this will always run   
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
// This value will cycle from 0 to (interval - 1) and then back to 0 and set LED pin bits to 1 on the LED port
  if (timerCounter % interval <= onInterval) {
    bit_set(LED_PORT, LED_PIN);
   // when greater than onInterval, the LED pin are set to 0  
  } else {
    bit_clear(LED_PORT, LED_PIN);
  }
}
// playing a sequence of musical notes and uses the timerCounter variable to keep track of the time elapsed since the last note was played
int songCycle() {
  int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
  int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 80};
  // divide the total size of the array by the size of a single element gives the number of elements in the array
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  //keeps track of the time elapsed since the last note was played
  static long currentTime = timerCounter;
  //keeps track of the index of the current note in the melody 
  static int noteIndex;
  // calculates the duration of the current note in milliseconds.
  // It multiplies it by 50 as a scaling factor to convert beats to milliseconds. 
  // how long the current note should be played.
  int noteDuration = 50 * beats[noteIndex];
  // extracts frequency for the same noteIndex
  int freq = melody[noteIndex];

 // When the elapsed time  of the current note is greater than or equal to the noteDuration, it must end. 
  if (timerCounter - currentTime >= noteDuration) {
  // increment leads to playing continuously.  
    noteIndex = noteIndex++;
  //keep track of the starting time of the current note (new) 
    currentTime = timerCounter;
  }

  //set the tone for playing the next melody.
  return freq;
}


 // Function for playing the theme song on the speaker
// Implement the logic for playing the notes of the song based on the timerCounterSong
void playSpeaker() {
  // extract freq from songCycle
  int freq = songCycle();
  //Check if the frequency is 0 (rest) or a valid musical note
  if (freq == 0) {
  // If the frequency is 0 (rest), set OCR4A to 0 to produce silence (no sound)
    OCR4A = 0;
  } else {
  // If the frequency is a valid musical note, calculates the value of OCR4A  based on the frequency. 
  // OCR4A controls the output frequency of the speaker 

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
