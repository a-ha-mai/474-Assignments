#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte dino [8] = {
  B00000,
  B00111,
  B00101,
  B10111,
  B11100,
  B11111,
  B01101,
  B01100,
};

byte tree [8] = {
  B00011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111,
  B01110,
  B01110
};

const int BUTTON_ENTER = 8;

const int MENU_SIZE = 2;
const int LCD_COLUMN = 16;

const int TREE_CHAR = 6;
const int DINO_CHAR = 7;

TaskHandle_t gameTaskHandle;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(DINO_CHAR, dino);
  lcd.createChar(TREE_CHAR, tree);

  Serial.begin(9600);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  
  xTaskCreate(gameTask, "GameTask", 128, NULL, 1, &gameTaskHandle);

  vTaskStartScheduler();
}

void loop() {
  // Unused in FreeRTOS
}

void gameTask(void *pvParameters) {
  boolean isPlaying = false;
  boolean isDinoOnGround = true;

  while (1) {
    handleMenu(isPlaying);

    if (isPlaying) {
      handleGame(isDinoOnGround);
    }
  }
}

void handleMenu(boolean &isPlaying) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-> START");

  if (digitalRead(BUTTON_ENTER) == LOW) {
    isPlaying = true;
    lcd.clear();
  }
}

void handleGame(boolean &isDinoOnGround) {
  lcd.clear();

  int secondPosition = random(4, 9);
  int thirdPosition = random(4, 9);
  int firstTreePosition = LCD_COLUMN;

  const int columnValueToStopMoveTrees = -(secondPosition + thirdPosition);

  for (; firstTreePosition >= columnValueToStopMoveTrees; firstTreePosition--) {
    defineDinoPosition(isDinoOnGround);

    int secondTreePosition = firstTreePosition + secondPosition;
    int thirdTreePosition = secondTreePosition + thirdPosition;

    showTree(firstTreePosition);
    showTree(secondTreePosition);
    showTree(thirdTreePosition);

    if (isDinoOnGround) {
      if (firstTreePosition == 1 || secondTreePosition == 1 || thirdTreePosition == 1) {
        handleGameOver(isPlaying);
        vTaskDelay(pdMS_TO_TICKS(5000));
        break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void handleGameOver(boolean &isPlaying) {
  isPlaying = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER");
  vTaskDelay(pdMS_TO_TICKS(3000));
}

void showTree(int position) {
  lcd.setCursor(position, 1);
  lcd.write(TREE_CHAR);

  lcd.setCursor(position + 1, 1);
  lcd.print(" ");
}

void defineDinoPosition(boolean &isDinoOnGround) {
  int buttonState = digitalRead(BUTTON_ENTER);
  buttonState == HIGH ? putDinoOnGround(isDinoOnGround) : putDinoOnAir(isDinoOnGround);
}

void putDinoOnGround(boolean &isDinoOnGround) {
  lcd.setCursor(1, 1);
  lcd.write(DINO_CHAR);
  lcd.setCursor(1, 0);
  lcd.print(" ");

  isDinoOnGround = true;
}

void putDinoOnAir(boolean &isDinoOnGround) {
  lcd.setCursor(1, 0);
  lcd.write(DINO_CHAR);
  lcd.setCursor(1, 1);
  lcd.print(" ");

  isDinoOnGround = false;
}
