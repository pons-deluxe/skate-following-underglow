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
#include <stdint.h>
#include "SpeedCalculation.h"
#include "LedPatterns.h"



const uint16_t numled = 76;
const uint8_t pinToLeds = 1;   // Usable pins(Teensy LC):   1, 4, 5, 24. The chosen pin must be physically 
                                      // connected to pin 17 with a jumper cable to use the 5V buffer. Pin "17-5V"
                                      // can then be connected to the DATA pin on the LED strip

const float patternLengthMM = 1000.0;  // Length in mm before the led pattern repeats
const uint16_t boardTipLedNum = 26;  // The number of the LED at the tip of the board (zero indexed)
const bool doubleLedOnTip = true;  // 2 LEDs at front tip of board?


const uint8_t pinHallSpeed = 15;       // Connects to pin 2 of TLE4966, will be used as interrupt
const uint8_t pinHallDirection = 14;   // Connects to pin 3 of TLE4966
const uint8_t brightnessPin = A6;      // Connects to middle pin of pot 1
const uint8_t brightnessHysteresis = 1;  // how much the ADC value needs to change before we actually call setBrightness();

const uint8_t timestampsSaveNb = 12;  // max 255
const uint32_t ledUpdateDelay = 10000;  // in microseconds
const float ledSpacingMM = 16.1;            // Space between each LED on LED strip
const float wheelDiameterMM = 50.8;         // wheel diameter in mm
const uint8_t pulsesPerRotation = 6;  //how many pulses we expect the hall effect sensor to give
                                      // after one wheel rotation


                                            
                                            
TimestampBuffer     timestampBuffer;  // has volatile elements, keeps the result of micros() each time we get a pulse
TimestampBufferCopy timestampBufferCopy; // copy of timestampBuffer that is unaffected by interrupts
elapsedMicros sinceLastFrame;

//float distancePerPulse = 3.1416 * wheelDiameterMM / pulsesPerRotation;
// Work with units of 0.1 mm and keep everything int instead of float.
int32_t distancePerPulseUnits =  (int32_t)(3.1416 * wheelDiameterMM / pulsesPerRotation * 10);  // Units are steps of 0.1 mm
int32_t ledSpacingUnits = (int32_t)(ledSpacingMM * 10);
//uint32_t patternLengthUnits = (uint32_t)(patternLengthMM * 10);
uint32_t patternLengthLeds = (uint32_t)(patternLengthMM / ledSpacingMM);
int32_t leadingPos = 0;  // Position of the leading LED in the pattern, 0 to (patternLengthLeds - 1)
int16_t ledBrightness;  // Saved value of brightness to compare to new reading before actually changing the brightness.

//byte drawingMemory[numled*3];         //  3 bytes per LED
//DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

//WS2812Serial leds(numled, displayMemory, drawingMemory, pinToLeds, WS2812_GRB);

// Define the array of leds
CRGB leds[numled];

#if DEBUG
int32_t loopCount = 0;
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
  Serial.print("dir ");
  Serial.print(20, DEC);
  Serial.print(" speed ");
  Serial.println(22, DEC);
#endif

  // Attach interrupt
  pinMode(pinHallSpeed, INPUT_PULLUP);
  pinMode(pinHallDirection, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinHallSpeed), saveMicrosToBuffer, FALLING);


  // LEDs 
  pinMode(pinToLeds, OUTPUT);
  pinMode(17, INPUT);
  LEDS.addLeds<WS2812SERIAL,pinToLeds,GRB>(leds,numled);
  pinMode(brightnessPin, INPUT);
  ledBrightness = analogRead(brightnessPin);
  LEDS.setBrightness(map(ledBrightness, 0, 1023, 0, MAX_BRIGHTNESS));  // Map ADC value (0-1023) to brightness value (0-MAX)
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

    
    // Adjust brightness if ADC has significant change.
    int16_t newBrightness = analogRead(brightnessPin);
#if DEBUG
    Serial.print("new ");
    Serial.print(newBrightness, DEC);
    Serial.print("old ");
    Serial.println(ledBrightness, DEC);
#endif
    if( abs(newBrightness - ledBrightness) > brightnessHysteresis ){
#if DEBUG
      Serial.println("change");
#endif
      ledBrightness = newBrightness;
      LEDS.setBrightness(map(newBrightness, 0, 1023, 0, MAX_BRIGHTNESS));  // Map ADC value (0-1023) to brightness value (0-MAX)
    }
    

    //Calculate new colors positions 
    int32_t distance = distanceTravelledUnits(timestampBufferCopy, ledUpdateDelay, distancePerPulseUnits);
    int32_t pixelShift = distanceToPixCount(distance, ledSpacingUnits);
    leadingPos += pixelShift;
    leadingPos = (leadingPos < 0)? (leadingPos + patternLengthLeds) : leadingPos;
#if DEBUG
    //Serial.print(" distance ");
    //Serial.println(distance, DEC);
#endif

    //Update the LEDs
    rainbowPattern(leds, numled, leadingPos, patternLengthLeds, boardTipLedNum, doubleLedOnTip);
    //testFrontBackPattern(leds, numled, leadingPos, patternLengthLeds, boardTipLedNum, doubleLedOnTip);
    FastLED.show();

    //Reset the timer for next frame
    sinceLastFrame = 0;
  }
}
