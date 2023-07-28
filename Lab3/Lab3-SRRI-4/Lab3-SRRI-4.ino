/**
 * @file Lab3_DDS-4.ino
 * @author Anna Mai (2165101)
 * @author Paria Naghavi (1441396)
 * @date 28-July-2023
 * @brief UW ECE 474 Lab 3 Assignment
 * 
 * A Synchronized Round Robin Scheduler that runs 3 tasks simultaneously:
 * - Flashing an LED on for 250ms and off for 750ms
 * - Playing the intro to the Super Mario Bros. theme song repeatedly, with 4 seconds of silence between each play
 * - Counting up by 1 every 100ms on the 7-segment display
 * 
 * Also includes an Interupt Service Routine that runs every 2ms.
 * - Every 100 ISR runs, an LED connected to pin 48 will toggle
 */

#include "SevSeg.h" // Offers basic macros for streamlined usage of the seven segment display
SevSeg sevseg; //Instantiate a seven segment controller object

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
 * @def LED_PIN
 * @brief The pin number (48) the LED is connected to as represented by the Arduino hardware.
 */
#define INTERRUPT_LED_PIN PL1 // pin 48

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
 * @def MAX_SIZE
 * @brief The size of array to initialize for the scheduler's task list.
 */
#define MAX_SIZE 10

// Declare function prototypes
void flashExternalLED();
void playSpeaker(); 
void updateDisplay();
void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);
void sleep_474(int t);
void schedule_sync();

/**
 * @enum TaskState
 * @brief The various states a task can be in.
 */
enum TaskState {
  READY,
  RUNNING,
  SLEEPING
};

/**
 * @enum TaskState
 * @brief The various states a flag can be in.
 */
enum FlagState {
  PENDING,
  DONE
};

/**
 * @brief Array of function pointers to tasks to be executed.
 */
void (*taskScheduler[MAX_SIZE])() = {flashExternalLED, playSpeaker, updateDisplay, schedule_sync}; // Adjust the array size according to the number of tasks
/**
 * @brief Array to keep track of task states.
 */
enum TaskState taskStates[MAX_SIZE] = {READY, READY, READY, READY}; // Initialize the states for each task
/**
 * @brief Array to store sleep duration for each task.
 */
int taskSleep[MAX_SIZE] = {0, 0, 0, 0};
/**
 * @brief Array to store the start time of sleep for each task.
 */
unsigned long taskSleepStartTime[MAX_SIZE] = {0, 0, 0, 0};
/**
 * @brief Timer Counter.
 * 
 * Stores how long (in ms) it has been since the program started running.
 * Is used to implement time-based functionalities in the program.
 */
unsigned long timerCounter = 0;
/**
 * @brief Current Task Identifier.
 * 
 * Represents the index of what task is being run in the scheduler.
 */
int currentTask = 0;
/**
 * @brief Flag to signal when an ISR is completed.
 */
volatile int sFlag = READY;
/**
 * @brief A counter that tracks how many times the ISR has run since the last interrupt LED toggle.
 */
volatile long isrCounter = 0;
/**
 * @brief How many times the ISR should run before the interrupt LED gets toggled.
 */
const uint16_t toggleThreshold = 100;
/**
 * @brief Variable that is incremented by 1 in each iteration where the program waits until sFlag changes from PENDING.
 */
int x = 0;

/**
 * @brief Initializes pins, timers, and interrupts during setup.
 */
