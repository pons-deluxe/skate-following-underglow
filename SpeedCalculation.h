#ifndef _SPPEDCALCULATION_H
#define _SPPEDCALCULATION_H

typedef struct TimestampBufferTag {
    volatile unsigned int * timestamps;
    volatile unsigned char currentBin;
    volatile char currentDirection;  // 0=Forwards, 1=Backwards
    unsigned char buffSize;
} TimestampBuffer;

void setupBuffer (TimestampBuffer myBuffer, unsigned char numberOfBins);

#endif 
