#ifndef BLE_H_
#define BLE_H_

#include <msp430.h>
#include "spi.h"
#include "timerA0.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bluetooth Enable
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.5
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define BLE_BIT                 BIT0
#define BLE_PORT                P2OUT
#define BLE_DDR                 P2DIR
#define SPI_CS_AS_OUTPUT        BLE_DDR |= BLE_BIT
#define ENABLE_BLE              BLE_PORT &= ~BLE_BIT
#define DISABLE_BLE             BLE_PORT |= BLE_BIT

#define IRQ_BIT                 BIT1
#define IRQ_PORT                P2IN
#define IRQ_DDR                 P2DIR
#define IRQ_AS_INPUT            IRQ_DDR &= ~IRQ_BIT
#define READ_IRQ                IRQ_PORT & IRQ_BIT

#define MAXPACKETSIZE           16

struct sdep {
    unsigned char type;
    unsigned char id0;
    unsigned char id1;
    unsigned char length;   //bit 7 is more_data, bits 4-0 are payload length
    unsigned char payload[MAXPACKETSIZE];
};

void InitializeBLE();
void BLESPI(unsigned char toggle);
unsigned char* BLEReadData();
void BLEWriteData(unsigned char* data, unsigned char data_length);

unsigned char reset(unsigned char blocking);
unsigned char factoryReset(unsigned char blocking);
unsigned char resetCompleted();
void info();
unsigned char echo(unsigned char enable);
unsigned char isConnected();
void disconnect();
unsigned char setAdvData(unsigned char data[], unsigned char size);
void update(unsigned int period_ms);

#endif /* BLE_H_ */
