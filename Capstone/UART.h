#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include "gps_parser.h"

#define LED1_BIT                BIT0
#define LED1_PORT               P1OUT
#define LED1_DDR                P1DIR
#define SET_LED1_AS_AN_OUTPUT   LED1_DDR |= LED1_BIT
#define TURN_ON_LED1            LED1_PORT |= LED1_BIT
#define TURN_OFF_LED1           LED1_PORT &= ~LED1_BIT
#define TOGGLE_LED1             LED1_PORT ^= LED1_BIT

unsigned char data;

void InitializeUART(int baud_rate);
void transmit(unsigned char *data);
//unsigned char receive();

#endif /* UART_H_ */