void setup() {
  // Configuring settings of the seven segment display to match with the hardware.
  byte numDigits = 4;
  byte digitPins[] = {22, 23, 24, 25};
  byte segmentPins[] = {26, 27, 28, 29, 30, 31, 32, 33};
  bool resistorsOnSegments = false; // resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = false;
  bool leadingZeros = false; // no leading zeroes
  bool disableDecPoint = true; // decimal point not used
  
  // Set pins as outputs to corresponding DDR
  bit_set(LED_DDR, LED_PIN);
  bit_set(LED_DDR, INTERRUPT_LED_PIN);
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  
  // Set Waveform Generation bits (WGM) to Fast PWM mode to timer 4
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
  
  // Set the timer mode and prescaler to achieve 2ms interval for ISR
  TCCR3A = 0; // Normal operation
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30); // CTC mode, prescaler 64
  OCR3A = 500; 
  
  // Enable TIMER3 compare match A interrupt
  TIMSK3 |= (1 << OCIE3A);
  
  // Initialize the seven-segment display using the SevSeg library
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(100);
    
  // Enable global interrupts
  sei();
}

/**
 * @brief Controls the execution of tasks based on the process list and scheduler.
 */
void loop() {
  // Enter the loop when a task is present and not null
  while (taskScheduler[currentTask] != NULL) {
    // Run the task if it is not in the SLEEPING state
    if (taskStates[currentTask] != SLEEPING) {
      taskScheduler[currentTask]();
      // Change the task state to RUNNING
      taskStates[currentTask] = RUNNING;
    }
    // Increment the currentTask and timerCounter
    currentTask = (currentTask + 1) % 10;
    timerCounter++;
    // Delay for 1ms
    delay(1);
  }
  // Reset the currentTask to 0
  currentTask = 0;
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
    noteIndex++;
    currentTime = timerCounter;
  }
  
  // Check if all notes in the melody have been played
  if (noteIndex >= melodyLength) {
    // If the end of the melody is reached, reset the noteIndex to 0 to start over
    noteIndex = 0;
    // depreciated use of sleep_474 because we found out from TAs that you can hard code the 4s downtime
    // sleep_474(4000); // 4-second sleep between song cycles
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
 * @brief Updates the content displayed on the seven-segment display.
 */
void updateDisplay() {
  int deciSecondsElapsed = (timerCounter / 100) % 1000; // Calculate elapsed time in deciseconds
  sevseg.setNumber(deciSecondsElapsed, 0); // Set the number to be displayed on the seven-segment display
  sevseg.refreshDisplay(); // Update the content on the seven-segment display
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

/**
 * @brief Puts the current task to sleep for a specified duration.
 *
 * This function is currently not being used, as the TAs have said that we can hard code in a 4s sleep in playSpeaker()
 *
 * @param[in] t The duration (in some unit) for which the task should sleep.
 */
void sleep_474(int t) {
  taskStates[currentTask] = SLEEPING; // Set the task state to SLEEPING
  taskSleepStartTime[currentTask] = timerCounter; // Record the current time as the start time of sleep
  taskSleep[currentTask] = t; // Set the sleep duration for the current task
}

/**
 * @brief Interrupt Service Routine for TIMER3_COMPA vector.
 */
ISR(TIMER3_COMPA_vect) {
  isrCounter++; // Increment the ISR counter
  if (isrCounter >= toggleThreshold) {
    LED_PORT ^= (1 << INTERRUPT_LED_PIN); // Toggle the interrupt indivator LED
    isrCounter = 0; // Reset the counter after LED toggle
  }
  sFlag = DONE; // Set the sFlag to DONE to signal task completion
}

/**
 * @brief Synchronizes task scheduling and state transitions.
 */
void schedule_sync() {
  while (sFlag == PENDING) {
    x += 1; // Increment the variable x while waiting for sFlag change
  }
  unsigned long currentTime = timerCounter; // Get the current time
  // This current conditional is not being used as the 4s sleep for playSpeaker() has been hardcoded in.
  if (taskStates[currentTask] == SLEEPING) {
    unsigned long elapsedTime = currentTime - taskSleepStartTime[currentTask];
    if (elapsedTime >= taskSleep[currentTask]) {
      // If the sleep duration has passed, set the task state to READY
      taskStates[currentTask] = READY;
    }
  } else {
    taskStates[currentTask] = READY; // Set the task state to READY
  } 
  sFlag = PENDING; // Reset sFlag to PENDING for the next synchronization iteration
}