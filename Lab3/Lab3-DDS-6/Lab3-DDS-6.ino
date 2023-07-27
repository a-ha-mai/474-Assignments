/**
 * @file Lab3_DDS.ino
 * @date __date__
 * @brief ECE Lab 3 Assignment
 * @detail University of Washington ECE 474
 * 
 * 
 */
// Define states for TCB, timers, and periods
// FILL THESE VALUES ACCORDING TO THE SPECIFICATION GIVEN IN YOUR ASSIGNMENT
#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object
#define LED_DDR   DDRL
#define LED_PORT  PORTL
#define LED_PIN   PL0 // pin 49

#define SPEAKER_PORT PORTH
#define SPEAKER_DDR  DDRH
#define SPEAKER_PIN  PH3 // pin 6

#define E 659
#define C 523
#define G 784
#define g 392
#define R 0

#define MAX_SIZE 10

unsigned long timerCounter = 0;
int currentTask = 0;
int songPlayCount = 0;
long countdownStartTime;
long smileStartTime;

enum TaskState {
  READY,
  RUNNING,
  SLEEPING,
  DEAD
};

/**
  * A struct that defines the template of characteristics for one task
  * @return a struct that contains the characteristics declared within
  */
struct TCB {
  int pid;
  void* function;
  int state;
  int timesRestarted;
} TCB_struct; // name given to the TCB struct

// Create an array of TCB structures to control the processes
struct TCB taskScheduler[MAX_SIZE];
struct TCB deadTaskList[MAX_SIZE];
// Here we will define our states for TCB, timers, and periods based on the specifications given in the assignment

// An array of TCB structures will be created here to control the processes
//void updateDisplay();
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
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  // Set Waveform Generation bits (WGM) to Fast PWM mode to timer 4
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) 
  TCCR4A |= (1 << COM4A0) | (1 << COM4B0) | (1 << COM4C0);
  TCCR4A &= ~(1 << COM4A1) | ~(1 << COM4B1) | ~(1 << COM4C1);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);

  // Initialize the seven segment display
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(100);
  
  taskScheduler[0].pid = 0;
  taskScheduler[0].function = flashExternalLED;
  taskScheduler[0].state = READY;
  taskScheduler[0].timesRestarted = 0;

  taskScheduler[1].pid = 1;
  taskScheduler[1].function = playSpeaker;
  taskScheduler[1].state = READY;
  taskScheduler[1].timesRestarted = 0;

  taskScheduler[2].pid = 2;
  taskScheduler[2].function = updateDisplay;
  taskScheduler[2].state = READY;
  taskScheduler[2].timesRestarted = 0;
}

void loop() {
  while(1) {
    scheduler();
    timerCounter++;
    delay(1);
  }
}

// Function for flashing the external LED
void flashExternalLED() {
  const int onInterval = 250;
  int interval = 1000;   // Total interval (1s)
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
  int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 80};
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  static long currentTime = timerCounter;
  static int noteIndex;
  
  int noteDuration = 50 * beats[noteIndex];
  int freq = melody[noteIndex];
  int playTime;
  
  if (timerCounter - currentTime >= noteDuration) {
    noteIndex++;
    currentTime = timerCounter;
  }
  
  if (noteIndex == melodyLength) {
    noteIndex = 0;
    songPlayCount++;
    
    if (songPlayCount >= 2) {
      if (taskScheduler[getIndexFromPID(1)].timesRestarted == 0) {
        countdownStartTime = timerCounter;
      } else {
        smileStartTime = timerCounter;
      }
      task_self_quit();
    }
  }
  
  return freq;
}

/**
  * Function that plays a theme from a song
  * and then sleeps for a certain amount of time, then plays the theme again
  */
void playSpeaker() {
  int freq = songCycle();
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = (F_CPU / (64UL * freq)) / 2; //The value of OCR4A is set according to the frequency returned from speakerCycle()
  }
}

void function_ptr(void* function()) {
  // Invoke the function passed as parameter here
  function();
}

void task_self_quit() {
  taskScheduler[currentTask].state = DEAD;
  // Move this task to the deadTaskList
  deadTaskList[currentTask] = taskScheduler[currentTask];
  // Shift down the remaining tasks in taskScheduler
  for (int i = currentTask; i < MAX_SIZE - 1; i++) {
    taskScheduler[i] = taskScheduler[i + 1];
  }
  // Clear the last element of taskScheduler since it's a duplicate of the previous one
  taskScheduler[MAX_SIZE - 1].pid = NULL;
  taskScheduler[MAX_SIZE - 1].function = NULL;
  taskScheduler[MAX_SIZE - 1].state = NULL;
  taskScheduler[MAX_SIZE - 1].timesRestarted = NULL;
}

/**
  * This function allows a task to start up another task\
  * A task can call this function to change the status of a DEAD task to READY,\
  * effectively "reviving" the task.\
  * @param[task] pointer to task control block struct\
  */  
void task_start(TCB* task) {
  // Find the first null entry in the taskScheduler
  int firstNullIndex;
  for (int i = 0; i < MAX_SIZE; i++) {
    if (taskScheduler[i].function == NULL) {
      firstNullIndex = i;
      break;
    }
  }
  // Set the desired task's state to READY and move it to the taskScheduler
  task->state = READY;
  task->timesRestarted++;
  taskScheduler[firstNullIndex] = *task;

  // Clear the task from the deadTaskList
  deadTaskList[currentTask].pid = NULL;
  deadTaskList[currentTask].function = NULL;
  deadTaskList[currentTask].state = NULL;
  deadTaskList[currentTask].timesRestarted = NULL;
}

void scheduler(){
  while (taskScheduler[currentTask].function != NULL) {
    if (taskScheduler[currentTask].state != SLEEPING) {
      function_ptr(taskScheduler[currentTask].function);
      taskScheduler[currentTask].state = RUNNING;
    }
    currentTask++;
  }
  currentTask = 0;
}

// set the bit to 1 given bit position and register name
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
// set the bit to 0 given bit position and register name
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

void updateDisplay() {
  int countdownStart = 30;
  int countdown;
  int smileDuration = 2000;
  uint8_t smile[4] = {0x64, 0x09, 0x09, 0x52};
  
  if (songPlayCount == 2) {
    if (timerCounter - countdownStartTime <= 3000) {
      countdown = countdownStart - ((timerCounter - countdownStartTime) / 100);
      sevseg.setNumber(countdown, 0);
    } else {
      task_start(&deadTaskList[getIndexFromPID(1)]);
    }
  } else if (songPlayCount > 2 & deadTaskList[getIndexFromPID(1)].timesRestarted > 0) {
    if (timerCounter - smileStartTime <= smileDuration) {
      // smile goes here
      sevseg.setSegments(smile);
    } else {
      sevseg.blank();
      task_self_quit();
    }
  } else {
    sevseg.blank();
  }
  sevseg.refreshDisplay(); // Must run repeatedly
}

int getIndexFromPID(int pid) {
  // Search for the task in taskScheduler
  for (int i = 0; i < MAX_SIZE; i++) {
    if (taskScheduler[i].pid == pid) {
      return i;
    }
  }

  // If the task is not found in taskScheduler, search in deadTaskList
  for (int i = 0; i < MAX_SIZE; i++) {
    if (deadTaskList[i].pid == pid) {
      return i;
    }
  }
  
  // If the task is not found in both arrays, return -1
  return -1;
}