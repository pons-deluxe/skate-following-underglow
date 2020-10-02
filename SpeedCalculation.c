#include "SpeedCalculation.h"
#include <stdlib.h>
#include <string.h>



void setupBuffer (TimestampBuffer * myBuffer, unsigned char numberOfBins){
  myBuffer->timestamps = (unsigned int *)malloc(numberOfBins * sizeof(unsigned int));
  myBuffer->currentDirection = 0;
  myBuffer->currentBin = 0;
  myBuffer->buffSize = numberOfBins;
  myBuffer->pulseCount = 0;
}

void setupBufferCopy (TimestampBufferCopy * myBuffer, unsigned char numberOfBins){
  myBuffer->timestamps = (unsigned int *)malloc(numberOfBins * sizeof(unsigned int));
  myBuffer->currentDirection = 0;
  myBuffer->currentBin = 0;
  myBuffer->buffSize = numberOfBins;
  myBuffer->pulseCount = 0;
}

// Interrupts MUST be deactivated before calling this function
void copyTimestampBuffer(TimestampBuffer * originalBuffer, TimestampBufferCopy * copiedBuffer){
  // typecast originalBuffer->timestamps from (volatile unsigned int *) to (unsigned int *)
  memcpy(copiedBuffer->timestamps, (unsigned int *)originalBuffer->timestamps, originalBuffer->buffSize * sizeof(unsigned int));
  copiedBuffer->currentDirection = originalBuffer->currentDirection;
  copiedBuffer->currentBin = originalBuffer->currentBin;
  copiedBuffer->pulseCount = originalBuffer->pulseCount;
}

float distanceTravelled( TimestampBufferCopy bufferCopy, unsigned int ledUpdateDelay, 
                                  float wheelDiameterMM, unsigned char pulsesPerRotation){
  // A static variable used to check the number of pulses since last time we calculated distance
  static unsigned int earlierPulseCount = 0;
  unsigned int deltaPulse = bufferCopy.pulseCount - earlierPulseCount;
  // Save the current pulseCount to be used the next time this function is called.
  earlierPulseCount = bufferCopy.pulseCount;
  float distancePerPulse = 3.1416 * wheelDiameterMM / pulsesPerRotation;

  // Lots of pulses available, use frequency to calculate distance
  if (deltaPulse > bufferCopy.buffSize){
    return 0;
  }
  // A few pulses available, use period between pulses to calculate distance
  else if(deltaPulse > 1){
    return 0;
  }
  // Only one pulse available, distance is the smallest resolution we have
  else if(deltaPulse > 0){
    return distancePerPulse;
  }
  else{ // (deltaPulse == 0)
    return 0;
  }

  
}
