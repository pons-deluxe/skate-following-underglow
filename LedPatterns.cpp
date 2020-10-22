#include "LedPatterns.h"


void rainbowPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, 
                    uint32_t patternLengthLeds, uint16_t boardTipLedNum, bool doubleLedOnTip){
  static int32_t leadingPos = 0;
  leadingPos += pixelShift;

  // Keep leadingPos in range 0-patternLengthLeds
  if (leadingPos < 0)
    leadingPos += patternLengthLeds;
  // When switching from one animation to another, leadingPos could be much bigger 
  // than the new patternLengthLeds. Modulo will bring back in range
  if (leadingPos >= patternLengthLeds)
    leadingPos %= patternLengthLeds;
  
  uint16_t tipBoardLeds[2];
  tipBoardLeds[0] = boardTipLedNum;
  tipBoardLeds[1] = (doubleLedOnTip) ? boardTipLedNum + 1 : boardTipLedNum;
  uint16_t ourSegmentLength;  // One half of our led strip, accounting for symmetry/asymmetry.
  if(doubleLedOnTip && (totalLeds % 2 == 0)){
    ourSegmentLength = totalLeds / 2;
  }
  else{
    ourSegmentLength = totalLeds / 2 + 1;
  }
  
  for(uint16_t i = 0; i < ourSegmentLength; i++){
    int32_t leftIndex = tipBoardLeds[0] - i;
    if(leftIndex < 0)
      leftIndex += totalLeds;
    int32_t rightIndex = tipBoardLeds[1] + i;
    if(rightIndex >= totalLeds)
      rightIndex -= totalLeds;
    
    uint8_t newHue = map((leadingPos + i) % patternLengthLeds, 0, patternLengthLeds, 0, 255);
    leds[leftIndex].setHue(newHue);
    leds[rightIndex].setHue(newHue);
  }

}

void cyberPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, 
                    uint32_t patternLengthLeds, uint16_t boardTipLedNum, bool doubleLedOnTip){
  static int32_t leadingPos = 0;
  leadingPos += pixelShift;

  // Keep leadingPos in range 0-patternLengthLeds
  if (leadingPos < 0)
    leadingPos += patternLengthLeds;
  // When switching from one animation to another, leadingPos could be much bigger 
  // than the new patternLengthLeds. Modulo will bring back in range
  if (leadingPos >= patternLengthLeds)
    leadingPos %= patternLengthLeds;
  
  uint16_t tipBoardLeds[2];
  tipBoardLeds[0] = boardTipLedNum;
  tipBoardLeds[1] = (doubleLedOnTip) ? boardTipLedNum + 1 : boardTipLedNum;
  uint16_t ourSegmentLength;  // One half of our led strip, accounting for symmetry/asymmetry.
  if(doubleLedOnTip && (totalLeds % 2 == 0)){
    ourSegmentLength = totalLeds / 2;
  }
  else{
    ourSegmentLength = totalLeds / 2 + 1;
  }

  // Since this pattern is alternating between 3 colors, we need to know the one third and 2 thirds of the pattern
  uint32_t oneThirdPattern = patternLengthLeds / 3;
  uint32_t twoThirdsPattern = patternLengthLeds * 2 / 3;

  
  for(uint16_t i = 0; i < ourSegmentLength; i++){
    int32_t leftIndex = tipBoardLeds[0] - i;
    if(leftIndex < 0)
      leftIndex += totalLeds;
    int32_t rightIndex = tipBoardLeds[1] + i;
    if(rightIndex >= totalLeds)
      rightIndex -= totalLeds;
    
    // Copy (leadingPos + i) with modulo so we don't get values above patternLengthLeds
    int32_t safeLeadingPos = (leadingPos + i) % patternLengthLeds;
    
    // With leading position and patternLengthLeds, determine RGB values
    if (safeLeadingPos < oneThirdPattern){
      
      // Left
      leds[leftIndex].red = map(safeLeadingPos, 0, oneThirdPattern, 255, 0);
      leds[leftIndex].green = 0;
      // Right
      leds[rightIndex].red = 0;
      leds[rightIndex].green = map(safeLeadingPos, 0, oneThirdPattern, 0, 255);
    }
    else if (safeLeadingPos < twoThirdsPattern){
      
      // Left
      leds[leftIndex].red = 0;
      leds[leftIndex].green = map(safeLeadingPos, oneThirdPattern, twoThirdsPattern, 0, 255);
      // Right
      leds[rightIndex].red = map(safeLeadingPos, oneThirdPattern, twoThirdsPattern, 0, 255);
      leds[rightIndex].green = map(safeLeadingPos, oneThirdPattern, twoThirdsPattern, 255, 0);
    }
    else{  // between 2/3 and 1 of patternLengthLeds
      
      // Left
      leds[leftIndex].red = map(safeLeadingPos, twoThirdsPattern, patternLengthLeds, 0, 255);
      leds[leftIndex].green = map(safeLeadingPos, twoThirdsPattern, patternLengthLeds, 255, 0);
      // Right
      leds[rightIndex].red = map(safeLeadingPos, twoThirdsPattern, patternLengthLeds, 255, 0);
      leds[rightIndex].green = 0;
    }
    //Blue is always full for this pattern
    leds[leftIndex].blue = 255;
    leds[rightIndex].blue = 255;

    /*
    Serial.print(leds[leftIndex].r, DEC);
    Serial.print("\t");
    Serial.print(leds[leftIndex].g, DEC);
    Serial.print("\t");
    Serial.println(leds[leftIndex].b, DEC);
    */
    
  }

}

void testFrontBackPattern(CRGB leds[], uint16_t totalLeds, int32_t leadingPos, 
                    uint32_t patternLengthLeds, uint16_t boardTipLedNum, bool doubleLedOnTip){
  //in fastLED, could be: leds[i].setHue( 160);
  //                      leds[i].setRGB( 50, 100, 150);
  uint16_t tipBoardLeds[2];
  tipBoardLeds[0] = boardTipLedNum;
  tipBoardLeds[1] = (doubleLedOnTip) ? boardTipLedNum + 1 : boardTipLedNum;
  uint16_t ourSegmentLength;  // One half of our led strip, accounting for symmetry/asymmetry.
  if(doubleLedOnTip && (totalLeds % 2 == 0)){
    ourSegmentLength = totalLeds / 2;
  }
  else{
    ourSegmentLength = totalLeds / 2 + 1;
  }
  
  for(uint16_t i = 0; i < ourSegmentLength; i++){
    int32_t leftIndex = tipBoardLeds[0] - i;
    if(leftIndex < 0)
      leftIndex += totalLeds;
    int32_t rightIndex = tipBoardLeds[1] + i;
    if(rightIndex >= totalLeds)
      rightIndex -= totalLeds;
    
    if(i == 0){
      leds[leftIndex].setRGB(255,255,255);
      leds[rightIndex].setRGB(255,255,255);
    }
    else if(i == (ourSegmentLength-1)){
      leds[leftIndex].setRGB(255,0,0);
      leds[rightIndex].setRGB(255,0,0);
    }
    else{
      leds[leftIndex].setRGB(0,0,0);
      leds[rightIndex].setRGB(0,0,0);
    }
    
  }

}
