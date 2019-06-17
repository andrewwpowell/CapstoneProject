#include "FIFO.h"

void InitializeFIFO(unsigned char* t_buffer, unsigned short t_depth, unsigned char t_itemSize, unsigned char t_overwritable) {

    //Initialize
    //buffer, depth, itemSize, overwritable, count, writeIndex, readIndex
    buffer = t_buffer;
    depth = t_depth;
    itemSize = t_itemSize;
    overwritable = t_overwritable;

    count = writeIndex = readIndex = 0;
}

void FIFO_clear() {

    count = writeIndex = readIndex = 0;

}

unsigned char FIFO_peek(void *peekBuffer) {

    if(empty())
        return 0;

    memcpy(peekBuffer, buffer + (readIndex*itemSize), itemSize);

    return 1
}

unsigned char FIFO_peekAt(void *peekBuffer, unsigned short position) {

    if(empty() || position >= count)
        return 0;

    //index based on position
    unsigned short index = readIndex+position;
    if(index > depth)
        //circular buffer
        index = index - depth;

    memcpy(peekBuffer, buffer + (index*itemSize), itemSize);

    return 1;
}

unsigned char FIFO_write(void const* data) {

    //check if fifo is full or note overwritable -> return 0 if so
    if(full() && !overwritable)
        return 0;

    //copy data to buffer + (writeIndex * itemSize)
    memcpy(buffer + (writeIndex*itemSize), data, itemSize);

    //increment writeIndex, make sure still within buffer range
    writeIndex++;
    if(writeIndex > depth)
        //circular buffer
        writeIndex = writeIndex - depth;

    //check if fifo is full
    if(full())
        readIndex = writeIndex;
    else
        count++;

    return 1;

}

unsigned short FIFO_writeMultiple(void const* data, unsigned short n) {

    if(n == 0)
        return 0;

    unsigned char* buf = (unsigned char*)data;

    unsigned short len = 0;
    while(len < n && FIFO_write(buf)) {
        len++;
        buf += itemSize;
    }

    return len;
}

unsigned char FIFO_read(void const* readBuffer) {

    if(empty())
        return 0;

    //copy data at readIndex into read buffer location
    memcpy(buf, buffer + (readIndex*itemSize), itemSize);

    //increment readIndex, make sure still within buffer range
    readIndex++;
    if(readIndex > depth)
        //circular buffer
        readIndex = readIndex - depth;

    count--;
    return 1;
}

unsigned short FIFO_readMultiple(void const* readBuffer, unsigned short n) {

    if(n == 0)
        return 0;

    unsigned char *buf = (unsigned char*)readBuffer;

    unsigned short len = 0;
    while(len < n && FIFO_read(buf)) {
        len++;
        buf += itemSize;
    }

    return len;
}

unsigned char empty() {

    if(count == 0)
        return 1;
    else
        return 0;

}

unsigned char full() {

    if(count == depth)
        return 1;
    else
        return 0;

}

unsigned short remaining() {

    return depth - count;

}
