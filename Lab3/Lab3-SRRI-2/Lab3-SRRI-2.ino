
// Define pins and timers for LED and speaker
#define LED_DDR   DDRL
#define LED_PORT  PORTL
#define LED_PIN   PL0 // pin 49

#define SPEAKER_PORT PORTH
#define SPEAKER_DDR  DDRH
#define SPEAKER_PIN  PH3 // pin 6

// Define note frequencies
#define E 659
#define C 523
#define G 784
#define g 392
#define R 0

// Declare function prototypes
void flashExternalLED();
int playSpeaker(); // Change the return type to int
void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);
void sleep_474(int t);
int schedule_sync();
int remainingSleepTime;

void (*taskScheduler[10])() = {flashExternalLED, playSpeaker, NULL}; // Adjust the array size according to the number of tasks

// Task states
enum TaskState {
  READY,
  RUNNING,
  SLEEPING,
  PENDING,
  DONE
};

enum TaskState taskStates[10] = {READY, READY, READY}; // Initialize the states for each task
volatile int sFlag = READY;
int currentTask = 0;
int timerCounter = 0;

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
  
  // Enable Timer/Counter4 Compare Match A interrupt
  TIMSK4 |= (1 << OCIE4A);
  
  // Set Timer/Counter4 Compare Match A value for 2ms interrupt
  OCR4A = F_CPU / 64 / 500 - 1;
  
  // Enable global interrupts
  sei();
}

void loop() {
  // Enter the loop when a task is present in the taskScheduler
  while (taskScheduler[currentTask] != NULL) {
    // Run the task if it is not in the SLEEPING state
    if (taskStates[currentTask] == RUNNING) {
      taskScheduler[currentTask]();
    }

    // Change the task state to RUNNING
    taskStates[currentTask] = RUNNING;

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

int playSpeaker() { // Change the return type to int
  int freq = songCycle();
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = (F_CPU / (64UL * freq)) / 2;
  }
  return remainingSleepTime;
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

void sleep_474(int t) {
  taskStates[currentTask] = SLEEPING;
  taskScheduler[currentTask + 1] = schedule_sync;
  taskStates[currentTask + 1] = READY;
  remainingSleepTime = t;
  // Store the sleep time for the current task
  // The timer interrupt will handle decrementing the sleep time
}

ISR(TIMER4_COMPA_vect) {
  // Set the sFlag to DONE
  sFlag = DONE;
}

int schedule_sync() {
  while (sFlag == PENDING) {
    for (int i = 0; i < 10; i++) {
      if (taskStates[i] == SLEEPING) {
        remainingSleepTime -= 2; // Decrement sleep time by 2ms
        if (remainingSleepTime <= 0) {
          taskStates[i] = READY; // Wake up the sleeping task
        }
      }
    }
    sFlag = PENDING; // Reset sFlag to PENDING
  }
  return remainingSleepTime;
}
