#include "SpeedCalculation.h"


void setupBuffer (TimestampBuffer * myBuffer, uint8_t numberOfBins){
  myBuffer->timestamps = (uint32_t *)malloc(numberOfBins * sizeof(uint32_t));
  myBuffer->currentDirection = 0;
  myBuffer->currentBin = 0;
  myBuffer->buffSize = numberOfBins;
  myBuffer->pulseCount = 0;
}

void setupBufferCopy (TimestampBufferCopy * myBuffer, uint8_t numberOfBins){
  myBuffer->timestamps = (uint32_t *)malloc(numberOfBins * sizeof(uint32_t));
  myBuffer->currentDirection = 0;
  myBuffer->currentBin = 0;
  myBuffer->buffSize = numberOfBins;
  myBuffer->pulseCount = 0;
}

// Interrupts MUST be deactivated before calling this function
void copyTimestampBuffer(TimestampBuffer * originalBuffer, TimestampBufferCopy * copiedBuffer){
  // typecast originalBuffer->timestamps from (volatile uint32_t *) to (uint32_t *)
  memcpy(copiedBuffer->timestamps, (uint32_t *)originalBuffer->timestamps, originalBuffer->buffSize * sizeof(uint32_t));
  copiedBuffer->currentDirection = originalBuffer->currentDirection;
  copiedBuffer->currentBin = originalBuffer->currentBin;
  copiedBuffer->pulseCount = originalBuffer->pulseCount;
}

int32_t distanceToPixCount(int32_t distance, int32_t ledSpacing){
  static int32_t remainder = 0;  // Keeps note of the actual distance we should have if ledSpacing was 0.
  int32_t numPixels = 0;
  int32_t actualDistance = distance + remainder;
  
  while(actualDistance >= ledSpacing){
    actualDistance -= ledSpacing;
    numPixels++;
  }
  
  while(actualDistance <= -ledSpacing){
    actualDistance += ledSpacing;
    numPixels--;
  }
  remainder = actualDistance;
  return numPixels;
}

float distanceTravelled( TimestampBufferCopy bufferCopy, uint32_t ledUpdateDelay, 
                                  float distancePerPulse){
  // A static variable used to check the number of pulses since last time we calculated distance
  static uint32_t earlierPulseCount = 0;
  uint32_t deltaPulse = bufferCopy.pulseCount - earlierPulseCount;
  // Save the current pulseCount to be used the next time this function is called.
  earlierPulseCount = bufferCopy.pulseCount;

  float curDirection = (bufferCopy.currentDirection == 0) ? 1.0 : -1.0;

  // Lots of pulses available, use frequency to calculate distance
  if (deltaPulse > bufferCopy.buffSize){
    return curDirection * distancePerPulse * deltaPulse;
  }
  // A few pulses available, use period between pulses to calculate distance
  else if(deltaPulse > 1){
    // man idk
    /*
    float period = 0;
    char bin0, bin1;
    for(int i = 0; i < deltaPulse; i++){
      bin1 = bufferCopy.currentBin - i;
      if(bin1 < 0){
        bin1 += bufferCopy.buffSize;
      }
      bin0 = bufferCopy.currentBin - i - 1;
      if(bin0 < 0){
        bin0 += bufferCopy.buffSize;
      }
      // Sum all the the periods between pulses
      period += bufferCopy.timestamps[bin1] - bufferCopy.timestamps[bin0];
    }
    // Get the average period for our pulses (speed = distance / time -> distance = speed * time
    */
    return curDirection * distancePerPulse * deltaPulse;
  }
  // Only one pulse available, distance is the smallest resolution we have
  else if(deltaPulse > 0){
    return curDirection * distancePerPulse;
  }
  else{ // (deltaPulse == 0)
    return 0;
  }
}

int32_t distanceTravelledUnits(TimestampBufferCopy bufferCopy, uint32_t ledUpdateDelay, 
                        int32_t distancePerPulseUnits){
  // A static variable used to check the number of pulses since last time we calculated distance
  static uint32_t earlierPulseCount = 0;
  uint32_t deltaPulse = bufferCopy.pulseCount - earlierPulseCount;
  // Save the current pulseCount to be used the next time this function is called.
  earlierPulseCount = bufferCopy.pulseCount;

  int32_t curDirection = (bufferCopy.currentDirection == 0) ? 1 : -1;

  return curDirection * distancePerPulseUnits * deltaPulse;
}
