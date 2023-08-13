#include <Arduino_FreeRTOS.h>

const int sensorPin = 0;
const int ledPin = 9;

int lightLevel;
int cactusBrightness = 550;

void sensorTask(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    lightLevel = analogRead(sensorPin);
    Serial.println(lightLevel);

    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void ledTask(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    if (lightLevel > cactusBrightness) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  xTaskCreate(sensorTask, "SensorTask", 128, NULL, 1, NULL);
  xTaskCreate(ledTask, "LedTask", 128, NULL, 2, NULL);
  
  vTaskStartScheduler();
}

void loop() {
  // Empty because the tasks are now being managed by the RTOS
}
