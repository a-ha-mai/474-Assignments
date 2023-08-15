// how bright and prarmetes how this LED function 
// paramter for fixung LED 
#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10
// communication protocol = SPI= serial communication/ serial data comes one at the time- bit se- packet of info with header- specific bit seq 
//Transfers 1 SPI command to LED Matrix for given row
//Input: row - row in LED matrix
//       data - bit representation of LEDs in a given row; 1 indicates ON, 0 indicates OFF
void spiTransfer(volatile byte row, volatile byte data);
void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);

// 8x8 Pins
#define DIN 47 // PL2
#define CS  48 // PL1
#define CLK 49 // PL0
#define DIN_PIN PL2
#define CS_PIN PL1
#define CLK_PIN PL0
#define BOARD_PORT PORTL
#define BOARD_DDR DDRL

// Joystick Pins
#define JOY_X PF0
#define JOY_Y PF1
#define JOY_PORT PORTF
#define JOY_DDR DDRF

byte spidata[2]; //spi shift register uses 16 bits, 8 for ctrl and 8 for data

void setup() {

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
  bit_set(rowControl, x);
  spiTransfer(y, rowControl);
  bit_clear(rowControl, x);
  spiTransfer(y, rowControl);
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
  int xReading = analogRead(JOY_X);
  int yReading = analogRead(JOY_Y);
  coordinates[0] = xReading / 128;
  coordinates[1] = yReading / 128;
  return coordinates;
}

void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}

void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
