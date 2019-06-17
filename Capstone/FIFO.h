#ifndef FIFO_H_
#define FIFO_H_

unsigned char *buffer;
unsigned short depth;
unsigned char itemSize;
unsigned char overwritable;
volatile unsigned short count;
volatile unsigned short writeIndex;
volatile unsigned short readIndex;

void InitializeFIFO(unsigned char* t_buffer, unsigned short t_depth, unsigned char t_itemSize, unsigned char t_overwritable);
void FIFO_clear();
unsigned char FIFO_peek(void *peekBuffer);
unsigned char FIFO_peekAt(void *peekBuffer, unsigned short position);
unsigned char FIFO_write(void const* data);
unsigned short FIFO_writeMultiple(void const* data, unsigned short n);
unsigned char FIFO_read(void const* readBuffer);
unsigned short readMultiple(void const* readbuffer, unsigned short n);
unsigned char empty();
unsigned char full();
unsigned short remaining();

#endif /* FIFO_H_ */
