/*
put lights under board and light em up according to wheel speed

TODO configure unused digital pins to OUTPUT to save power

FastLED.setBrightness TO 200 (of 255) MAXIMUM, ON ALL WHITE THIS USES 2A FOR 76 LEDS

uint8_t is unsigned char;
int8_t is signed char;
uint16_t is unsigned short;
int16_t is short;
uint32_t is unsigned int;
int32_t is int

*/
#define DEBUG 1
#define MAX_BRIGHTNESS 200  // At 77 LEDs full white this pulls 2A, max is 2.1A
                            // Change brightness with ledBrightness below, not this define


#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>
#include "SpeedCalculation.h"


const unsigned short numled = 77;
const unsigned char ledBrightness = 200;  // Value 0-255
const unsigned char pinToLeds = 24;   // Usable pins(Teensy LC):   1, 4, 5, 24. The chosen pin must be physically 
                                      // connected to pin 17 with a jumper cable to use the 5V buffer. Pin "17-5V"
                                      // can then be connected to the DATA pin on the LED strip

const float patternLength = 1000.0;  // Length in mm before the led pattern repeats


const unsigned char pinHallSpeed = 14;       // Connects to pin 2 of TLE4966, will be used as interrupt
const unsigned char pinHallDirection = 15;   // Connects to pin 3 of TLE4966

const unsigned char timestampsSaveNb = 12;  // max 255
const unsigned int ledUpdateDelay = 10000;  // in microseconds
const float ledSpacingMM = 16.1;            // Space between each LED on LED strip
const float wheelDiameterMM = 50.8;         // wheel diameter in mm
const unsigned char pulsesPerRotation = 6;  //how many pulses we expect the hall effect sensor to give
                                            // after one wheel rotation


TimestampBuffer     timestampBuffer;  // has volatile elements, keeps the result of micros() each time we get a pulse
TimestampBufferCopy timestampBufferCopy; // copy of timestampBuffer that is unaffected by interrupts
elapsedMicros sinceLastFrame;


//byte drawingMemory[numled*3];         //  3 bytes per LED
//DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

//WS2812Serial leds(numled, displayMemory, drawingMemory, pinToLeds, WS2812_GRB);

// Define the array of leds
CRGB leds[numled];

#if DEBUG
int loopCount = 0;
#endif


/* Interrupt function, called on falling edge of speed signal from TLE4966
 *  Update the direction value. We save a timestamp in the circular buffer 
 *  and increase the counter to write at the next bin next time. Reset bin 
 *  count if over the max. Add to the pulse count
 */
void saveMicrosToBuffer(){ //TODO if timestamps not initialized problem on timestamp[0]?
  timestampBuffer.currentDirection = digitalRead(pinHallDirection);
  timestampBuffer.currentBin++;
  if (timestampBuffer.currentBin >= timestampBuffer.buffSize){
    timestampBuffer.currentBin = 0;
  }
  timestampBuffer.timestamps[timestampBuffer.currentBin] = micros();
  timestampBuffer.pulseCount++;
}


void setup() {
  
  // Setup the timestamp buffer and its copy
  setupBuffer(&timestampBuffer,     timestampsSaveNb);
  setupBufferCopy(&timestampBufferCopy, timestampsSaveNb);

#if DEBUG
  Serial.begin(9600);  // USB, speed that is entered here doesn't matter, will always be USB speed
#endif

  // Attach interrupt
  pinMode(pinHallSpeed, INPUT_PULLUP);
  pinMode(pinHallDirection, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinHallSpeed), saveMicrosToBuffer, FALLING);
  

  // LEDs 
  LEDS.addLeds<WS2812SERIAL,pinToLeds,GRB>(leds,numled);
  if (ledBrightness > MAX_BRIGHTNESS){
    LEDS.setBrightness(MAX_BRIGHTNESS);
  }
  else{
    LEDS.setBrightness(ledBrightness);
  }
  //leds.begin();

}


void loop() {
  //Check if 1/60th of a second has passed
  if (sinceLastFrame > ledUpdateDelay){
    noInterrupts();
    copyTimestampBuffer(&timestampBuffer, &timestampBufferCopy);
    interrupts();
#if DEBUG
    /*
    Serial.print("speed pin ");
    Serial.println(digitalRead(pinHallSpeed));
    Serial.print("dir pin ");
    Serial.println(digitalRead(pinHallDirection));
    Serial.print("interrupt count ");
    Serial.println(testCount);
    */
    
    /*
    Serial.print(loopCount, DEC);
    Serial.print(" dir ");
    Serial.print(timestampBuffer.currentDirection, DEC);
    Serial.print(" last timestamp ");
    Serial.println(timestampBuffer.timestamps[timestampBuffer.currentBin], DEC);
    */
    
    Serial.print("dir ");
    Serial.print(digitalRead(pinHallDirection), DEC);
    Serial.print(" speed ");
    Serial.println(digitalRead(pinHallSpeed) + 2, DEC);

    loopCount++;
#endif

    

    //Calculate new colors positions 
    //float distance = distanceTravelled(timestampBufferCopy, ledUpdateDelay, wheelDiameterMM, pulsesPerRotation);
    

    //Update the LEDs
    //in fastLED, could be: leds[i].setHue( 160);
    FastLED.show();

    //Reset the timer for next frame
    sinceLastFrame = 0;
  }

}
