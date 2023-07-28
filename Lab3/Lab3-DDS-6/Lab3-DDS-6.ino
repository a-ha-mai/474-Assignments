/**
 * @file Lab3_DDS-6.ino
 * @author Anna Mai (2165101)
 * @author Paria Naghavi (1441396)
 * @date 28-July-2023
 * @brief UW ECE 474 Lab 3 Assignment
 * 
 * A Data-Driven Scheduler that runs 3 tasks:
 * 1. Flashing an LED on for 250ms and off for 750ms
 * 2. Playing the intro to the Super Mario Bros. theme song repeatedly, with 4 seconds of silence between each play
 * 3. Updating a seven segment display
 * 
 * These tasks will start and stop according to this schedule:
 * - Task 1 runs all the time.
 * - Task 2 runs at the start but stops after playing the theme 2 times.
 * - After Task 2, start a count-down on the 7-segment display for 3 seconds and then restarts task 2 for one final time.
 * - Display a “smile” for 2 seconds on the 7-segment display after part 5c is completed
 * - Stop all tasks, except “a) Task 1” in this list.
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
 * @brief Play Count of Song
 * 
 * How many times the Super Mario Bros. intro has been played.
 */ 
int songPlayCount = 0;
/**
 * @brief The start time of countdown
 * 
 * Marks the timerCounter value that the countdown was started on.
 */
long countdownStartTime;
/**
 * @brief The start time of smile display
 * 
 * Marks the timerCounter value that the smile started to be displayed.
 */
long smileStartTime;

/**
 * @enum TaskState
 * @brief The various states a task can be in.
 */
enum TaskState {
  READY,    /**< Task is ready to run. */
  RUNNING,  /**< Task is currently executing. */
  SLEEPING, /**< Task is waiting for a specific event or time. */
  DEAD      /**< Task has been terminated. */
};

/**
 * @struct TCB
 * @brief Template for defining characteristics of a task (Thread Control Block).
 */
struct TCB {
  int pid;
  void* function;
  int state;
  int timesRestarted;
} TCB_struct; // name given to the TCB struct

/**
 * @brief Array of TCB structures for process control.
 */
struct TCB taskScheduler[MAX_SIZE];
/**
 * @brief Array of TCB structures for terminated tasks.
 */
struct TCB deadTaskList[MAX_SIZE];

/**
 * @brief Initializes pins, timers, and tasks during setup.
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
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  
  // Set Waveform Generation bits (WGM) to Fast PWM mode to timer 4
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) 
  TCCR4A |= (1 << COM4A0) | (1 << COM4B0) | (1 << COM4C0);
  TCCR4A &= ~(1 << COM4A1) | ~(1 << COM4B1) | ~(1 << COM4C1);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);

  // Initialize the seven-segment display using the SevSeg library
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(100);
  
  // Initialize first task
  taskScheduler[0].pid = 0;
  taskScheduler[0].function = flashExternalLED;
  taskScheduler[0].state = READY;
  taskScheduler[0].timesRestarted = 0;
  
  // Initialize second task
  taskScheduler[1].pid = 1;
  taskScheduler[1].function = playSpeaker;
  taskScheduler[1].state = READY;
  taskScheduler[1].timesRestarted = 0;

  // Initialize third task
  taskScheduler[2].pid = 2;
  taskScheduler[2].function = updateDisplay;
  taskScheduler[2].state = READY;
  taskScheduler[2].timesRestarted = 0;
}

/**
 * @brief Controls the execution of tasks based on the process list and scheduler.
 */
void loop() {
  while(1) {
    scheduler();
    timerCounter++;
    delay(1);
  }
}

