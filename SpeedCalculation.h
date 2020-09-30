#ifndef _SPPEDCALCULATION_H
#define _SPPEDCALCULATION_H

typedef struct TimestampBufferTag {
    volatile unsigned int * timestamps;
    volatile unsigned char currentBin;
    volatile char currentDirection;  // 0=Forwards, 1=Backwards (TODO verify this is the case)
    unsigned char buffSize;
} TimestampBuffer;

void setupBuffer (TimestampBuffer myBuffer, unsigned char numberOfBins);

#endif 
