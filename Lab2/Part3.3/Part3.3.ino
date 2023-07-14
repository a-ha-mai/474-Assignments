#define SPEAKER_PIN PH3 // pin 6
#define LED_1 PL2 // pin 47
#define LED_2 PL1 // pin 48
#define LED_3 PL0 // pin 49
#define SPEAKER_PORT PORTH
#define SPEAKER_DDR DDRH
#define LED_PORT PORTL
#define LED_DDR DDRL

// Note Frequencies 
#define NC 261
#define ND 294
#define NE 329
#define NF 349
#define NG 392
#define NA 440
#define NB 493
#define NC2 523
#define R 0

// Global flags for each task
volatile bool taskAActive;
volatile bool taskBActive;

void setup() {
// Setting the pins as outputs
  LED_DDR |= (1 << LED_1) | (1 << LED_2) |(1 << LED_3);
  // Set Waveform Generation bits (WGM) to Fast PWM mode to timer 4
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
  // set the speaker pin as output 
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  taskAActive = false;
  taskBActive = false;
}
void loop() {
  int taskATime = 2000;
  int taskBTime = 10000;
  int taskABTime = 10000;
  int downtime = 1000;
  long startTime;
  
  taskAActive = true;  // Start Task A
  startTime = millis();
  //while loop continues until the task A duration is reached 
  while (millis() - startTime < taskATime) {
    ledCycle();
  }
  taskAActive = false;  // Stop Task A
  
  taskBActive = true; // Starts Task B
  startTime = millis();
  // Continue while depending on the frequency variable returned from speakercycle()
  while (millis() - startTime < taskBTime) {
    int freq = speakerCycle();
    if (freq == 0) {
      OCR4A = 0;
    } else {
      OCR4A = (F_CPU / (64UL * freq)) / 2; //The value of OCR4A is set according to the frequency returned from speakerCycle()
    }
  }
  OCR4A = 0;
  taskBActive = false; // halt task B
  
  // Task A and Task B run simultaneously for 10 seconds
  taskAActive = true;  // Start Task A
  taskBActive = true; // Start Task B
  startTime = millis();
  //As long the TaskAB time is not reaches the LED cycle and speaker cycle will run 
  while (millis() - startTime < taskABTime) { 
    ledCycle();
    int freq = speakerCycle();
    if (freq == 0) {
      OCR4A = 0;
    } else {
      OCR4A = (F_CPU / (64UL * freq)) / 2;
    }
  }
  OCR4A = 0;
  taskAActive = false;
  taskBActive = false;
  
  delay(downtime);  // No tasks for 1 second
}
// sets bits in a register
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
// clears bits in a register
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
// function defn for speaker to iterate over 
int speakerCycle() {
  int melody[] = { NE, R, ND, R, NC, R, ND, R, NE, R, NE, R, NE, R, ND, R, ND, R, ND, R, NE, R, NG, R, NG, R, NE, R, ND, R, NC, R, ND, R, NE, R, NE, R, NE, R, NE, R, ND, R, ND, R, NE, R, ND, R, NC, R};
  int beats[] =  { 5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  13, 1,  5, 1,  5, 1,  13, 1,  5, 1,  5, 1,  13, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  26, 1};
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  static long currentTime = millis();
  static int noteIndex;
  int freq;

  if (taskBActive) {
    int noteDuration = 50 * beats[noteIndex]; // mutiplied the array to reach 10,000 which is task A and B duration 
    int freq = melody[noteIndex]; // sets freq as melody at that index
    // increments through the melody and beats index after note durtion excessed 
    if (millis() - currentTime >= noteDuration) {
      noteIndex = (noteIndex + 1) % melodyLength;
      currentTime = millis();
    }
  // returns the freq for the loop() 
    return freq; // returns frequency to be used in the loop()
  } else {
    noteIndex = 0;
    currentTime = millis();
    return 0; // No sound
  }
}
// Instead of using delay, we use if conditonals with time intervals to keep LEDs flashing with the given period 
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