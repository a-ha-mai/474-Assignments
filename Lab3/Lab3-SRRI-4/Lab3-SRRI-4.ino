#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object
// Define pins and timers for LED and speaker
#define LED_DDR   DDRL
#define LED_PORT  PORTL
#define LED_PIN   PL0 // pin 49

#define SPEAKER_PORT PORTH
#define SPEAKER_DDR  DDRH
#define SPEAKER_PIN  PH3 // pin 6

#define INTERRUPT_LED_PIN PL1 // pin 48

// Define note frequencies
#define E 659
#define C 523
#define G 784
#define g 392
#define R 0

#define MAX_SIZE 10

// Declare function prototypes
void flashExternalLED();
void playSpeaker(); 
void updateDisplay();
void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);
void sleep_474(int t);
void schedule_sync();

// Task states
enum TaskState {
  READY,
  RUNNING,
  SLEEPING
};

// Flag States
enum FlagState {
  PENDING,
  DONE
};

// Array of function pointers to tasks to be executed
void (*taskScheduler[MAX_SIZE])() = {flashExternalLED, playSpeaker, updateDisplay, schedule_sync}; // Adjust the array size according to the number of tasks
// Array to keep track of task states (READY, RUNNING, SLEEPING, PENDING, DONE)
enum TaskState taskStates[MAX_SIZE] = {READY, READY, READY, READY}; // Initialize the states for each task
// Array to store sleep duration for each task
int taskSleep[MAX_SIZE] = {0, 0, 0, 0};
// Array to store the start time of sleep for each task
unsigned long taskSleepStartTime[MAX_SIZE] = {0, 0, 0, 0};
// Flag to signal when a task is completed
volatile int sFlag = READY;
// A counter that tracks how many times the ISR has run since last interupt LED toggle
volatile long isrCounter = 0;
// How many times the ISR should run before the interupt LED gets toggled
const uint16_t toggleThreshold = 100;
// Index of the current task being executed
int currentTask = 0;
// Variable to keep track of time
unsigned long timerCounter = 0;
//x will be incremented by 1 in each iteration where the program waits until sFlag changes from PENDING 
int x = 0;
// Setup function, runs once at the start
void setup() {
  byte numDigits = 4;
  byte digitPins[] = {22, 23, 24, 25};
  byte segmentPins[] = {26, 28, 29, 30, 31, 32, 33, 34};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected
  
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
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(100);
    
  // Enable global interrupts
  sei();
}
// Loop function, runs repeatedly after setup
void loop() {
  // Enter the loop when a task is present and not null, one of the 3
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

void flashExternalLED() {
  const int onInterval = 250;
  int interval = 1000;   // Total interval (1s)

  if (taskStates[currentTask] == RUNNING) {
    if (timerCounter % interval <= onInterval) {
      bit_set(LED_PORT, LED_PIN);
    } else {
      bit_clear(LED_PORT, LED_PIN);
    }
  }
}

int songCycle() {
  // chaning the rest element to beat of 80 which yeilds 4s pause between cycles 
  int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
  int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 80};
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  static long currentTime = timerCounter;
  static int noteIndex;
  
  int noteDuration = 50 * beats[noteIndex];
  int freq = melody[noteIndex];
  int playTime;
  // Check if the current note's duration has elapsed
  if (timerCounter - currentTime >= noteDuration) {
  // Move on to the next note in the melody  
    noteIndex++;
  // Update the current time to the current value of timerCounter
    currentTime = timerCounter;
  }
  // Check if all notes in the melody have been played
  if (noteIndex >= melodyLength) {
    // If the end of the melody is reached, reset the noteIndex to 0 to start over  
    
    noteIndex = 0;
    for (int i = 0; i < MAX_SIZE; i++) {
    if (taskScheduler[i] == sleep_474) {
      currentTask = i;
      break;
    }
  }
  //sleep_474(4000); // 4-second sleep between song cycles
}
return freq;
}

// This function is responsible for playing the notes on the speaker.
void playSpeaker() {
  // Extract the frequency of the current note from the songCycle() function
  int freq = songCycle();
  // Check if the frequency is 0, which means a rest or no note to play
  if (freq == 0) {
    //sleep_474(4000);
    // Turn off the speaker by setting the Output Compare Register A to 0
    OCR4A = 0;
  } else {
    // If it's not a rest, calculate the appropriate OCR4A value to produce the desired frequency
    // Toggling the speaker on compare match, which controls the frequency of the output
    OCR4A = (F_CPU / (64UL * freq)) / 2;
  }
}

void updateDisplay() {
  int deciSecondsElapsed = (timerCounter / 100) % 1000;
  sevseg.setNumber(deciSecondsElapsed, 0);
  sevseg.refreshDisplay(); // Must run repeatedly
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
// This function puts the current task to sleep for a specified duration.
// The task is marked as SLEEPING, and its sleep start time and sleep duration are recorded.
void sleep_474(int t) {
  // Set the task state to SLEEPING
  taskStates[currentTask] = SLEEPING;
  // Record the current time as the start time of sleep
  taskSleepStartTime[currentTask] = timerCounter;
  // Set the sleep duration for the current task
  taskSleep[currentTask] = t;
}

ISR(TIMER3_COMPA_vect) {
  isrCounter++; // Increment the ISR counter

  if (isrCounter >= toggleThreshold) {
      LED_PORT ^= (1 << INTERRUPT_LED_PIN);
      isrCounter = 0; // Reset the counter after LED toggle
  }
  sFlag = DONE;
}

void schedule_sync() {
  while (sFlag == PENDING) {
    x += 1;
  }
  
  unsigned long currentTime = timerCounter;
  
  if (taskStates[currentTask] == SLEEPING) {
    unsigned long elapsedTime = currentTime - taskSleepStartTime[currentTask];
    if (elapsedTime >= taskSleep[currentTask]) {
      // If the sleep duration has passed, set the task state to READY
      taskStates[currentTask] = READY;
    }
  } else {
    taskStates[currentTask] = READY;
  } 
  sFlag = PENDING; // Reset sFlag to PENDING
}