#include "spi.h"

void InitializeSPI() {

    /*
     * Set UCSWRST
     * Initialize all USCI registers with UCSWRST=1
     * Configure ports
     * Clear UCSWRST via software
     * Enable Interrupts
     */

    UCB0CTL1 |= UCSWRST;

    /*
     * UCCKPH = 1 ->    Data changed on first uclk edge and captured on following edge
     * UCCKPL = 0 ->    Inactive state is low
     * UCMSB = 1 ->     MSB first
     * UC7BIT = 0 ->    8 bit data
     * UCMST = 1 ->     Master mode
     * UCMODE = 00 ->   SPI mode
     * UCSYNC = 1 ->    SPI mode
     */
    UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;
    UCB0CTL1 |= UCSSEL_2;
    //4 MHz clock needed - divide 12 MHz master clock by 3
    UCB0BR1 = 0; UCB0BR0 |= 0x03;
    UCB0TXBUF = 0;

    // Configure port pins - Hardware SPI: no need to configure pins
    SET_SPI_SCK_AS_AN_OUTPUT;
    SET_USCIB0_MOSI_AS_AN_OUTPUT;
    SET_USCIB0_MISO_AS_AN_INPUT;

    // Configure port pins to receive output from USCI B0
    P1SEL2 |= SPI_SCK_BIT; P1SEL |= SPI_SCK_BIT;
    P1SEL2 |= USCIB0_MOSI_BIT; P1SEL |= USCIB0_MOSI_BIT;
    P1SEL2 |= USCIB0_MISO_BIT; P1SEL |= USCIB0_MISO_BIT;

//    WRITE_LOGIC_0_TO_SLAVE;
//    SET_USCIB0_MOSI_AS_AN_OUTPUT;
//    P1SEL2 |= USCIB0_MOSI_BIT; P1SEL |= USCIB0_MOSI_BIT;
//
//    USCIB0_MISO_PORT &= ~USCIB0_MISO_BIT;
//    SET_USCIB0_MISO_AS_AN_INPUT;
//    P1SEL2 |= USCIB0_MISO_BIT; P1SEL |= USCIB0_MISO_BIT;

    //Clear UCSWRST
    UCB0CTL1 &= ~UCSWRST;

    //Enable Interrupts
//    IE2 |= UCB0TXIE;
//    IE2 |= UCB0RXIE;

}


unsigned char SendAndReceive(unsigned char sendByte) {

    unsigned char receiveByte = 0;

    while(!(IFG2 & UCB0TXIFG));  //wait for transmit buffer to be empty
    UCB0TXBUF = sendByte;   //set transmit buffer

    while(!(IFG2 & UCB0RXIFG));
    receiveByte = UCB0RXBUF;

    return receiveByte;

}
