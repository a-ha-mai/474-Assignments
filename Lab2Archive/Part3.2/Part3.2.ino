#define SPEAKER_PIN PH3 // pin 6
#define LED_1 PL2 // pin 47
#define LED_2 PL1 // pin 48
#define LED_3 PL0 // pin 49
#define SPEAKER_PORT PORTH
#define SPEAKER_DDR DDRH
#define LED_PORT PORTL
#define LED_DDR DDRL
// Global flags
// This flag is set to true when Task A is active and being executed. It is set to false when Task A should be stopped.
volatile bool taskAActive;
// The taskBActive flag is used in Task B to determine whether the speaker should play a tone. When taskBActive is true, Task B is active and the speaker should play a tone based on the current time.
volatile bool taskBActive;

void setup() {
  LED_DDR |= (1 << LED_1) | (1 << LED_2) |(1 << LED_3);
  // Set Waveform Generation Mode (WGM) to Fast PWM mode (Mode 14)
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
  //bit_set is called with arguments SPEAKER_DDR and SPEAKER_PIN. Setting a specific bit in a register to 1.
  // The line bit_set(SPEAKER_DDR, SPEAKER_PIN); sets the specified bit of the SPEAKER_DDR register to 1. 
  // It configures the corresponding pin of the speaker as an output by setting the direction register (DDR) for that pin.
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  taskAActive = false;
  taskBActive = false;
}
void loop() {
  int taskATime = 2000; // Duration of Task A in milliseconds
  int taskBTime = 4000; // Duration of Task B in milliseconds
  int taskABTime = 10000; // Duration of Task A and Task B running simultaneously in milliseconds
  int downtime = 1000; // Duration of downtime between tasks in milliseconds
  long startTime;
  
  taskAActive = true;  // Start Task A
  startTime = millis(); // Get the current time
  while (millis() - startTime < taskATime) {
    ledCycle(); // Run the LED cycle for Task A
  }
  taskAActive = false;  // Stop Task A
  
  taskBActive = true; // Start Task B
  startTime = millis(); // Get the current time
  while (millis() - startTime < taskBTime) {
    if (speakerCycle(startTime) == 0) {
      OCR4A = 0;
    } else {
      OCR4A = (F_CPU / (64UL * speakerCycle(startTime))) / 2;
    }
  }
  OCR4A = 0;
  taskBActive = false;
  
  // Task A and Task B run simultaneously for 10 seconds
  taskAActive = true;  // Start Task A
  taskBActive = true;
  startTime = millis();
  while (millis() - startTime < taskABTime) {
    ledCycle();
    if (speakerCycle(startTime) == 0) {
      OCR4A = 0;
    } else {
      OCR4A = (F_CPU / (64UL * speakerCycle(startTime))) / 2;
    }
  }
  OCR4A = 0;
  taskAActive = false;
  taskBActive = false;
  
  delay(downtime);  // No tasks for 1 second
}
// Setting a specific bit in a register to 1
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
// Setting a specific bit in a register to 0
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
int speakerCycle(int startTime) {
  int interval = 4000;
  long currentTime = millis() - startTime;
  int freq = 0;
  if (taskBActive) {
    if (currentTime % interval >= 0 && currentTime % interval < 1000) {
      freq = 400;
    } else if (currentTime % interval >= 1000 && currentTime % interval < 2000) {
      freq = 250;
    } else if (currentTime % interval >= 2000 && currentTime % interval < 3000) {
      freq = 800;
    } 
  } else {
    freq = 0;
  }
  return freq;
}
void ledCycle() {
   if (taskAActive) {
    int interval = 999;
    long currentTime = millis();
    if (currentTime % interval >= 0 && currentTime % interval < 333) {
      bit_set(LED_PORT, LED_1);
      bit_clear(LED_PORT, LED_2);
      bit_clear(LED_PORT, LED_3);
    } else if (currentTime % interval >= 333 && currentTime % interval < 666) {
      bit_clear(LED_PORT, LED_1);
      bit_set(LED_PORT, LED_2);
      bit_clear(LED_PORT, LED_3);
    } else if (currentTime % interval >= 666 && currentTime % interval < 999) {
      bit_clear(LED_PORT, LED_1);
      bit_clear(LED_PORT, LED_2);
      bit_set(LED_PORT, LED_3);
    }
  } else {
      bit_clear(LED_PORT, LED_1);
      bit_clear(LED_PORT, LED_2);
      bit_clear(LED_PORT, LED_3);
  } 
}