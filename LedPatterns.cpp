#include "LedPatterns.h"


Board setupBoard (uint16_t totalLeds, uint16_t boardTipLedNum, bool doubleLedOnTip){
  Board myBoard;
  myBoard.totalLeds = totalLeds;
  myBoard.doubleLedOnTip = doubleLedOnTip;
  myBoard.tipLed0 = boardTipLedNum;
  myBoard.tipLed1 = (doubleLedOnTip) ? boardTipLedNum + 1 : boardTipLedNum;
  
  if(doubleLedOnTip && (totalLeds % 2 == 0)){
    myBoard.halfBoard = totalLeds / 2;
  }
  else{
    myBoard.halfBoard = totalLeds / 2 + 1;
  }
  
  return myBoard;
}

void rainbowPattern(CRGB leds[], Board board, int32_t pixelShift, int32_t patternLengthLeds){
  static int32_t leadingPos = 0;
  leadingPos += pixelShift;

  // Keep leadingPos in range 0-patternLengthLeds
  if (leadingPos < 0)
    leadingPos += patternLengthLeds;
  // When switching from one animation to another, leadingPos could be much bigger 
  // than the new patternLengthLeds. Modulo will bring back in range
  if (leadingPos >= patternLengthLeds)
    leadingPos %= patternLengthLeds;
  
  for(uint16_t i = 0; i < board.halfBoard; i++){
    int32_t leftIndex = board.tipLed0 - i;
    if(leftIndex < 0)
      leftIndex += board.totalLeds;
    int32_t rightIndex = board.tipLed1 + i;
    if(rightIndex >= board.totalLeds)
      rightIndex -= board.totalLeds;
    
    uint8_t newHue = map((leadingPos + i) % patternLengthLeds, 0, patternLengthLeds, 0, 255);
    leds[leftIndex].setHue(newHue);
    leds[rightIndex].setHue(newHue);
  }

}

void threeColorPattern(CRGB leds[], Board board, int32_t pixelShift, int32_t patternLengthLeds, 
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

  // Since this pattern is alternating between 3 colors, we need to know the one third and 2 thirds of the pattern
  int16_t oneThirdPattern = patternLengthLeds / 3;
  int16_t twoThirdsPattern = patternLengthLeds * 2 / 3;
  int16_t transitionLength = patternLengthLeds / 12;

  for(uint16_t i = 0; i < board.halfBoard; i++){
    int32_t leftIndex = board.tipLed0 - i;
    if(leftIndex < 0)
      leftIndex += board.totalLeds;
    int32_t rightIndex = board.tipLed1 + i;
    if(rightIndex >= board.totalLeds)
      rightIndex -= board.totalLeds;
    
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
    
  }

}

void multiFillPattern(CRGB leds[], Board board, int32_t pixelShift, 
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
  for (uint16_t i = 0; i < board.totalLeds; i++){
    leds[i].setHue(fillHue);
  }

}

void testFrontBackPattern(CRGB leds[], Board board){
   
  for(uint16_t i = 0; i < board.halfBoard; i++){
    int32_t leftIndex = board.tipLed0 - i;
    if(leftIndex < 0)
      leftIndex += board.totalLeds;
    int32_t rightIndex = board.tipLed1 + i;
    if(rightIndex >= board.totalLeds)
      rightIndex -= board.totalLeds;
    
    if(i == 0){
      leds[leftIndex].setRGB(255,255,255);
      leds[rightIndex].setRGB(255,255,255);
    }
    else if(i == (board.halfBoard - 1)){
      leds[leftIndex].setRGB(255,0,0);
      leds[rightIndex].setRGB(255,0,0);
    }
    else{
      leds[leftIndex].setRGB(0,0,0);
      leds[rightIndex].setRGB(0,0,0);
    }
    
  }

}

void theaterLights(CRGB leds[], uint16_t totalLeds, int32_t leadingPos, uint32_t patternLengthLeds, 
                    uint16_t boardTipLedNum, bool doubleLedOnTip){
  
}
