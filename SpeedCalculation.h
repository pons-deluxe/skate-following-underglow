#ifndef _SPEEDCALCULATION_H
#define _SPEEDCALCULATION_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct TimestampBuffer_S {
    volatile uint32_t * timestamps;
    volatile char currentBin;  // The last bin we wrote a timestamp for. TODO char should be int8_t
    volatile char currentDirection;  // 0=Forwards, 1=Backwards (TODO verify this is the case)
    uint8_t buffSize;
    volatile uint32_t pulseCount;
} TimestampBuffer;

// A copy of the timestamp buffer that won't be affected by interrupts
typedef struct TimestampBufferCopy_S {
    uint32_t * timestamps;
    char currentBin;
    char currentDirection;  // 0=Forwards, 1=Backwards (TODO verify this is the case)
    uint8_t buffSize;
    uint32_t pulseCount;
} TimestampBufferCopy;

#ifdef __cplusplus
extern "C" {
#endif

void setupBuffer (TimestampBuffer * myBuffer, uint8_t numberOfBins);

void setupBufferCopy (TimestampBufferCopy * myBuffer, uint8_t numberOfBins);

void copyTimestampBuffer(TimestampBuffer * originalBuffer, TimestampBufferCopy * copiedBuffer);

int32_t distanceToPixCount(int32_t distance, int32_t ledSpacing);

float distanceTravelled(TimestampBufferCopy bufferCopy, uint32_t ledUpdateDelay, 
                        float distancePerPulse);
                        
int32_t distanceTravelledUnits(TimestampBufferCopy bufferCopy, uint32_t ledUpdateDelay, 
                        int32_t distancePerPulseUnits);



#ifdef __cplusplus
} // extern "C"
#endif

#endif  // _SPEEDCALCULATION_H
