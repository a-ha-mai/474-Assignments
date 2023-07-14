// defined constants representing different control operations for the LED matrix
#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10

//Transfers 1 SPI command to LED Matrix for given row
//Input: row - row in LED matrix
//       data - bit representation of LEDs in a given row; 1 indicates ON, 0 indicates OFF

void bit_set(volatile uint8_t& reg, uint8_t bit);
void bit_clear(volatile uint8_t& reg, uint8_t bit);

// 8x8 Pins, port and DDR
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
// sets the data direction registers for the DIN, CS, and CLK pins of the LED matrix. This configures these pins as outputs.
  BOARD_DDR |= (1 << DIN_PIN) | (1 << CS_PIN) | (1 << CLK_PIN);
//  clears the corresponding bits in the data direction register for the JOY_X and JOY_Y pins, effectively setting them as inputs.
  JOY_DDR &= ~((1 << JOY_X) | (1 << JOY_Y));
// By setting the CS pin to a high logic level, the LED matrix is selected, indicating that it is ready to receive commands
  bit_set(BOARD_PORT, CS_PIN);
// calling the spiTransfer function to send SPI commands to the LED matrix
// initialize and configure the LED matrix
  spiTransfer(OP_DISPLAYTEST, 0);
  spiTransfer(OP_SCANLIMIT, 7);
  spiTransfer(OP_DECODEMODE, 0);
  spiTransfer(OP_SHUTDOWN, 1);
}

void loop(){
  int* coordinates = findCoordinates();  // Call the function findCoordinates to get the joystick coordinates
  int x = coordinates[0];  // Assign the x coordinate from the returned array
  int y = coordinates[1];  // Assign the y coordinate from the returned array
  uint8_t rowControl = 0;  // Initialize a variable to hold the row control value

  bit_set(rowControl, x);  // Set the bit corresponding to the x coordinate in rowControl using the bit_set function
  spiTransfer(y, rowControl);  // Send the rowControl value as data to the LED matrix for the specified row y using spiTransfer function

  bit_clear(rowControl, x);  // Clear the bit corresponding to the x coordinate in rowControl using the bit_clear function
  spiTransfer(y, rowControl);  // Send the updated rowControl value as data to the LED matrix for the specified row y using spiTransfer function
}
// The purpose of the spiTransfer function is to transfer SPI command to the LED matrix. 
//It takes two parameters: opcode and data, which represent the control operation and data to be sent, respectively.
// The spiTransfer function takes two arguments: opcode and data. 
//The opcode represents the control operation or command for the LED matrix, and the data represents the data associated with that command.
void spiTransfer(volatile byte opcode, volatile byte data){
  int offset = 0; //only 1 device
  int maxbytes = 2; //16 bits per SPI command
  // This for loop is used to zero out the spidata array. 
  //The loop iterates maxbytes times, which is the number of elements in the spidata array. Each element of the spidata array is set to 0 using the assignment statement spidata[i] = (byte)0;.
  for(int i = 0; i < maxbytes; i++) { //zero out spi data
    spidata[i] = (byte)0;
  }
  //load in spi data
  //The variable specifies the index for spidata 
  spidata[offset] = data;
  // clears 
  bit_clear(BOARD_PORT, CS_PIN);
  for(int i=maxbytes;i>0;i--)
    shiftOut(DIN, CLK, MSBFIRST, spidata[i-1]); //shift out 1 byte of data starting with leftmost bit
  // this sets the port L and CS pin which enables the peripheral connection with LED display  
  bit_set(BOARD_PORT, CS_PIN);
}
//objective: read the analog data from the joystick's X and Y axes and convert them into digital coordinates
int* findCoordinates() {
  int* coordinates = new int[2];// Declaring a new integer array of size 2 dynamically 
  int xReading = min((1024 - analogRead(JOY_X)), 1023); // reads the inverted analog value from the pin connected to the X-axis of the joystick
  int yReading = analogRead(JOY_Y);//reads the analog value from the pin connected to the y-axis of the joystick 
  coordinates[0] = xReading / 128; //calculates the X-coordinate by dividing input of 1024 bit by 128 to get 8 bits data 
  coordinates[1] = yReading / 128;//calculates the Y-coordinate by dividing input of 1024 bit by 128 to get 8 bits data 
  return coordinates;
}
// set bits to 1 using bitwise OR 
void bit_set(volatile uint8_t& reg, uint8_t bit) {
  reg |= (1 << bit);
}
// set bits to 0 using bitwise AND 
void bit_clear(volatile uint8_t& reg, uint8_t bit) {
  reg &= ~(1 << bit);
}
