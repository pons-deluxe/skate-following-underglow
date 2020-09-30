/*
put lights under board and light em up according to wheel speed


uint8_t is unsigned char;
int8_t is signed char;
uint16_t is unsigned short;
int16_t is short;
uint32_t is unsigned int;
int32_t is int


*/
#define TIMESTAMPS_TO_SAVE 12 // max 255
#define LED_UPDATE_DELAY 16666 //in microseconds
#define PULSES_PER_ROTAION 6 //how many pulses we expect the hall effect sensor to give
                              // after one wheel rotation


#include "SpeedCalculation.h"



TimestampBuffer timestampBuffer;
elapsedMicros sinceLastFrame;



void setup() {
  noInterrupts();
  
  //Setup the timestamp buffer
  setupBuffer(timestampBuffer, TIMESTAMPS_TO_SAVE);
  





  interrupts();
}

void loop() {
  //Check if 1/60th of a second has passed
  if (sinceLastFrame > LED_UPDATE_DELAY){
    //Calculate new colors postions 

    //Update the LEDs

    //Reset the timer for next frame
    sinceLastFrame = 0;
  }

}
