#include "stm32f030x6.h"
#include "rcc.h"
#include "usart.h"
#include "delay.h"

int main(void)
{
	init_RCC();
	init_USART1(USART1_BAUDRATE_115200);
	while (1)
	{
		while (available_USART1() > 20)
		{
			char str[50] = "";
			read_string_USART1(str);
			print_string_USART1(str);
		}
	}
}
