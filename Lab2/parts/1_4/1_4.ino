#define BIT_47 1 << 2 // PL2
#define BIT_48 1 << 1 // PL1
#define BIT_49 1 << 0 // PL0

#define LED_PORT PORTL
#define LED_DDR DDRL

void setup() {
  LED_DDR |= BIT_47 | BIT_48 | BIT_49;
}

void loop() {
  LED_PORT &= ~(BIT_49 | BIT_48);
  LED_PORT |= BIT_47;
  delay(333);
  LED_PORT &= ~(BIT_47 | BIT_49);
  LED_PORT |= BIT_48;
  delay(333);
  LED_PORT &= ~(BIT_48 | BIT_47);
  LED_PORT |= BIT_49;
  delay(333);
}