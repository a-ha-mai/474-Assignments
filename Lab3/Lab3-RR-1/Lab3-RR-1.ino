/**
 * @file Lab3_DDS-1.ino
 * @author Anna Mai (2165101)
 * @author Paria Naghavi (1441396)
 * @date 28-July-2023
 * @brief UW ECE 474 Lab 3 Assignment
 * 
 * A Round Robin Scheduler that runs 2 tasks simultaneously:
 * - Flashing an LED on for 250ms and off for 750ms
 * - Playing the intro to the Super Mario Bros. theme song repeatedly, with 4 seconds of silence between each play
 */

/**
 * @def LED_DDR
 * @brief This macro defines the DDR corresponding to the LED pin.
 */
#define LED_DDR   DDRL
/**
 * @def LED_PORT
 * @brief This macro defines the Port Register corresponding to the LED pin.
 */
#define LED_PORT  PORTL
/**
 * @def LED_PIN
 * @brief The pin number (49) the LED is connected to as represented by the Arduino hardware.
 */
#define LED_PIN   PL0 // pin 49


/**
 * @def SPEAKER_DDR
 * @brief This macro defines the DDR corresponding to the Speaker pin.
 */
#define SPEAKER_DDR  DDRH
/**
 * @def SPEAKER_DDR
 * @brief This macro defines the Port Register corresponding to the Speaker pin.
 */
#define SPEAKER_PORT PORTH
/**
 * @def SPEAKER_DDR
 * @brief The pin number (6) the LED is connected to as represented by the Arduino hardware.
 */
#define SPEAKER_PIN  PH3 // pin 6

/**
 * @def E
 * @brief The frequency (in Hz) to play for the note E5.
 */
#define E 659
/**
 * @def C
 * @brief The frequency (in Hz) to play for the note C5.
 */
#define C 523
/**
 * @def G
 * @brief The frequency (in Hz) to play for the note G5.
 */
#define G 784
/**
 * @def g
 * @brief The frequency (in Hz) to play for the note G4.
 */
#define g 392
/**
 * @def R
 * @brief The frequency (in Hz) to play during a rest.
 */
#define R 0

/**
 * @brief Timer Counter.
 * 
 * Stores how long (in ms) it has been since the program started running.
 * Is used to implement time-based functionalities in the program.
 */
unsigned long timerCounter = 0; // Initialize the timerCounter

/**
 * @brief Initializes pins and timers during setup.
 */
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

/**
 * @brief Controls the execution of tasks by going through them in order repeatedly.
 */
void loop() {
  while (1) {
    // Call the flashExternalLED + playSpeaker function
    flashExternalLED();
    playSpeaker();
    // Increment the timerCounter
    timerCounter++;
    delay(1); // 1ms delay
  }
}

/**
 * @brief Flashes the LED connected to pin 49 on for 250ms and off for 750ms
 */
void flashExternalLED() {
  const int onInterval = 250;
  int interval = 1000;   // Total interval (1s)
  
  // Calculate the remainder of the division of timerCounter by interval to determine when to turn the LED on or off.
  if (timerCounter % interval <= onInterval) {
    bit_set(LED_PORT, LED_PIN); // Turn the LED on
  } else {
    bit_clear(LED_PORT, LED_PIN); // Turn the LED off
  }
}

/**
 * @brief Cycles through a melody array and returns the next frequency to play.
 * @return The frequency (in Hz) of the next note to be played in the melody.
 */
int songCycle() {
  // Melody and beats arrays represent the notes and their durations
  int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
  int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 80};
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  
  // Static variables for keeping track of the current note and time since a note started playing
  static long currentTime = timerCounter;
  static int noteIndex;
  
  // Calculate the duration of the current note in milliseconds
  int noteDuration = 50 * beats[noteIndex];
  
  // Retrieve the frequency of the next note to be played
  int freq = melody[noteIndex];
  
  // Play the note for its specified duration
  // If the elapsed time since the last note is greater than or equal to the duration,
  // move to the next note and update the current time
  if (timerCounter - currentTime >= noteDuration) {
    noteIndex = (noteIndex + 1) % melodyLength;
    currentTime = timerCounter;
  }
  return freq; // Return the frequency of the next note to be played
}

/**
 * @brief Plays a theme from a song and introduces a sleep interval of 4s between plays.
 */
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

/**
 * @brief Set a bit to 1 in the given register at the specified bit position.
 * @param[in, out] reg The register in which the bit is to be set to 1.
 * @param[in] bit The bit position (0 to 7) to set to 1 in the register.
 */ 
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

/**
 * @brief Set a bit to 0 in the given register at the specified bit position.
 * @param[in, out] reg The register in which the bit is to be set to 0.
 * @param[in] bit The bit position (0 to 7) to set to 0 in the register.
 */
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
