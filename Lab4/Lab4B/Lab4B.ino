/**
 * @file Lab4B.ino
 * @author Anna Mai (2165101)
 * @author Paria Naghavi (1441396)
 * @date 14-August-2023
 * @brief UW ECE 474 Lab 4 Assignment
 * 
 *
 * This file contains a scheduler based in Free RTOS that manage the execution of following tasks:
 * - Flashing an LED on for 250ms and off for 750ms.
 * - Playing the intro to the Super Mario Bros. theme song repeatedly, with 4 seconds of silence between each play.
 * - FFT processing of pseudo-random data and measuring computation time.
 */

#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <math.h>
#include <arduinoFFT.h>
arduinoFFT FFT;

#define LED_PIN 49

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
TaskHandle_t R3;
TaskHandle_t R4;


QueueHandle_t fftQueue = xQueueCreate(1, sizeof(double *));
QueueHandle_t completionQueue = xQueueCreate(5, sizeof(int)); 

double data[N]; 


// for the sound
const int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, g, R};
const int beats[]  = {5, 1, 5, 1, 5, 5, 1, 5, 5, 1, 5, 1, 5, 5, 1, 5, 5, 5, 5};
const int melodyLength = sizeof(melody) / sizeof(melody[0]);
const double sampleFreq = 5000;

// define two tasks for Blink 
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );
void SpeakerTick(void *pvParameters);
void TaskRT3(void *pvParameters);
void TaskRT3p0(void *pvParameters);
//void TaskRT3p1(void *pvParameters);




/**
 * @brief Setup function to initialize hardware and start the FreeRTOS scheduler.
 */
void setup() {
    pinMode(SPEAKER_PIN, OUTPUT);
    Serial.begin(19200);
    while (!Serial);

    xTaskCreate(TaskBlink, "FlashLED", 128, NULL, 1, NULL);
    xTaskCreate(SpeakerTick, "Speaker", 128, NULL, 1, NULL);
    //xTaskCreate(TaskRT3, "RT-3", 1000, NULL, 2, NULL);
    xTaskCreate(TaskRT3p0, "RT3p0", 2048, NULL, 2, &R3);
    //xTaskCreate(TaskRT3p1, "FFTTask1", 1024, NULL, 2, NULL);
    xTaskCreate(TaskRT4, "FFTTask2", 2048, NULL, 2, &R4);

    vTaskStartScheduler();
}

void loop() {
    // Empty. All logic is managed by FreeRTOS tasks.
}

/**
 * @brief Flashes the LED connected to pin 49 on for 100ms and off for 200ms
 */
void TaskBlink(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);   

    for (;;) {
        digitalWrite(LED_PIN, HIGH);   // LED ON
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms

        digitalWrite(LED_PIN, LOW);    // LED OFF
        vTaskDelay(pdMS_TO_TICKS(200)); // Delay for 200ms
    }
}

/**
 * @brief Initilizes the timer 4 for the speaker 
 */
void initializeTimer4PWM() {
  TCCR4A = (1 << WGM41) | (1 << WGM40);
  TCCR4B = (1 << WGM43) | (1 << WGM42);
  TCCR4A |= (1 << COM4A0);
  TCCR4B |= (1 << CS41) | (1 << CS40);

}

/**
 * @brief A FreeRTOS task to handle playing sound through a speaker.
 *
 * The function uses Timer 4 Pulse Width Modulation (PWM) to control a speaker's sound output.
 * It will cycle through a pre-defined melody and play each note in sequence,
 * taking into account the note's frequency and duration.
 *
 * @param[in] pvParameters A pointer to parameters which is NULL for this function.
 */

void SpeakerTick(void *pvParameters) {
    // Initialize Timer 4 for PWM to control the speaker.
    initializeTimer4PWM(); 
    // Store the last time a note started playing. Type static ensures that the value persists across function calls.
    static long currentTime = 0;
    // Static variable keeps track of which note in the melody is currently playing.
    static int noteIndex = 0;
    // Infinite loop definition of FreeRTOS that keeps running.
    for (;;) {
        // Calculate how long the current note should play, using beats[]'s elements indicating duration for each note.
        int noteDuration = 4 * beats[noteIndex];
        // Find out the frequency of the current note from the melody[].
        int freq = melody[noteIndex];
        // Check if the current note's duration has elapsed.
        // xTaskGetTickCount() returns the current tick count since the system started.
        if ((xTaskGetTickCount() - currentTime) >= noteDuration) {
          // Print the note index to the serial monitor for debugging.
            Serial.print("Playing note index: "); Serial.println(noteIndex); // This line will print the current note index to the serial monitor
          // Move to the next note. If we're at the end of the melody, start from the beginning.
            noteIndex = (noteIndex + 1) % melodyLength;
          // Update the currentTime to the current tick count.
            currentTime = xTaskGetTickCount();
        }
        // Set the output for OCR4A which is the PWM for the speaker.
        // If the frequency is 0 is rest with no sound produced.
        if (freq == 0) {
            OCR4A = 0;
        // Calculate the timer compare value based on the desired frequency.
        } else {
            OCR4A = (F_CPU / (64UL * freq)) / 2;
        }
        // A small delay in each iteration to prevent the task from continuously hogging the CPU allowing other tasks to run.
        vTaskDelay(1); 
    }
}

