/**
 * @file Lab3_DDS-5.ino
 * @author Anna Mai (2165101)
 * @author Paria Naghavi (1441396)
 * @date 28-July-2023
 * @brief UW ECE 474 Lab 3 Assignment
 * 
 * A Data-Driven Scheduler that runs 3 tasks simultaneously:
 * - Flashing an LED on for 250ms and off for 750ms
 * - Playing the intro to the Super Mario Bros. theme song repeatedly, with 4 seconds of silence between each play
 * - Updating a seven segment display based on the current song state
 *   - When playing, the frequency will be displayed
 *   - When not playing, the display will countdown until the next song is played by 1 every 100ms
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
 * @brief Flag indicating whether music is currently playing or not.
 */
bool musicPlaying = false;
/**
 * @brief Timestamp of the start of a downtime period.
 * 
 * The initial value of -1000 is to ensure that the first instance of downtime is processed properly.
 */
long downtimeStart = -1000;

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
  int pid;            /**< Process ID of the task. */
  void* function;     /**< Function pointer of the task. */
  int state;          /**< Current state of the task. */
  int timesRestarted; /**< Number of times the task was restarted. */
} TCB_struct;

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
    scheduler();    // Run the scheduler to manage task execution
    timerCounter++; // Increment the timer counter to measure elapsed time
    delay(1);       // Introduce a 1ms delay between each loop iteration
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
  
  // Check if the last note in the melody has been reached
  if (noteIndex == (melodyLength - 1)) {
    musicPlaying = false; // Set the musicPlaying flag to false as the melody is complete
    // Check if it's been more than 4 seconds since the melody ended
    // and update the downtimeStart timestamp if necessary
    if (timerCounter - downtimeStart >= 4000) {
      downtimeStart = timerCounter;
    }
  } else {
    musicPlaying = true; // Set the musicPlaying flag to true as there are more notes to play
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
  int downtime = 40; // Downtime in deciseconds (used when music is not playing)
  
  if (musicPlaying) {
    // If music is playing, retrieve the frequency of the next note from the songCycle() function
    int freq = songCycle();
    if (freq != 0) {
      // Display the frequency in Hz if the current note isn't a rest
      sevseg.setNumber(freq, 0);
    }
  } else {
    // If music is not playing, calculate the elapsed time in deciseconds since the downtime started
    int deciSecondsElapsed = downtime - ((timerCounter - downtimeStart) / 100);
    // Display the remaining downtime on the 7-Segment display
    sevseg.setNumber(deciSecondsElapsed, 0);
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
  function(); // Invoke the function passed as parameter here
}

/**
 * @brief Allows a task to terminate itself by manipulating its TCB.
 * 
 * This changes the task state to DEAD and removes itself from the taskScheduler
 * and moved into deadTaskList, which allows for future referencing.
 */
void task_self_quit() {
  // Set the current task's state to DEAD in the taskScheduler
  taskScheduler[currentTask].state = DEAD;
  
  // Move this task to the deadTaskList
  deadTaskList[currentTask] = taskScheduler[currentTask];
  
  // Shift down the remaining tasks in taskScheduler to remove the terminated task
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
