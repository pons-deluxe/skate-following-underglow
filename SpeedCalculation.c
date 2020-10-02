#include "SpeedCalculation.h"
#include <stdlib.h>

void setupBuffer (TimestampBuffer myBuffer, unsigned char numberOfBins){
  myBuffer.timestamps = (unsigned int *)malloc(numberOfBins * sizeof(unsigned int));
  myBuffer.currentDirection = 0;
  myBuffer.currentBin = 0;
  myBuffer.buffSize = numberOfBins;
}
