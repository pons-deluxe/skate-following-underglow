/*
put lights under board and light em up according to wheel speed


uint8_t is unsigned char;
int8_t is signed char;
uint16_t is unsigned short;
int16_t is short;
uint32_t is unsigned int;
int32_t is int

*/
#define DEBUG 1


#include <WS2812Serial.h>
#include "SpeedCalculation.h"


const unsigned short numled = 180;
const unsigned char pinToLeds = 24;   // Usable pins(Teensy LC):   1, 4, 5, 24. The chosen pin must be physically 
                                      // connected to pin 17 with a jumper cable to use the 5V buffer. Pin "17-5V"
                                      // can then be connected to the DATA pin on the LED strip

const unsigned char pinHallSpeed = 1;       // Connects to pin 2 of TLE4966, will be used as interrupt
const unsigned char pinHallDirection = 1;   // Connects to pin 3 of TLE4966

const unsigned char timestampsSaveNb = 12;  // max 255
const unsigned int ledUpdateDelay = 16666;  // in microseconds
const unsigned char pulsesPerRotation = 6;  //how many pulses we expect the hall effect sensor to give
                                            // after one wheel rotation


TimestampBuffer timestampBuffer;  // has volatile elements
elapsedMicros sinceLastFrame;


byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pinToLeds, WS2812_GRB);


/* Interrupt function, called on falling edge of speed signal from TLE4966
 *  Update the direction value. We save a timestamp in the circular buffar 
 *  and increase the counter to writeat the next bin next time. Reset bin 
 *  count if over the max.
 */
void saveMicrosToBuffer(){
  timestampBuffer.currentDirection = digitalRead(pinHallDirection);
  timestampBuffer.timestamps[timestampBuffer.currentBin] = micros();
  timestampBuffer.currentBin++;
  if (timestampBuffer.currentBin >= timestampBuffer.buffSize){
    timestampBuffer.currentBin = 0;
  }
}

void setup() {
  
  // Setup the timestamp buffer
  setupBuffer(timestampBuffer, timestampsSaveNb);

#if DEBUG
  Serial.begin(9600);
#endif

  // Attach interrupt
  pinMode(pinHallSpeed, INPUT);
  pinMode(pinHallDirection, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinHallSpeed), saveMicrosToBuffer, FALLING);
  

  // LEDs 
  leds.begin();
  

  
}

void loop() {
  //Check if 1/60th of a second has passed
  if (sinceLastFrame > ledUpdateDelay){
    //Calculate new colors postions 

    //Update the LEDs

    //Reset the timer for next frame
    sinceLastFrame = 0;
  }

}
