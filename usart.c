#include "usart.h"
#include <string.h>

ring_buffer rx_buffer = {{0}, 0, 0};
ring_buffer tx_buffer = {{0}, 0, 0};

void USART1_IRQHandler(void);
void print_base(long n, uint8_t base);
void store_char(uint8_t c, ring_buffer *buffer);

void init_USART1(uint32_t baudrate)
{
	//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	// USART1 init on PA2 -> Tx & P3 -> Rx
	//------------------------------------------------------------------
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	GPIOA->MODER &= ~((GPIO_MODER_MODER2) | (GPIO_MODER_MODER3));
	GPIOA->MODER |= (GPIO_MODER_MODER2_1) | (GPIO_MODER_MODER3_1);
	GPIOA->MODER |= GPIO_MODER_MODER4_0;
	GPIOA->ODR |= GPIO_ODR_4;
	GPIOA->AFR[0] &= ~0x0000FF00;
	GPIOA->AFR[0] |= 0x00001100;

	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_0; //
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_0; //

	USART1->BRR = baudrate; // default 1 stop bit, 8 data bits, no parity no flow

	USART1->CR1 = (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE); // enable transmiter, reciver, rxne interrupt
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

void store_char(uint8_t c, ring_buffer *buffer)
{
	uint8_t i = (buffer->head + 1) % USART_BUFFER_SIZE;
	if (i != buffer->tail)
	{
		buffer->buffer[buffer->head] = c;
		buffer->head = i;
	}
}

uint8_t read_USART1()
{
	if (rx_buffer.head != rx_buffer.tail)
	{
		uint8_t recived_char = rx_buffer.buffer[rx_buffer.tail];
		rx_buffer.tail = (rx_buffer.tail + 1) % USART_BUFFER_SIZE;
		return recived_char;
	}
	return 0;
}

uint8_t peak_USART1()
{
	if (rx_buffer.head != rx_buffer.tail)
	{
		uint8_t recived_char = rx_buffer.buffer[rx_buffer.tail];
		return recived_char;
	}
	return 0;
}

void write_USART1(uint8_t c)
{
	uint8_t i = (tx_buffer.head + 1) % USART_BUFFER_SIZE;
	tx_buffer.buffer[tx_buffer.head] = c;
	tx_buffer.head = i;
	USART1->CR1 |= USART_CR1_TXEIE; // Enable USART TX interupt
}

void print_string_USART1(const uint8_t *s)
{
	while (*s)
	{
		write_USART1(*s++);
	}
}

void print_number_USART1(long n)
{
	print_base(n, 10);
}

void print_bin_USART1(long n)
{
	print_string_USART1("b");
	print_base(n, 2);
}

void print_hex_USART1(long n)
{
	print_string_USART1("0x");
	print_base(n, 16);
}

uint8_t available_USART1()
{
	return (USART_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % USART_BUFFER_SIZE;
}

void USART1_IRQHandler()
{

	/* if DR is not empty and the Rx Int is enabled */
	if (((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE) && ((USART1->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE))
	{
		uint8_t c = USART1->RDR;	 /* Read data register */
		store_char(c, &rx_buffer); // store data in buffer
		return;
	}

	/*If interrupt is caused due to Transmit Data Register Empty */
	if (((USART1->ISR & USART_ISR_TXE) == USART_ISR_TXE) && ((USART1->CR1 & USART_CR1_TXEIE) == USART_CR1_TXEIE))
	{
		if (tx_buffer.head == tx_buffer.tail)
		{
			// Buffer empty, so disable interrupts
			USART1->CR1 &= ~USART_CR1_TXEIE;
		}
		else
		{
			// There is more data in the output buffer. Send the next byte
			uint8_t c = tx_buffer.buffer[tx_buffer.tail];
			tx_buffer.tail = (tx_buffer.tail + 1) % USART_BUFFER_SIZE;
			USART1->TDR = c;
		}
		return;
	}
}

void print_base(long n, uint8_t base)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *s = &buf[sizeof(buf) - 1];

	*s = '\0';

	// prevent crash if called with base == 1
	if (base < 2)
		base = 10;

	do
	{
		unsigned long m = n;
		n /= base;
		char c = m - base * n;
		*--s = c < 10 ? c + '0' : c + 'A' - 10;
	} while (n);

	while (*s)
	{
		write_USART1(*s++);
	}
}
