#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <math.h>
#include <arduinoFFT.h>


#define E 659
#define C 523
#define G 784
#define g 392
#define R 0
#define SPEAKER_PIN 6
#define N 128

// for FFT
#define FFT_SIZE 128 // Start with 128 samples
double real[FFT_SIZE];
double imag[FFT_SIZE];

QueueHandle_t completionQueue;
QueueHandle_t fftQueue;
double data[N]; 


// for the sound
const int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
const int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 5};
const int melodyLength = sizeof(melody) / sizeof(melody[0]);

// define two tasks for Blink 
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );
void SpeakerTick(void *pvParameters);
void TaskRT3(void *pvParameters);
void TaskRT3p0(void *pvParameters);
void TaskRT3p1(void *pvParameters);




// the setup function runs once when you press reset or power the board
void setup() {
    pinMode(SPEAKER_PIN, OUTPUT);
    Serial.begin(9600);
    while (!Serial);

    xTaskCreate(TaskBlink, "FlashLED", 128, NULL, 1, NULL);
    xTaskCreate(SpeakerTick, "Speaker", 128, NULL, 1, NULL);
    //xTaskCreate(TaskRT3, "RT-3", 1000, NULL, 2, NULL);
    xTaskCreate(TaskRT3p0, "RT3p0", 2048, NULL, 2, NULL);
    xTaskCreate(TaskRT3p1, "FFTTask1", 1024, NULL, 2, NULL);
    //xTaskCreate(TaskRT4, "FFTTask2", 1024, NULL, 2, NULL);

    vTaskStartScheduler();
}

void loop() {
    // Empty. All logic is managed by FreeRTOS tasks.
}

void TaskBlink(void *pvParameters) {
    pinMode(13, OUTPUT);   

    for (;;) {
        digitalWrite(13, HIGH);   // LED ON
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms

        digitalWrite(13, LOW);    // LED OFF
        vTaskDelay(pdMS_TO_TICKS(200)); // Delay for 200ms
    }
}


void initializeTimer4PWM() {
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  TCCR4A |= (1 << COM4A0);
  TCCR4B |= (1 << CS41) | (1 << CS40);

}



void SpeakerTick(void *pvParameters) {
    initializeTimer4PWM();

    static long currentTime = 0;
    static int noteIndex = 0;

    for (;;) {
        int noteDuration = 4 * beats[noteIndex];
        int freq = melody[noteIndex];

        // Check if the current note's duration has elapsed
        if ((xTaskGetTickCount() - currentTime) >= noteDuration) {
            Serial.print("Playing note index: "); Serial.println(noteIndex); // This line will print the current note index to the serial monitor
            noteIndex = (noteIndex + 1) % melodyLength;
            currentTime = xTaskGetTickCount();
        }

        if (freq == 0) {
            OCR4A = 0;
        } else {
            OCR4A = (F_CPU / (64UL * freq)) / 2;
        }

        vTaskDelay(1); // A slight delay to reduce CPU usage
    }
}


void TaskRT3p0(void *pvParameters) {
    // Generates an array of N pseudo-random doubles
    for (int i = 0; i < N; i++) {
        data[i] = (double)random(-1000, 1000) / 1000; // random double between -1 and 1
    }

    // Initialize a FreeRTOS Queue
    fftQueue = xQueueCreate(1, sizeof(double *));
    completionQueue = xQueueCreate(5, sizeof(int)); 

    // Starts RT3p1 and halts itself
    xTaskCreate(TaskRT3p1, "RT3p1", 2048, NULL, 2, NULL);
    vTaskDelete(NULL);
}

void TaskRT3p1(void *pvParameters) {
    int elapsedTime = 0;
    int totalElapsedTime = 0;

    for (int i = 0; i < 5; i++) {
        // Send a pointer to the data to task RT-4
        double *dataPointer = data;
        xQueueSend(fftQueue, &dataPointer, portMAX_DELAY);

        // Block and wait for the FFT to complete
        xQueueReceive(completionQueue, &elapsedTime, portMAX_DELAY);
        totalElapsedTime += elapsedTime;
    }

    // Report back to your laptop/PC
    Serial.print("Total wall clock time elapsed for the 5 FFTs: ");
    Serial.print(totalElapsedTime);
    Serial.println(" ticks");

    vTaskDelete(NULL);
}
void TaskRT4(void *pvParameters) {
    arduinoFFT FFT = arduinoFFT(); // Create FFT object

    double *receivedData;
    double real[N]; // Assuming N is the size of your data array
    double imag[N];

    while (1) {
        xQueueReceive(fftQueue, &receivedData, portMAX_DELAY);

        for (int i = 0; i < N; i++) {
            real[i] = receivedData[i];
            imag[i] = 0; // Initialize imaginary part to zero
        }

        TickType_t startTick = xTaskGetTickCount(); // Get start tick count
        FFT.Compute(real, imag, N, FFT_FORWARD); // Perform FFT
        TickType_t endTick = xTaskGetTickCount(); // Get end tick count

        int elapsedTime = endTick - startTick; // Calculate elapsed time
        xQueueSend(completionQueue, &elapsedTime, portMAX_DELAY); // Send elapsed time back to RT-3
    }
}
