#include <avr/io.h>
#include <avr/interrupt.h>

#define DDR_L   DDRL
#define PORT_L  PORTL
#define LED_PIN PL0 // pin 49
#define LED_PORT PORTL
#define LED_DDR DDR_L

#define E 659
#define R 0

// Define the bit number for pin 49 (PL0)
#define PIN_49  0

const int numTasks = 5; // Replace 5 with the number of tasks you have

// Task states
enum TaskState {
  RUNNING,
  SLEEPING
};

// Declare the taskScheduler array to hold task function pointers
void (*taskScheduler[numTasks])() = {NULL};

// Declare the taskStates array to hold the state of each task
enum TaskState taskStates[numTasks];

// Declare the currentTask and timerCounter variables
int currentTask = 0;
int timerCounter = 0;

void setup() {
  // Set up pins as outputs for the LED
  bit_set(DDR_L, PIN_49);

  // Set up Timer0 for the scheduler
  TCCR0A = 0;
  TCCR0B = (1 << CS01) | (1 << CS00); // Set prescaler to 64
  OCR0A = 249; // Set compare match value for 2ms interrupt
  TIMSK0 |= (1 << OCIE0A); // Enable Timer0 Compare Match A interrupt

  // Initialize the task structure for the ISR
  taskScheduler[0] = flashExternalLED; // Add other tasks as needed
  taskStates[0] = RUNNING; // Initialize the task state

  // Initialize timerCounter
  timerCounter = 0;
}

void loop() {
  // Enter the loop when a task is present in the taskScheduler
  while (taskScheduler[currentTask] != NULL) {
    // Run the task if it is not in the SLEEPING state
    if (taskStates[currentTask] != SLEEPING) {
      // Change the task state to RUNNING
      taskStates[currentTask] = RUNNING;

      // Run the task function
      taskScheduler[currentTask]();

      // Change the task state back to SLEEPING after execution
      taskStates[currentTask] = SLEEPING;
    }

    // Increment the currentTask
    currentTask++;
  }

  // Reset the currentTask to 0 for the next iteration
  currentTask = 0;
}

void flashExternalLED() {
  const int onInterval = 250;
  int interval = 1000;   // Total interval (1s)

  if (taskStates[currentTask] == RUNNING) {
    // Implement the logic for flashing the LED based on the timerCounter
    if (timerCounter % interval <= onInterval) {
      bit_set(PORT_L, PIN_49);
    } else {
      bit_clear(PORT_L, PIN_49);
    }
  }
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

void sleep_474(int t) {
  // Set the sleep time of the current task
  taskStates[currentTask] = SLEEPING;
}

void schedule_sync() {
  while (taskStates[currentTask] == SLEEPING) {
    int deltaT = 2; // Time in ms between interrupts (2ms)
    for (int i = 0; i < numTasks; i++) {
      if (taskStates[i] == SLEEPING) {
        taskScheduler[i]();
        if (taskScheduler[i] <= 0) {
          taskStates[i] = RUNNING;
          taskScheduler[i] = 0;
        }
      }
    }
    taskStates[currentTask] = SLEEPING;
  }
}

ISR(TIMER0_COMPA_vect) {
  timerCounter++;
  taskStates[currentTask] = SLEEPING;
}
