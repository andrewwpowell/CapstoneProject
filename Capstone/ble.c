#include "ble.h"



void InitializeBLE() {

    //Configure GPIO Chip Enable port pin
    SPI_CS_AS_OUTPUT;
    //Configure IRQ pin
    IRQ_AS_INPUT;

}

void BLESPI(unsigned char toggle) {

    if(toggle) {
        //CS pin low
        ENABLE_BLE;
        delayMicroseconds(100);
    }
    else {
        //CS pin high
        DISABLE_BLE;
    }
}

unsigned char BLEReadData(sdep* response) {

    int i;
    struct sdep *data;
    unsigned char payloadLength = 0;
    unsigned char moreData = 0;
    unsigned char[MAXPACKETSIZE] buffer;

    //wait for IRQ pin to be high to signify data ready to be received

    data = &response;

    //wait until byte received isn't ignored/overread

    data->type = SendAndReceive(DUMMYBYTE);
    buffer[0] = data->type;

    //check to make sure type is correct

    data->id1 = SendAndReceive(DUMMYBYTE);
    buffer[2] = data->id1;
    data->id0 = SendAndReceive(DUMMYBYTE);
    buffer[1] = data->id0;

    //check for error message response

    //check for invalid command

    data->length = SendAndReceive(DUMMYBYTE);
    buffer[4] = data->length;

    //check for invalid length

    payloadLength = data->length & 0x1F; //bits 4-0
    moreData = data->length & BIT7;

    if(payloadLength) {
        for(i = payloadLength-1; i >= 0; i--) {
            data->payload[i] = SendAndReceive(DUMMYBYTE);
            buffer[payloadLength-1-i] = data->payload[i];
        }
    }

    //how to handle moreData != 0

    return data;

}

void BLEWriteData(struct sdep data) {

    int i;
    unsigned char payloadLength;
    unsigned char moreData;

    SendAndReceive(data.type);

    //little endian -> 1 then 0
    SendAndReceive(data.id1);
    SendAndReceive(data.id0);

    SendAndReceive(data.length);

    payloadLength = data.length & 0x1F;
    moreData = data.length & BIT7;

    if(payloadLength) {

        //little endian
        for(i = payloadLength-1; i >= 0; i--) {
            SendAndReceive(data.payload[i]);
        }
    }

    //how to handle moreData != 0?

}
