#ifndef _LEDPATTERN_H
#define _LEDPATTERN_H

#include <FastLED.h>


void mirrorPattern(CRGB leds, uint16_t numLed, unsigned short leadingPos, 
                    float patternLength);


#endif  // _LEDPATTERN_H
