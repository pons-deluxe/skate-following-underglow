/*
put lights under board and light em up according to wheel speed

TODO configure unused digital pins to OUTPUT to save power

FastLED.setBrightness TO 200 (of 255) MAXIMUM, ON ALL WHITE THIS USES 2A FOR 76 LEDS

*/
#define DEBUG 0
#define MAX_BRIGHTNESS 200  // At 77 LEDs full white this pulls 2A, max is 2.1A
                            // Change brightness with ledBrightness below, not this define


#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>
#include <stdint.h>
#include "SpeedCalculation.h"
#include "LedPatterns.h"


/*--------------------------------Settings-----------------------------------*/

const uint16_t numled = 76;
const uint8_t pinToLeds = 1;   // Usable pins(Teensy LC):   1, 4, 5, 24. The chosen pin must be physically 
                                      // connected to pin 17 with a jumper cable to use the 5V buffer. Pin "17-5V"
                                      // can then be connected to the DATA pin on the LED strip

const float patternLengthMM = 1000.0;  // Length in mm before the led pattern repeats
const uint16_t boardTipLedNum = 26;  // The number of the LED at the tip of the board (zero indexed)
const bool doubleLedOnTip = true;  // 2 LEDs at front tip of board?


const uint8_t pinHallSpeed = 11;       // Connects to pin 2 of TLE4966, will be used as interrupt
const uint8_t pinHallDirection = 12;   // Connects to pin 3 of TLE4966
const uint8_t brightnessPin = A0;      // Connects to middle pin of pot 1
const uint8_t animSelectPin = A1;      // Connects to middle pin of pot 2
const uint8_t brightnessHysteresis = 10;  // how much the ADC value needs to change before we actually call setBrightness();
const uint8_t animSelectHysteresis = 20;  // how much the ADC value needs to change the selection value changes;

const uint8_t animationNb = 6;  // number of selectable animations
const uint8_t timestampsSaveNb = 12;  // max 255
const uint32_t ledUpdateDelay = 10000;  // in microseconds
const float ledSpacingMM = 16.1;            // Space between each LED on LED strip
const float wheelDiameterMM = 50.8;         // wheel diameter in mm
const uint8_t pulsesPerRotation = 6;  //how many pulses we expect the hall effect sensor to give
                                      // after one wheel rotation


                                            
/*-----------------------------Global variables------------------------------*/

Board board;  // Contains info like total number of LEDs and which LED is at the tip of the board.
TimestampBuffer     timestampBuffer;  // has volatile elements, keeps the result of micros() each time we get a pulse
TimestampBufferCopy timestampBufferCopy; // copy of timestampBuffer that is unaffected by interrupts
uint32_t sinceLastFrame = 0;

//float distancePerPulse = 3.1416 * wheelDiameterMM / pulsesPerRotation;
// Work with units of 0.1 mm and keep everything int instead of float.
int32_t distancePerPulseUnits =  (int32_t)(3.1416 * wheelDiameterMM / pulsesPerRotation * 10);  // Units are steps of 0.1 mm
int32_t ledSpacingUnits = (int32_t)(ledSpacingMM * 10);
//uint32_t patternLengthUnits = (uint32_t)(patternLengthMM * 10);
uint32_t patternLengthLeds = (uint32_t)(patternLengthMM / ledSpacingMM);
int32_t leadingPos = 0;  // Position of the leading LED in the pattern, 0 to (patternLengthLeds - 1)
int16_t ledBrightness;  // Saved value of brightness pot ADC to compare to new reading before actually changing the brightness.
int16_t animSelectVal;  // Saved value of animation selection pot ADC  to compare to new reading before actually changing the animation.
uint8_t animSelect;  // The number of the selected animation
bool animChange = 1;  // Flag set if the animation selected is different from last frame. A 1 signifies all LED values need to be calculated.

//byte drawingMemory[numled*3];         //  3 bytes per LED
//DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

//WS2812Serial leds(numled, displayMemory, drawingMemory, pinToLeds, WS2812_GRB);

// Define the array of leds
CRGB leds[numled];

#if DEBUG
int32_t loopCount = 0;
#endif


/*---------------------------------Interrupt---------------------------------*/

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


/*-----------------------------------Setup-----------------------------------*/

void setup() {
  
  // Setup the board info
  board = setupBoard(numled, boardTipLedNum, doubleLedOnTip);
  
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
  LEDS.addLeds<WS2812SERIAL,pinToLeds,BRG>(leds,numled);  // amazon says GRB, it is not.

  // First brightness selection
  pinMode(brightnessPin, INPUT);
  delay(250);
  ledBrightness = analogRead(brightnessPin);
  uint32_t scaledBrightness = uint32_t(ledBrightness) * uint32_t(ledBrightness) >> 10; // Scale to a more parabola-ish shape
  LEDS.setBrightness(map(scaledBrightness, 0, 1023, 1, MAX_BRIGHTNESS));  // Map ADC value (0-1023) to brightness value (0-MAX)

  // First Animation selection
  pinMode(animSelectPin, INPUT);
  delay(250);
  
  animSelectVal = analogRead(animSelectPin);
  
  animSelect = (animSelectVal + 1023/(2*(animationNb - 1))) / (1023 / (animationNb - 1));
  
#if DEBUG
  Serial.print("val ");
  Serial.print(animSelectVal, DEC);
  Serial.print(" animSelect ");
  Serial.println(animSelect, DEC);
  
#endif
}


