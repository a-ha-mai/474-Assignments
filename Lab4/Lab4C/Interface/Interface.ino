/**
 * @file Interface.ino
 * @author Anna Mai (2165101)
 * @author Paria Naghavi (1441396)
 * @date 14-August-2023
 * @brief UW ECE 474 Lab 4 Assignment
 *
 * A simple game interface using FreeRTOS and LiquidCrystal library that plays the Dino Game.
 */

#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte dino [8] = { /**< dino character bitmap */
  B00000,
  B00111,
  B00101,
  B10111,
  B11100,
  B11111,
  B01101,
  B01100,
};

byte tree [8] = { /**< tree character bitmap */
  B00011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111,
  B01110,
  B01110
};

const int BUTTON_ENTER = 8; /**< The button pin for starting the game */

const int MENU_SIZE = 2; /**< The size of the menu */
const int LCD_COLUMN = 16; /**< The number of LCD columns */

const int TREE_CHAR = 6; /**< Character index for the tree bitmap in LCD */
const int DINO_CHAR = 7; /**< Character index for the dino bitmap in LCD */

TaskHandle_t gameTaskHandle; /**< Task handle for the game task */

/**
 * @brief Setup function to initialize hardware and start the FreeRTOS scheduler.
 */
void setup() {
  pinMode(BUTTON_ENTER, INPUT_PULLUP); /**< Set button pin as input with pull-up resistor */

  lcd.begin(16, 2);
  lcd.createChar(DINO_CHAR, dino); /**< Create custom character for dino */
  lcd.createChar(TREE_CHAR, tree); /**< Create custom character for tree */

  Serial.begin(9600); /**< Initialize serial communication */

  xTaskCreate(gameTask, "GameTask", 128, NULL, 1, &gameTaskHandle); /**< Create the game task */

  vTaskStartScheduler(); /**< Start the FreeRTOS scheduler */
}

/**
 * @brief Loop function. Unused in FreeRTOS.
 */
void loop() {
  // Unused in FreeRTOS
}

/**
 * @brief Task to handle the game logic.
 * 
 * @param pvParameters A pointer to task parameters.
 */
void gameTask(void *pvParameters) {
  boolean isPlaying = false; /**< Indicates if the game is currently being played */
  boolean isDinoOnGround = true; /**< Indicates if the dino is currently on the ground */

  while (1) {
    handleMenu(isPlaying);

    if (isPlaying) {
      handleGame(isDinoOnGround);
    }
  }
}

/**
 * @brief Displays the game menu and handles start button press.
 * 
 * @param isPlaying Reference to the game playing status.
 */
void handleMenu(boolean &isPlaying) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-> START");

  if (digitalRead(BUTTON_ENTER) == LOW) {
    isPlaying = true;
    lcd.clear();
  }
}

/**
 * @brief Handles the game logic during gameplay.
 * 
 * @param isDinoOnGround Reference to the dino's on-ground status.
 */
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

/**
 * @brief Handles the game over condition and displays the game over screen.
 * 
 * @param isPlaying Reference to the game playing status.
 */
void handleGameOver(boolean &isPlaying) {
  isPlaying = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER");
  vTaskDelay(pdMS_TO_TICKS(3000));
}

/**
 * @brief Displays a tree character on the LCD at the given position.
 * 
 * @param position The column position on the LCD to display the tree.
 */
void showTree(int position) {
  lcd.setCursor(position, 1);
  lcd.write(TREE_CHAR);

  lcd.setCursor(position + 1, 1);
  lcd.print(" ");
}

/**
 * @brief Defines the dino's position based on the button state.
 * 
 * @param isDinoOnGround Reference to the dino's on-ground status.
 */
void defineDinoPosition(boolean &isDinoOnGround) {
  int buttonState = digitalRead(BUTTON_ENTER);
  buttonState == HIGH ? putDinoOnGround(isDinoOnGround) : putDinoOnAir(isDinoOnGround);
}

/**
 * @brief Places the dino on the ground and updates the display.
 * 
 * @param isDinoOnGround Reference to the dino's on-ground status.
 */
void putDinoOnGround(boolean &isDinoOnGround) {
  lcd.setCursor(1, 1);
  lcd.write(DINO_CHAR);
  lcd.setCursor(1, 0);
  lcd.print(" ");

  isDinoOnGround = true;
}

/**
 * @brief Places the dino in the air and updates the display.
 * 
 * @param isDinoOnGround Reference to the dino's on-ground status.
 */
void putDinoOnAir(boolean &isDinoOnGround) {
  lcd.setCursor(1, 0);
  lcd.write(DINO_CHAR);
  lcd.setCursor(1, 1);
  lcd.print(" ");

  isDinoOnGround = false;
}
