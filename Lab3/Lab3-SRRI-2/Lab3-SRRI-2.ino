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

#define MAX_SIZE 10

// Declare function prototypes
void flashExternalLED();
void playSpeaker(); 
void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);
void sleep_474(int t);
void schedule_sync();

// Task states
enum TaskState {
  READY,
  RUNNING,
  SLEEPING,
  PENDING,
  DONE
};

void (*taskScheduler[MAX_SIZE])() = {flashExternalLED, playSpeaker, schedule_sync, NULL}; // Adjust the array size according to the number of tasks
enum TaskState taskStates[MAX_SIZE] = {READY, READY, READY, NULL}; // Initialize the states for each task
int taskSleep[MAX_SIZE] = {0, 0, 0};
unsigned long taskSleepStartTime[MAX_SIZE] = {0, 0, 0};
volatile int sFlag = READY;
int currentTask = 0;
unsigned long timerCounter = 0;
int x = 0;
volatile unsigned long remainingSleepTime = 0;
bool songPlaying = false;

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
  int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
  int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 15};
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
  
  if (noteIndex >= melodyLength) {
    songPlaying = false;
    noteIndex = 0;
    //sleep_474(4000);

    
  }
  return freq;
}

void playSpeaker() {
  int freq = songCycle();
  if (freq == 0 || !songPlaying) {
    OCR4A = 0; // Silence when freq is 0 or song is not playing
  } else {
    OCR4A = (F_CPU / (64UL * freq)) / 2;
  }
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}

void sleep_474(int t) {
  taskStates[currentTask] = SLEEPING;
  taskSleepStartTime[currentTask] = timerCounter;
  taskSleep[currentTask] = t;
  remainingSleepTime = t;
}



ISR(TIMER4_COMPA_vect) {
  // Decrement the remaining sleep time if it's greater than 0
  if (remainingSleepTime > 0) {
    remainingSleepTime--;
  }
  
  // Check if the currentTask is in SLEEPING state and the sleep time has elapsed
  if (taskStates[currentTask] == SLEEPING && remainingSleepTime == 0) {
    // Set the task state to READY
    taskStates[currentTask] = READY;
  }
  
  // Increment the timerCounter
  timerCounter++;
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