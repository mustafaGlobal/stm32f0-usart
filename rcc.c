#include "rcc.h"

void init_RCC(void)
{
  //wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
  // System Clock Configuration
  // System Clock source            = PLL (HSI48)
  // SYSCLK(Hz)                     = 48000000
  // HCLK(Hz)                       = 48000000
  // AHB Prescaler                  = 1
  // APB1 Prescaler                 = 1
  // HSI Frequency(Hz)              = 48000000
  // PREDIV                         = 2
  // PLLMUL                         = 2
  // Flash Latency(WS)              = 1
  //wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

  RCC->CR |= (0x00010000);
  // wait until ready
  while ((RCC->CR & 0x00020000) == 0)
    ;

  // disable PLL
  RCC->CR &= ~(RCC_CR_PLLON);
  while ((RCC->CR & (RCC_CR_PLLRDY)) == (RCC_CR_PLLRDY))
    ;

  RCC->CFGR2 = 0x00000000;
  RCC->CFGR &= ~0x003C0003;

  RCC->CFGR |= 0x00110001;

  // enable PLL
  RCC->CR |= (RCC_CR_PLLON);
  while ((RCC->CR & (RCC_CR_PLLRDY)) == 0)
    ;
  GPIOA->ODR &= ~0x0010;

  FLASH->ACR |= (FLASH_ACR_LATENCY);

  // HCLK Configuration
  RCC->CFGR &= ~(RCC_CFGR_HPRE);
  RCC->CFGR |= (RCC_CFGR_HPRE_DIV1);

  // SYSCLK Configuration
  RCC->CFGR &= ~(RCC_CFGR_SW);
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  while ((RCC->CFGR & (RCC_CFGR_SWS)) != 0x00000008)
    ;

  RCC->CFGR &= ~(RCC_CFGR_PPRE);
  RCC->CFGR |= (RCC_CFGR_PPRE_DIV1);
  RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;
  RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK;
}
