#include "timerA0.h"

void ConfigureTimerA0(void)
{
    // Timer0_A3 Control Register
    TA0CTL = (MC_0 | TACLR);     // Stop the timer and clear it.

    TA0CTL |= TASSEL_2 | ID_0 | MC_1;
    /* TASSEL1 = 0x0200 Timer A clock source select 0
     * TASSEL0 = 0x0100 Timer A clock source select 1
     *  00 = TACLK
     *  01 = ACLK
     *  10 = SMCLK
     *  11 = INCLK
     * ID1 = 0x0080 Timer A clock input divider 1
     * ID0 = 0x0040 Timer A clock input divider 0
     *  00 => Input divider = 1
     *  01 => Input divider = 2
     *  10 => Input divider = 4
     *  11 => Input divider = 8
     * MC1 = 0x0020 Timer A mode control 1
     * MC0 = 0x0010 Timer A mode control 0
     *  00 = Stop
     *  01 = Up
     *  10 = Continuous
     *  11 = Up/Down
     * TACLR = 0x0004 Timer A counter clear
     * TAIE = 0x0002 Timer A counter interrupt enable
     * TAIFG = 0x0001 Timer A counter interrupt flag
     */

    TACCR0 = TACCR0_VALUE;

    // TACCR0 interrupt enabled (see Section 12.2.6 of User's Guide)
    TACCTL0 |= CCIE;
}

void delayMicroseconds(int count) {

    while(count) {
        __delay_cycles(4);
        count--;
    }
}

// Timer0 CCR0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{

}
