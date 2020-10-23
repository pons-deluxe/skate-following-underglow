#include "LedPatterns.h"


void rainbowPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, 
                    int32_t patternLengthLeds, uint16_t boardTipLedNum, bool doubleLedOnTip){
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

void threeColorPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, 
                    int32_t patternLengthLeds, uint16_t boardTipLedNum, bool doubleLedOnTip,
                    uint8_t colorHue1, uint8_t colorHue2, uint8_t colorHue3){
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
  int16_t oneThirdPattern = patternLengthLeds / 3;
  int16_t twoThirdsPattern = patternLengthLeds * 2 / 3;
  int16_t transitionLength = patternLengthLeds / 12;

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
    if (safeLeadingPos < transitionLength){
      leds[leftIndex].setHue(map(safeLeadingPos, 0, transitionLength, colorHue3, colorHue1));
      leds[rightIndex].setHue(map(safeLeadingPos, 0, transitionLength, colorHue1, colorHue2));
    }
    else if (safeLeadingPos < oneThirdPattern){
      leds[leftIndex].setHue(colorHue1);
      leds[rightIndex].setHue(colorHue2);
    }
    else if (safeLeadingPos < (oneThirdPattern + transitionLength)){
      leds[leftIndex].setHue(map(safeLeadingPos, oneThirdPattern, oneThirdPattern + transitionLength, colorHue1, colorHue2));
      leds[rightIndex].setHue(map(safeLeadingPos, oneThirdPattern, oneThirdPattern + transitionLength, colorHue2, colorHue3));
    }
    else if (safeLeadingPos < twoThirdsPattern){
      leds[leftIndex].setHue(colorHue2);
      leds[rightIndex].setHue(colorHue3);
    }
    else if (safeLeadingPos < (twoThirdsPattern + transitionLength)){
      leds[leftIndex].setHue(map(safeLeadingPos, twoThirdsPattern, twoThirdsPattern + transitionLength, colorHue2, colorHue3));
      leds[rightIndex].setHue(map(safeLeadingPos, twoThirdsPattern, twoThirdsPattern + transitionLength, colorHue3, colorHue1));
    }
    else{  //Smaller than patternLengthLeds
      leds[leftIndex].setHue(HUE_AQUA);
      leds[rightIndex].setHue(HUE_PURPLE);
    }

    /*
    Serial.print(leds[leftIndex].r, DEC);
    Serial.print("\t");
    Serial.print(leds[leftIndex].g, DEC);
    Serial.print("\t");
    Serial.println(leds[leftIndex].b, DEC);
    */
    
  }

}

void multiFillPattern(CRGB leds[], uint16_t totalLeds, int32_t pixelShift, 
                    int16_t ledsSolidHue, int16_t ledsTransition, 
                    uint8_t hues[], uint8_t hueNb){
  static int32_t leadingPos = 0;
  leadingPos += pixelShift;

  // Keep leadingPos in range 0 to (total steps in pattern)
  int16_t totalSteps = (ledsSolidHue + ledsTransition) * hueNb;
  if (leadingPos < 0)
    leadingPos += totalSteps;
  // When switching from one animation to another, leadingPos could be much bigger 
  // than the new totalSteps. Modulo will bring back in range
  if (leadingPos >= totalSteps)
    leadingPos %= totalSteps;

  uint8_t fillHue = 0;
  
  for (uint8_t i = 0; i < hueNb; i++){
    //Are we in this hue's transition step
    if (leadingPos >= (ledsSolidHue + ledsTransition) * i && leadingPos < (ledsSolidHue + ledsTransition) * i + ledsTransition){
      // Find a hue in between the 2 hues
      
      int16_t hue1 = (i==0) ? hues[hueNb - 1] : hues[i - 1];
      int16_t hue2 = hues[i];

      if (hue1 < 128 && hue2 >= hue1 + 128)
        hue2 -= 256;
      else if (hue2 < 128 && hue1 >= hue2 + 128)
        hue1 -= 256;
        
      int16_t newHue = map(leadingPos, (ledsSolidHue + ledsTransition) * i, (ledsSolidHue + ledsTransition) * i + ledsTransition, hue1, hue2);

      if (newHue < 0)
        newHue += 256;
      
      fillHue = (uint8_t)newHue;
      break;
    }
    // Are we in this hue solid color step
    if (leadingPos >= (ledsSolidHue + ledsTransition) * i + ledsTransition && leadingPos < (ledsSolidHue + ledsTransition) * (i + 1)){
      fillHue = hues[i];
      break;
    }
  }
  // Set the same color on all Leds
  for (uint16_t i = 0; i < totalLeds; i++){
    leds[i].setHue(fillHue);
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
