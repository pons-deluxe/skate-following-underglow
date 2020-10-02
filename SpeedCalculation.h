#ifndef _SPEEDCALCULATION_H
#define _SPEEDCALCULATION_H

typedef struct TimestampBuffer_S {
    volatile unsigned int * timestamps;
    volatile unsigned char currentBin;
    volatile char currentDirection;  // 0=Forwards, 1=Backwards (TODO verify this is the case)
    unsigned char buffSize;
    volatile unsigned int pulseCount;
} TimestampBuffer;

// A copy of the timestamp buffer that won't be affected by interrupts
typedef struct TimestampBufferCopy_S {
    unsigned int * timestamps;
    unsigned char currentBin;
    char currentDirection;  // 0=Forwards, 1=Backwards (TODO verify this is the case)
    unsigned char buffSize;
    unsigned int pulseCount;
} TimestampBufferCopy;

#ifdef __cplusplus
extern "C" {
#endif

void setupBuffer (TimestampBuffer * myBuffer, unsigned char numberOfBins);

void setupBufferCopy (TimestampBufferCopy * myBuffer, unsigned char numberOfBins);

void copyTimestampBuffer(TimestampBuffer * originalBuffer, TimestampBufferCopy * copiedBuffer);

float distanceTravelled(TimestampBufferCopy bufferCopy, unsigned int ledUpdateDelay, 
                        float wheelDiameterMM, unsigned char pulsesPerRotation);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // _SPEEDCALCULATION_H