/*-----------------------------------Loop------------------------------------*/

void loop() {
  //Check if 1/60th of a second has passed
  int32_t currentTime = micros();
  if (currentTime - sinceLastFrame > ledUpdateDelay){
    sinceLastFrame = currentTime;
    
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
    
    /*
    Serial.print("dir ");
    Serial.print(digitalRead(pinHallDirection), DEC);
    Serial.print(" speed ");
    Serial.println(digitalRead(pinHallSpeed) + 2, DEC);
    */

    loopCount++;
#endif

    
    /*----------Brightness ajustment----------*/
    
    int16_t newBrightness = analogRead(brightnessPin);
#if DEBUG
    /*
    Serial.print("new ");
    Serial.print(newBrightness, DEC);
    Serial.print(" old ");
    Serial.print(ledBrightness, DEC);
    Serial.print(" mapped ");
    uint8_t scaledBrightness = map(ledBrightness, 0, 1023, 0, 255);
    scaledBrightness = scale8(scaledBrightness,scaledBrightness);
    Serial.println(map(scaledBrightness, 0, 255, 0, MAX_BRIGHTNESS), DEC);
    */
#endif
    if( abs(newBrightness - ledBrightness) > brightnessHysteresis ){
#if DEBUG
      //Serial.println("change");
#endif
      ledBrightness = newBrightness;
      //uint8_t scaledBrightness = map(newBrightness, 0, 1023, 0, 255);
      //scaledBrightness = scale8(scaledBrightness,scaledBrightness);
      //LEDS.setBrightness(map(scaledBrightness, 0, 255, 1, MAX_BRIGHTNESS));  // Map ADC value (0-1023) to brightness value (0-MAX)
      //LEDS.setBrightness(map(ledBrightness, 0, 1023, 1, MAX_BRIGHTNESS));  // Map ADC value (0-1023) to brightness value (0-MAX)
      uint32_t scaledBrightness = uint32_t(ledBrightness) * uint32_t(ledBrightness) >> 10; // Scale to a more parabola-ish shape
      LEDS.setBrightness(map(scaledBrightness, 0, 1023, 1, MAX_BRIGHTNESS));  // Map ADC value (0-1023) to brightness value (0-MAX)
    }



    /*----------Animation selection----------*/
    
    int16_t newAnimSelectVal = analogRead(animSelectPin);
#if DEBUG
    /*
    Serial.print("new ");
    Serial.println(newAnimSelectVal, DEC);
    */
#endif
    if( abs(newAnimSelectVal - animSelectVal) > animSelectHysteresis ){

      animSelectVal = newAnimSelectVal;
      //uint8_t newAnimSelect = animSelectVal / (1023 / animationNb);
      // Get animation value while giving less range to first and last value
      newAnimSelectVal += 1023/(2*(animationNb - 1));
      uint8_t newAnimSelect = newAnimSelectVal / (1023 / (animationNb - 1));
      
      if (animSelect != newAnimSelect){
        animSelect = newAnimSelect;
        animChange = 1;  // Set the flag to indicate animation has changed.
      }
#if DEBUG
      Serial.print(" select ");
      Serial.println(animSelect, DEC);
#endif
    }

    
    
    /*----------Distance calculation----------*/
    
    int32_t distance = distanceTravelledUnits(timestampBufferCopy, ledUpdateDelay, distancePerPulseUnits);
    int32_t pixelShift = distanceToPixCount(distance, ledSpacingUnits);
    //leadingPos += pixelShift;
    //leadingPos = (leadingPos < 0)? (leadingPos + patternLengthLeds) : leadingPos;
#if DEBUG
    //Serial.print(" distance ");
    //Serial.println(distance, DEC);
#endif


    /*----------LED update----------*/

    switch(animSelect) {
      case 0:
        testFrontBackPattern(leds, board);
        break;
      case 1:
        rainbowPattern(leds, board, pixelShift, 720);
        break;
      case 2:
        {
          uint8_t hues[2] = {197, 162};
          multiFillPattern(leds, board, pixelShift, 100, 30, hues, 2);
        }
        break;
      case 3:
        theaterLights(leds, board, animChange);
        break;
      case 4:
        rainbowPattern(leds, board, pixelShift, 62);
        break;
      case 5:
        threeColorPattern(leds, board, pixelShift, 400, HUE_PURPLE, HUE_BLUE, 140);
        break;
    }
    
    animChange = 0;  // We have calculated new pixel values, don't need to next time.
    
    FastLED.show();
    
  }
}
