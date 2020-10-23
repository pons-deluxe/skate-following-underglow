#ifndef _LEDPATTERN_H
#define _LEDPATTERN_H

#include <stdint.h>
#include <FastLED.h>


void rainbowPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, int32_t patternLengthLeds, 
                    uint16_t boardTipLedNum, bool doubleLedOnTip);

void threeColorPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, int32_t patternLengthLeds, 
                    uint16_t boardTipLedNum, bool doubleLedOnTip, 
                    uint8_t colorHue1, uint8_t colorHue2, uint8_t colorHue3);


void multiFillPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, 
                    int16_t ledsSolidHue, int16_t ledsTransition, 
                    uint8_t hues[], uint8_t hueNb);

void testFrontBackPattern(CRGB leds[], uint16_t totalLeds, int32_t leadingPos, uint32_t patternLengthLeds, 
                    uint16_t boardTipLedNum, bool doubleLedOnTip);


#endif  // _LEDPATTERN_H
