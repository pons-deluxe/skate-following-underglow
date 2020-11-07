#ifndef _LEDPATTERN_H
#define _LEDPATTERN_H

#include <stdint.h>
#include <FastLED.h>


typedef struct Board_S {
    uint16_t tipLed0;  // The first LED on the strip situated on the tip of the board.
    uint16_t tipLed1;  // The second LED on the strip situated on the tip of the board. Same value 
                       // as tipLed0 if only a single LED is on the tip of the board.
    uint16_t halfBoard;  // The last bin we wrote a timestamp for. TODO char should be int8_t
    bool doubleLedOnTip;  // One half of our led strip, accounting for symmetry/asymmetry.
    uint16_t totalLeds;
} Board;

Board setupBoard (uint16_t totalLeds, uint16_t boardTipLedNum, bool doubleLedOnTip);

void rainbowPattern(CRGB leds[], Board board, int32_t pixelShift, int32_t patternLengthLeds);

void threeColorPattern(CRGB leds[], Board board, int32_t pixelShift, int32_t patternLengthLeds, 
                        uint8_t colorHue1, uint8_t colorHue2, uint8_t colorHue3);


void multiFillPattern(CRGB leds[], Board board, int32_t pixelShift, 
                    int16_t ledsSolidHue, int16_t ledsTransition, 
                    uint8_t hues[], uint8_t hueNb);

void testFrontBackPattern(CRGB leds[], Board board);

void theaterLights(CRGB leds[], Board board, bool animChangeFlag);


#endif  // _LEDPATTERN_H
