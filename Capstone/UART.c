#include "UART.h"

void InitializeUART(int baud_rate) {

    //8 bit data non parity
    //MSB first

    /*
     * Set UCSWRST
     * Initialize all USCI registers with UCSWRST=1
     * Configure Ports
     * Clear UCSWRST
     * Enable Interrupts
     */

    UCA0CTL1 |= UCSWRST;

//    UCA0CTL0 &= ~(BIT2 + BIT1); //UART with automatic baud rate detection
//    UCA0CTL0 &= ~UCPEN;
//    UCA0CTL0 |= UCMSB;
//    UCA0CTL0 &= ~UC7BIT;
//    UCA0CTL0 &= ~UCSPB;
//    UCA0CTL0 &= ~UCSYNC;
    UCA0CTL0 = UCMSB;

    UCA0CTL1 |= UCSSEL_2;   //SMCLK as source


    /*
     * Automatic Baud Rate:
     *  - Data frame is preceded by a synchronization sequence that consists of
     *  a break and a synch field
     *  - Break: 11 or more continuous zeros received
     *  - Synch field follows the break
     *  - Character format should be 8 data bits, LSB first, no parity and one stop bit
     *  - Baud rate measurement is transferred into the baud rate control registers
     *  UCA0BR0, UCA0BR1, and UCA0MCTL
     *  - If length of the synch field exceeds the measurable time the synch timeout error
     *  flag UCSTOE is set
     *  - If break exceeds 22 bit times the break timeout error flag UCBTOE is set
     *  - UCDORM bit is used to control data reception in this mode.
     */

    /*
     * Manual Baud Rate:
     *
     * N = (frequency of clock)/(Baud Rate)
     * where N is the required division factor
     *
     * frequency of clock is 8 MHz
     * Baud Rate is likely 9600
     * N should then be 833
     *
     * UCABR0 + UCABR1 * 256 = N
     *
     */

    const unsigned char lowByte = 833 & 0xFF;
    const unsigned char highByte = 833 >> 8;

    UCA0BR0 = lowByte; //Baud Rate of 9600
    UCA0BR1 = highByte;
    UCA0MCTL |= BIT2;

    //Configure ports (1.1(RXD) and 1.2(TXD))
    P1SEL = BIT1 + BIT2;
    P1SEL2 = BIT1 + BIT2;

    //Clear UCSWRST
    UCA0CTL1 &= ~UCSWRST;

    //enable interrupts
    UC0IE |= UCA0RXIE;

    SET_LED1_AS_AN_OUTPUT;
    TURN_OFF_LED1;

}

void transmit(unsigned char *data) {

//    while(!(IFG2 & UCA0TXIFG)); //wait until buffer is ready to accept another character
//    UCA0TXBUF = data;

    unsigned int i = 0;
    while(data[i]) {
        while(!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = data[i];
        i++;
    }

}

//unsigned char receive() {
//
//    unsigned char receiveByte;
//    while(!(IFG2 & UCA0RXIFG)); //wait until character loaded into buffer
//    receiveByte = UCA0RXBUF;
//
//    return receiveByte;
//}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {

    TOGGLE_LED1;

    data = UCA0RXBUF;
    gps_put(data);
    return;
}
