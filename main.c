#include "stm32f030x6.h"
#include "rcc.h"
#include "usart.h"
#include "delay.h"

int main(void)
{
	init_RCC();
	init_USART1(USART1_BAUDRATE_115200);
	print_bin_USART1(255);
	while (1)
	{
	}
}