/**
 * @brief Flashes the LED connected to pin 49 on for 250ms and off for 750ms
 */
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
  
  // Check if the melody has completed one full cycle
  if (noteIndex == melodyLength) {
    noteIndex = 0; // Reset the noteIndex to play the melody from the beginning
    songPlayCount++; // Increment the songPlayCount to keep track of how many times the melody has played
    
    // Check if the melody has played at least twice
    if (songPlayCount >= 2) {
      // Check if the speaker task has not been restarted
      if (taskScheduler[getIndexFromPID(1)].timesRestarted == 0) {
        countdownStartTime = timerCounter; // Set the countdown start time
      } else {
        smileStartTime = timerCounter; // Set the smile display start time
      }
      // Quit the current task (songCycle) to proceed with other tasks in the scheduler
      task_self_quit();
    }
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
  int countdownStart = 30; // Countdown start value (in seconds)
  int countdown; // Variable to store the countdown value (in seconds)
  int smileDuration = 2000; // Duration of the smile display (in milliseconds)
  uint8_t smile[4] = {0x64, 0x09, 0x09, 0x52}; // Smile pattern segments representation
  
  if (songPlayCount == 2) {
    // If the melody has played twice and the speaker task has not been restarted display a countdown on the 7-Segment display.
    if ((timerCounter - countdownStartTime) <= (countdownStart * 100)) {
      // Calculate the remaining countdown value based on the time elapsed since countdown started
      countdown = countdownStart - ((timerCounter - countdownStartTime) / 100);
      // Display the countdown value on the 7-Segment display
      sevseg.setNumber(countdown, 0);
    } else {
      // When the countdown period is over, restart the speaker task.
      task_start(&deadTaskList[getIndexFromPID(1)]);
    }
  } else if (songPlayCount > 2 & deadTaskList[getIndexFromPID(1)].timesRestarted > 0) {
    // If the melody has played more than twice and the speaker task has been restarted,
    // display a smile pattern on the 7-Segment display for 2 seconds.
    if (timerCounter - smileStartTime <= smileDuration) {
      // Display the smile pattern on the 7-Segment display
      sevseg.setSegments(smile);
    } else {
      // When the smile duration is over, display nothing and quit the current task.
      sevseg.blank();
      task_self_quit();
    }
  } else {
    // For other cases, simply display nothing.
    sevseg.blank();
  }
  
  // Refresh the display to update the content shown on the 7-Segment display
  sevseg.refreshDisplay(); // Must run repeatedly
}

/**
 * @brief Function pointer to execute any given task.
 * @param function A function pointer to the task or function that needs to be executed.
 *                 The function should have a void return type and take no parameters.
 */
void function_ptr(void* function()) {
  // Invoke the function passed as parameter here
  function();
}

/**
 * @brief Allows a task to terminate itself by manipulating its TCB.
 * 
 * This changes the task state to DEAD and removes itself from the taskScheduler
 * and moved into deadTaskList, which allows for future referencing.
 */
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
 * @brief Allows a task to start up another task by changing its status from DEAD to READY.
 * 
 * This also removes the task from deadTaskList, and it to taskScheduler
 * while also incrementing the the task's timesRestarted parameter by 1.
 * 
 * @param task A pointer to the Task Control Block (TCB) of the task to be started.
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

/**
 * @brief Schedules and executes tasks from the taskScheduler.
 */
void scheduler(){
  // Iterate through the taskScheduler and process each task
  while (taskScheduler[currentTask].function != NULL) {
    if (taskScheduler[currentTask].state != SLEEPING) {
      // Execute the task's associated function using function_ptr
      function_ptr(taskScheduler[currentTask].function);
      // Change the task's state to RUNNING during its execution
      taskScheduler[currentTask].state = RUNNING;
    }
    currentTask++; // Move to the next task in the taskScheduler
  }
  currentTask = 0; // Reset the currentTask to the beginning for the next scheduling cycle
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
 * @brief Searches for the index of a task in the taskScheduler or deadTaskList based on its PID.
 * @param[in] pid The Process ID of the task to be searched for.
 * @return The index of the task in the corresponding array if found, or -1 if the task is not found.
 */
int getIndexFromPID(int pid) {
  // Search for the task in taskScheduler
  for (int i = 0; i < MAX_SIZE; i++) {
    if (taskScheduler[i].pid == pid) {
      return i; // Return the index of the task in the taskScheduler array
    }
  }

  // If the task is not found in taskScheduler, search in deadTaskList
  for (int i = 0; i < MAX_SIZE; i++) {
    if (deadTaskList[i].pid == pid) {
      return i; // Return the index of the task in the deadTaskList array
    }
  }
  
  // If the task is not found in both arrays, return -1
  return -1;
}