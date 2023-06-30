const int ledPin = LED_BUILTIN;
const int extLedPin = 10;
const int ledPeriod = 400;
const int speakerPin = 2;
const int speakerPeriod = 4;
const int buzzingDuration = 5000;


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(extLedPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  if (millis() < buzzingDuration) {
    if (millis() % speakerPeriod == 0) {
      digitalWrite(speakerPin, HIGH);
      alternateLeds();
    } 
    else if (millis() % speakerPeriod == speakerPeriod / 2) {
      digitalWrite(speakerPin, LOW);
      alternateLeds();
    }
  } 
  else {
    alternateLeds();
  }
}

void alternateLeds() {
  if (millis() % ledPeriod == 0) {
    digitalWrite(ledPin, LOW);
    digitalWrite(extLedPin, HIGH);
    // digitalWrite(speakerPin, HIGH);
  }
  else if (millis() % ledPeriod == ledPeriod / 2) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(extLedPin, LOW);
    // digitalWrite(speakerPin, LOW);
  }
}