/**
 * @brief Task responsible for generating sample data, sending it for FFT processing, and calculating total elapsed time.
 * 
 * It generates an array of pseudo-random double values between 100 and 200. This data is sent to TaskRT4 for FFT processing.
 *  TaskRT3p0 waits for the FFT processing completion time feedback from TaskRT4, which is accumulated to calculate the total elapsed
 * time. After five iterations, the task deletes itself.
 * 
 * @param pvParameters Pointer to parameters passed to the task during its creation. 
 * 
 * @note TTaskRT3p0 will suspend itself after sending data and will be resumed by TaskRT4 after FFT processing.
 *       The fftQueue array is used for sending data to TaskRT4, and  completionQueue for receiving FFT computation time feedback.
 */

void TaskRT3p0(void *pvParameters) {

    // Generates an array of N pseudo-random doubles between 100 and 200
    for (int i = 0; i < N; i++) {
        data[i] = (double)random() / RAND_MAX*(100-0)+100; 
 
    }
    // Define two variables to track elapsed time
    TickType_t elapsedTime = 0;
    TickType_t totalElapsedTime = 0;
    // Loop 5 times
    for (int i = 0; i < 5; i++) {
        // Send a pointer to the data to task 4
        if (xQueueSend(fftQueue, &data, pdMS_TO_TICKS(100)) == pdPASS) {
           Serial.println("Data was sent to FFT ");
           vTaskResume(R4);// Resume task 4
           vTaskSuspend(R3); // Task 3 suspends itself 

        }
        // Delay for a message from task 4 about the FFT completion time
         if  (xQueueReceive(completionQueue, &elapsedTime, portMAX_DELAY) == pdTRUE){

         totalElapsedTime= totalElapsedTime+ elapsedTime;
          Serial.println( (totalElapsedTime*1000)/configTICK_RATE_HZ);
         }
    }
    // Delete this task
    vTaskDelete(NULL);
   
}

/**
 * @brief Task responsible for receiving pseudo-random data, processing it using FFT, and returning the computation time.
 * 
 * TaskRT4 waits to receive data sent by TaskRT3p0. Once received, it prepares for FFT by setting the imaginary parts to zero. 
 * The FFT is initialized with the received data and a Hamming window is applied. 
 * It then measures the time taken to compute the FFT. After processing, the computation time is sent back
 * to TaskRT3p0. The task performs these operations in five iterations, and after each iteration, it suspends itself.
 * 
 * @param pvParameters Pointer to parameters passed to the task during its creation. 
 * 
 * @note TaskRT4 will suspend itself after sending the computation time and will be resumed again by TaskRT3p0 
 *       after the data is sent for the next FFT processing. The fftQueue is used for receiving data from TaskRT3p0, 
 *       and completionQueue for sending FFT computation time feedback.
 */
void TaskRT4(void *pvParameters) {
 // Define a variable to store the computation time
    TickType_t comp = 0;
    double receivedData[N]; // An array to store received data
    double real[N]; // Array for real parts of FFT process
    double imag[N]; // Array for imaginary parts of FFT process
    
    // Initialize the imaginary part to all zeros
    for (int i=0; i< N ; i++){
      imag[i]=0.0;
    }
   // Loop 5 times 
   for (int i = 0; i < 5; i++) {
     // Wait indefinitely for data from task 3
       if  (xQueueReceive(fftQueue, &receivedData, portMAX_DELAY) == pdTRUE){
          Serial.println("Data was recieved from Task 3 ");
          FFT = arduinoFFT (receivedData, imag, N, sampleFreq);// Initialize FFT with received data
          FFT.Windowing (FFT_WIN_TYP_HAMMING, FFT_FORWARD); // Hamming window
          TickType_t T1= xTaskGetTickCount(); // Get current tick count before FFT computation
          FFT.Compute(FFT_FORWARD); // Perform FFT
          TickType_t T2= xTaskGetTickCount(); // Get current tick count after FFT computation
          comp= T2- T1;  // Compute the time taken for FFT

       }
        // Send the computation time back to task 3 to print 
       if(xQueueSend(completionQueue, &comp, pdMS_TO_TICKS(100)) == pdPASS){
          Serial.println("Data was sent to Task 3 ");
          vTaskResume(R3); // Resume task RT-3
          vTaskSuspend(R4); // Suspend task 4


         }  }
}
