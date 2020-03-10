#ifndef __USART_H_
#define __USART_H_

#include "stm32f030x6.h"

#define USART1_BAUDRATE_115200 0x000001A0U
#define USART_BUFFER_SIZE 64

typedef struct
{
  uint8_t buffer[USART_BUFFER_SIZE];
  volatile uint8_t head;
  volatile uint8_t tail;
} ring_buffer;

void init_USART1(uint32_t baudrate);

int read_USART1(void);
long parseInt_USART1(void);
void read_bytes_USART1(char *buffer, uint8_t length);
void read_string_USART1(char *buffer);
uint8_t peak_USART1(void);
void write_USART1(uint8_t c);
uint8_t available_USART1(void);

void print_string_USART1(const char *s);
void print_number_USART1(long n);
void print_bin_USART1(long n);
void print_hex_USART1(long n);

#endif
