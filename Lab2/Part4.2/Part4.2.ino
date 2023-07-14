#define OP_DECODEMODE  8  // Control operation opcode for setting decode mode
#define OP_SCANLIMIT   10 // Control operation opcode for setting scan limit
#define OP_SHUTDOWN    11 // Control operation opcode for setting shutdown mode
#define OP_DISPLAYTEST 14 // Control operation opcode for display test
#define OP_INTENSITY   10 // Control operation opcode for setting intensity

// 8x8 Pins
#define DIN 47 // PL2 - Pin number for data input
#define CS  48 // PL1 - Pin number for chip select
#define CLK 49 // PL0 - Pin number for clock
#define DIN_PIN PL2 // Data input pin on the board
#define CS_PIN PL1 // Chip select pin on the board
#define CLK_PIN PL0 // Clock pin on the board
#define BOARD_PORT PORTL // Port for the board
#define BOARD_DDR DDRL // Data direction register for the board


// Joystick Pins
#define JOY_X PF0 // A0 - Pin number for joystick X-axis
#define JOY_Y PF1 // A1 - Pin number for joystick Y-axis
#define JOY_PORT PORTF // Port for the joystick
#define JOY_DDR DDRF // Data direction register for the joystick

// Speaker Pins
#define SPEAKER_PIN PH3 // Pin 6 - Pin number for the speaker
#define SPEAKER_PORT PORTH // Port for the speaker
#define SPEAKER_DDR DDRH // Data direction register for the speaker

// Note Frequencies
#define NC 261 // Note frequency for C
#define ND 294 // Note frequency for D
#define NE 329 // Note frequency for E
#define NF 349 // Note frequency for F
#define NG 392 // Note frequency for G
#define NA 440 // Note frequency for A
#define NB 493 // Note frequency for B
#define NC2 523 // Note frequency for C2
#define R 0 // Rest

//Transfers 1 SPI command to LED Matrix for given row
//Input: row - row in LED matrix
//       data - bit representation of LEDs in a given row; 1 indicates ON, 0 indicates OFF
void spiTransfer(volatile byte row, volatile byte data);
void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);

byte spidata[2]; //spi shift register uses 16 bits, 8 for ctrl and 8 for data

void setup() {
// Set Waveform Generation Mode (WGM) to Fast PWM mode (Mode 14)
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  
  // Set Output Compare A (OC4A) for toggling on compare match
  TCCR4A |= (1 << COM4A0);
  
  // Set Timer/Counter4 prescaler to 64 (desired frequency range)
  TCCR4B |= (1 << CS41) | (1 << CS40);
  
  bit_set(SPEAKER_DDR, SPEAKER_PIN);
  BOARD_DDR |= (1 << DIN_PIN) | (1 << CS_PIN) | (1 << CLK_PIN);
  JOY_DDR &= ~((1 << JOY_X) | (1 << JOY_Y));
  bit_set(BOARD_PORT, CS_PIN);

  spiTransfer(OP_DISPLAYTEST, 0);
  spiTransfer(OP_SCANLIMIT, 7);
  spiTransfer(OP_DECODEMODE, 0);
  spiTransfer(OP_SHUTDOWN, 1);
}

void loop(){ 
  int* coordinates = findCoordinates();
  int x = coordinates[0];
  int y = coordinates[1];
  uint8_t rowControl = 0;
  int freq = speakerCycle();
  long currentTime;

  // board clear was moved from fter bit-clear to the beginning --> for loop 

  for (int i = 0; i < 8; i++) {
    spiTransfer(i, 0);
  }
  bit_set(rowControl, x);
  spiTransfer(y, rowControl);
  bit_clear(rowControl, x);
  OCR4A = (F_CPU / (64UL * freq)) / 2;
}

void spiTransfer(volatile byte opcode, volatile byte data){
  int offset = 0; //only 1 device
  int maxbytes = 2; //16 bits per SPI command
  
  for(int i = 0; i < maxbytes; i++) { //zero out spi data
    spidata[i] = (byte)0;
  }
  //load in spi data
  spidata[offset+1] = opcode+1;
  spidata[offset] = data;
  bit_clear(BOARD_PORT, CS_PIN);
  for(int i=maxbytes;i>0;i--)
    shiftOut(DIN, CLK, MSBFIRST, spidata[i-1]); //shift out 1 byte of data starting with leftmost bit
  bit_set(BOARD_PORT, CS_PIN);
}

int* findCoordinates() {
  int* coordinates = new int[2];
  int xReading = min((1024 - analogRead(JOY_X)), 1023);
  int yReading = analogRead(JOY_Y);
  coordinates[0] = xReading / 128;
  coordinates[1] = yReading / 128;
  return coordinates;
}

int speakerCycle() {
  int melody[] = { NE, R, ND, R, NC, R, ND, R, NE, R, NE, R, NE, R, ND, R, ND, R, ND, R, NE, R, NG, R, NG, R, NE, R, ND, R, NC, R, ND, R, NE, R, NE, R, NE, R, NE, R, ND, R, ND, R, NE, R, ND, R, NC, R};
  int beats[] =  { 5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  13, 1,  5, 1,  5, 1,  13, 1,  5, 1,  5, 1,  13, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  5, 1,  26, 1};
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  static long currentTime = millis();
  static long noteIndex;

  int noteDuration = 50 * beats[noteIndex];
  int freq = melody[noteIndex];
  
  if (millis() - currentTime >= noteDuration) {
    noteIndex = (noteIndex + 1) % melodyLength;
    currentTime = millis();
  }
  return freq;
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
