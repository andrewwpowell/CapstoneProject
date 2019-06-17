#include "serial_flash.h"

void InitializeSerialFlash() {

    //Initialize port pins
    SET_ENABLE_U3_AS_AN_OUTPUT;
    SET_ENABLE_U2_AS_AN_OUTPUT;
    SET_HOLD_U3_AS_AN_OUTPUT;
    SET_HOLD_U2_AS_AN_OUTPUT;
    SET_WRITE_PROTECT_AS_AN_OUTPUT;

    //If not sure assign inactive value
}

unsigned int ReadFlashMemoryID(unsigned char ComponentNumber){

    unsigned char ID[2];
    unsigned int returnValue;

    ComponentEnable(ComponentNumber);

    //Send Read-ID command.
    SendAndReceive(READ_ID);

    //Send address
    SendAndReceive(0x00);
    SendAndReceive(0x00);
    SendAndReceive(0x00);

    //Read Device ID
    ID[0] = SendAndReceive(DUMMYBYTE);
    ID[1] = SendAndReceive(DUMMYBYTE);

    ComponentDisable(ComponentNumber);

    //Place manufacturer ID in the upper byte of the return value, and place the device ID in the lower byte
    returnValue = ((unsigned int) ID[0] << 8) + (unsigned int) ID[1];

	return returnValue;
}

unsigned char ReadFlashMemoryStatusRegister(unsigned char ComponentNumber) {

    unsigned char returnValue;

    ComponentEnable(ComponentNumber);

    //Send Read-Status-Register command
    SendAndReceive(RDSR);

    //MISO then gives the value located in status register
    returnValue = SendAndReceive(DUMMYBYTE);

    ComponentDisable(ComponentNumber);

	return returnValue;
}

void WriteFlashMemoryStatusRegister(unsigned char WriteValue,unsigned char ComponentNumber) {

    WriteEnable(ComponentNumber);

    ComponentEnable(ComponentNumber);

    //Enable-Write-Status-Register
    SendAndReceive(EWSR);

    //Make CE high
    ComponentDisable(ComponentNumber);

    //Make CE low again
    ComponentEnable(ComponentNumber);

    //Send Write-Status-Register command
    SendAndReceive(WRSR);

    //Send value to be written
    SendAndReceive(WriteValue);

    ComponentDisable(ComponentNumber);

    WriteDisable(ComponentNumber);

}

void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
unsigned int NumberOfDataValues, unsigned char ComponentNumber, unsigned char ReadMode) {

    //StartAddress -> Address from which read starts
    //DataValuesArray -> Array holding bytes received
    //NumberOfDataValues -> Number of bytes to be received
    //ReadMode -> READ/HIGH_SPEED_READ

    ComponentEnable(ComponentNumber);

    //Send Read command
    SendAndReceive(ReadMode);

    if(ReadMode == READ) {

        //Split up StartAddress into 3 bytes to be sent in (lower 24 bits) 0x0___
        unsigned char* address = (unsigned char*)&StartAddress;

        SendAndReceive(address[2]);
        SendAndReceive(address[1]);
        SendAndReceive(address[0]);

        //iterate through number of data values to be read in
        int i;
        for(i = 0; i < NumberOfDataValues; i++) {

            //Store values read
            DataValuesArray[i] = SendAndReceive(DUMMYBYTE);

        }
    }
    else {

        //Split up StartAddress into 3 bytes to be sent in (lower 24 bits) 0x0___
        unsigned char* address = (unsigned char*)&StartAddress;

        SendAndReceive(address[2]);
        SendAndReceive(address[1]);
        SendAndReceive(address[0]);
        //Dummy byte
        SendAndReceive(0x00);

        //iterate through number of data values to be read in
        int i;
        for(i = 0; i < NumberOfDataValues; i++) {

            //Store values read
            DataValuesArray[i] = SendAndReceive(DUMMYBYTE);

        }
    }

    ComponentDisable(ComponentNumber);

}

void ByteProgramFlashMemory(unsigned long MemoryAddress, unsigned char WriteValue, unsigned char ComponentNumber) {

    WriteEnable(ComponentNumber);

    ComponentEnable(ComponentNumber);

    //Send in Byte program command
    SendAndReceive(BYTE_PROGRAM);

    //Split up MemoryAddress into 3 bytes to be sent in
    unsigned char* address = (unsigned char*)&MemoryAddress;

    SendAndReceive(address[2]);
    SendAndReceive(address[1]);
    SendAndReceive(address[0]);

    //Send in WriteValue
    SendAndReceive(WriteValue);

    ComponentDisable(ComponentNumber);

    unsigned char pollBusy = FlashMemoryBusy(ComponentNumber);
    while(pollBusy != 0)
        pollBusy = FlashMemoryBusy(ComponentNumber);

}

void AAIProgramFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
unsigned int NumberOfDataValues, unsigned char ComponentNumber) {

    WriteEnable(ComponentNumber);

    ComponentEnable(ComponentNumber);

    //Send in AAI command
    SendAndReceive(AAI_PROGRAM);

    //Split up StartAddress into 3 bytes to be sent in (lower 24 bits) 0x0___
    unsigned char* address = (unsigned char*)&StartAddress;

    SendAndReceive(address[2]);
    SendAndReceive(address[1]);
    SendAndReceive(address[0]);
    //Send in first data byte
    SendAndReceive(DataValuesArray[0]);

    unsigned char pollBusy = 0;

    int i;
    for(i = 1; i < NumberOfDataValues; i++) {

        ComponentDisable(ComponentNumber);

        //Poll Busy bit
        pollBusy = FlashMemoryBusy(ComponentNumber);

        while(pollBusy != 0)
            pollBusy = FlashMemoryBusy(ComponentNumber);

        ComponentEnable(ComponentNumber);

        //Send AAI command
        SendAndReceive(AAI_PROGRAM);

        //Send next value in array
        SendAndReceive(DataValuesArray[i]);
    }

    ComponentDisable(ComponentNumber);

    //Poll busy bit once more
    pollBusy = FlashMemoryBusy(ComponentNumber);
    while(pollBusy != 0)
        pollBusy = FlashMemoryBusy(ComponentNumber);

    WriteDisable(ComponentNumber);

    //Read Status Register Instruction to verify end of AAI Operation
    unsigned char status = ReadFlashMemoryStatusRegister(ComponentNumber);

    //Poll status register - while AAI program bit is high: continue looping until no longer high
    while(status & SR_AAI != 0)
        status = ReadFlashMemoryStatusRegister(ComponentNumber);

}

void ChipEraseFlashMemory(unsigned char ComponentNumber) {

    WriteEnable(ComponentNumber);

    ComponentEnable(ComponentNumber);

    //Send in Chip Erase command
    SendAndReceive(CHIP_ERASE);

    ComponentDisable(ComponentNumber);

    unsigned char isBusy = FlashMemoryBusy(ComponentNumber);
    while(isBusy)
        isBusy = FlashMemoryBusy(ComponentNumber);

}

void SectorBlockEraseFlashMemory(unsigned long StartAddress, unsigned char ComponentNumber, unsigned char EraseMode) {

    WriteEnable(ComponentNumber);

    ComponentEnable(ComponentNumber);

    SendAndReceive(EraseMode);

    //Split up StartAddress into 3 bytes to be sent in (lower 24 bits) 0x0___
    unsigned char* address = (unsigned char*)&StartAddress;

    SendAndReceive(address[2]);
    SendAndReceive(address[1]);
    SendAndReceive(address[0]);

    ComponentDisable(ComponentNumber);

    unsigned char isBusy = FlashMemoryBusy(ComponentNumber);
    while(isBusy)
        isBusy = FlashMemoryBusy(ComponentNumber);

}

void SetBlockProtection(unsigned char ProtectionLevel, unsigned char ComponentNumber) {

    ComponentEnable(ComponentNumber);

    unsigned char currentStatus = ReadFlashMemoryStatusRegister(ComponentNumber);

    if(ProtectionLevel == 0) {
        //BP0 = 0, BP1 = 0
        currentStatus &= ~SR_BP0;
        currentStatus &= ~SR_BP1;
        WriteFlashMemoryStatusRegister(currentStatus, ComponentNumber);
    }
    else if(ProtectionLevel == 1) {
        //BP0 = 1, BP1 = 0
        currentStatus &= ~SR_BP1;
        currentStatus |= SR_BP0;
        WriteFlashMemoryStatusRegister(currentStatus, ComponentNumber);
    }
    else if(ProtectionLevel == 2) {
        //BP0 = 0, BP1 = 1
        currentStatus &= ~SR_BP0;
        currentStatus |= SR_BP1;
        WriteFlashMemoryStatusRegister(currentStatus, ComponentNumber);
    }
    else {
        //BP0 = 1, BP1 = 1
        currentStatus |= SR_BP1;
        currentStatus |= SR_BP0;
        WriteFlashMemoryStatusRegister(currentStatus, ComponentNumber);
    }

    ComponentDisable(ComponentNumber);
}

unsigned char FlashMemoryBusy(unsigned char ComponentNumber) {

    //Determine if flash memory component is busy (whether or not the flash memory has completed the current command)
    //Read flash memory status register and Busy bit mask

    unsigned char busy = 0;

    busy = ReadFlashMemoryStatusRegister(ComponentNumber) & SR_BUSY;

	return busy;
}

void WriteEnable(unsigned char ComponentNumber) {

    ComponentEnable(ComponentNumber);

    SendAndReceive(WREN);

    ComponentDisable(ComponentNumber);

}

void WriteDisable(unsigned char ComponentNumber) {

    ComponentEnable(ComponentNumber);

    SendAndReceive(WRDI);

    ComponentDisable(ComponentNumber);

}

void ComponentEnable(unsigned char ComponentNumber) {

    if(ComponentNumber == 3) {
        //Enable U3
        ENABLE_FLASH_MEMORY_U3;
    }
    else {
        //Enable U2
        ENABLE_FLASH_MEMORY_U2;
    }

}

void ComponentDisable(unsigned char ComponentNumber) {

    if(ComponentNumber == 3) {
        //Disable U3
        DISABLE_FLASH_MEMORY_U3;
    }
    else {
        //Disable U2
        DISABLE_FLASH_MEMORY_U2;
    }

}
