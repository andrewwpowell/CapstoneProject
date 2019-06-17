#ifndef TIMER_A_H
#define TIMER_A_H

#include <msp430.h>
#include "spi.h"

#define TACCR0_VALUE (3125-1)   // 12 MHz MCLK divided by Fs=3840 Hz (64 sample points * 60 Hz)

// Function Prototypes
void ConfigureTimerA0(void);
void delayMicroseconds(int count);

#endif
