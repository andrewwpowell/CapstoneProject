#include <msp430.h> 
#include "spi.h"
#include "UART.h"
#include "timerA0.h"
#include "debug.h"
#include "ble.h"
#include "serial_flash.h"
#include "gps.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

#define READ_DATA_LENGTH 100
#define DUMMY_BYTE 8

#define BLE_SLAVE_NOT_READY 0xFE
#define BLE_READ_OVERFLOW   0xFF
#define COMMAND             0x10
#define RESPONSE            0x20
#define ALERT               0x40
#define ERROR               0x80
#define CMD_INITIALIZE      0xBEEF
#define CMD_WRAPPER         0x0A00
#define MAX_PACKET_SIZE     40

int i;
char lat_buffer[MAX_PACKET_SIZE];
char lon_buffer[MAX_PACKET_SIZE];
int writeAddress = 0;
int readAddress = 0;
char sdep_packet_buffer[SDEP_MAX_PACKETSIZE];

void ConfigureClockModule();
void check_gps();

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	ConfigureClockModule();
	ConfigureTimerA0();

	InitializeSPI();
	InitializeUART(9600);
	InitializeBLE();
	InitializeGPS();
	InitializeSerialFlash();
	
	//Write enable serial flash, no block protection
	WriteFlashMemoryStatusRegister(0x02,3);

	SET_DEBUG_LOW;
    SET_DEBUG_TO_AN_OUTPUT;

    _enable_interrupts();

    while(1) {

        //parse gps data, store in buffer
        gps_parse();
        check_gps();

        //send data to serial flash using AAI
        AAIProgramFlashMemory(writeAddress, lat_buffer, MAX_PACKET_SIZE, 3);
        writeAddress+=MAX_PACKET_SIZE;
        AAIProgramFlashMemory(flashAddress, lon_buffer, MAX_PACKET_SIZE, 3);
        writeAddress+=MAX_PACKET_SIZE;

        //if address reached -> read from serial flash
        if(writeAddress >= 0xFFFF) {
            ReadFlashMemory(readAddress, sdep_packet_buffer, MAX_PACKET_SIZE, 3, 1);
            readAddress+=MAX_PACKET_SIZE;

            //send to bluetooth
            sendPacket(COMMAND, sdep_packet_buffer, MAX_PACKET_SIZE, 0);
        }
    }

	return 0;
}

void ConfigureClockModule(void)
{
    // Configure Digitally Controlled Oscillator (DCO) using factory calibrations.
    DCOCTL  = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;
}

void check_gps() {

    lat = gps_current_lat();

    sprintf( lat_buffer, "Latitude\r\n\tDegrees: %d\r\n\tMinutes: %d\r\n\tDirection %d\r\n",
             lat->degrees, lat->minutes, lat->azmuth );

    lon = gps_current_lon();

    sprintf( lon_buffer, "Longitude\r\n\tDegrees: %d\r\n\tMinutes: %d\r\n\tDirection %d\r\n",
             lon->degrees, lon->minutes, lon->azmuth );


}
