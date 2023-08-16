/**
 * @file Player.ino
 *
 * @brief Light Sensing and LED Control using FreeRTOS
 */

#include <Arduino_FreeRTOS.h>

const int sensorPin = 0; /**< The analog pin connected to the light sensor */
const int ledPin = 9;    /**< The digital pin connected to the LED */
const int cactusBrightness = 550; /**< The threshold brightness level for LED activation */

int lightLevel; /**< Current light level reading from the sensor */

/**
 * @brief Task to read the light level from the sensor and print it to the serial monitor.
 * 
 * @param pvParameters A pointer to task parameters (unused).
 */
void sensorTask(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    lightLevel = analogRead(sensorPin);
    Serial.println(lightLevel);

    vTaskDelay(pdMS_TO_TICKS(100)); /**< Delay for 100 milliseconds */
  }
}

/**
 * @brief Task to turn off or on the LED based on the light level reading.
 * 
 * @param pvParameters A pointer to task parameters (unused).
 */
void ledTask(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    if (lightLevel > cactusBrightness) {
      digitalWrite(ledPin, HIGH); /**< Turn on the LED */
    } else {
      digitalWrite(ledPin, LOW);  /**< Turn off the LED */
    }

    vTaskDelay(pdMS_TO_TICKS(100)); /**< Delay for 100 milliseconds */
  }
}

/**
 * @brief Setup function to initialize hardware and start the FreeRTOS scheduler.
 */
void setup() {
  pinMode(ledPin, OUTPUT); /**< Set the LED pin as output */
  Serial.begin(9600); /**< Initialize serial communication */

  xTaskCreate(sensorTask, "SensorTask", 128, NULL, 1, NULL); /**< Create the sensor task */
  xTaskCreate(ledTask, "LedTask", 128, NULL, 2, NULL); /**< Create the LED task */
  
  vTaskStartScheduler(); /**< Start the FreeRTOS scheduler */
}

/**
 * @brief Loop function. Unused in FreeRTOS.
 */
void loop() {
  // Unused in FreeRTOS
}
