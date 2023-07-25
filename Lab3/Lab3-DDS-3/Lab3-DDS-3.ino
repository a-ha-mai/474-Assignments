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

enum TaskState {
  READY,
  RUNNING,
  SLEEPING,
  DONE,
  PENDING,
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
} TCB_struct; // name given to the TCB struct

// Create an array of TCB structures to control the processes
struct TCB taskScheduler[MAX_SIZE];

/**
  * Function initializes pins to output. Also initializes tasks
  */
void setup() {
  // Initialize the required pins as output
  // Set the required mode and prescalar value
  // Set the bits of COMnC1, COMnB1, COMnA1 to 0
  // Set the bits of COMnC0, COMnC0, COMnC0 to 1
  // Initialize characteristics of tasks in your processList array
  
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

  taskScheduler[0].pid = 0;
  taskScheduler[0].function = flashExternalLED;
  taskScheduler[0].state = READY;

  taskScheduler[1].pid = 1;
  taskScheduler[1].function = playSpeaker;
  taskScheduler[1].state = READY;

  taskScheduler[2].pid = 2;
  taskScheduler[2].function = NULL;
  taskScheduler[2].state = NULL;
}

/**
  * Function controls the tasks running dependent on the processList and scheduler
  * runs flashExternalLED and playSpeaker
  */
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

/**
  * This function serves as a function pointer to any given task to execute
  * @param[function()] function that needs a pointer
  */   
void function_ptr(void* function()) {
  // Invoke the function passed as parameter here
  function();
}

/**
  * This function allows a task to terminate itself by manipulating its TCB
  */  
void task_self_quit() {
  taskScheduler[currentTask].state = DEAD;
}

/**
  * This function allows a task to start up another task
  * @param[task] pointer to task control block struct
  */  
void task_start(TCB* task) {
  task -> state = READY;
}

/**
  * This function allows for tasks to be processed and executed
  */ 
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
