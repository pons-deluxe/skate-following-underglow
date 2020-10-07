#ifndef _LEDPATTERN_H
#define _LEDPATTERN_H

#include <stdint.h>
#include <FastLED.h>


void rainbowPattern(CRGB leds[], uint16_t totalLeds, uint32_t leadingPos, uint32_t patternLengthLeds, 
                    uint16_t boardTipLedNum, bool doubleLedOnTip);


#endif  // _LEDPATTERN_H
