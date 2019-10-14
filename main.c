/*
 * This example is a simple hexadecimal counter:
 * Push the USER button and the number on the indicator increases
 * up to 0xF and then resets to 0x0
 * If you want to see how it works w/o debouncing
 * delete the following line:
 *      #define TURN_ON_CONTACT_DEBOUNCER
 */

#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"
#include <math.h>

#define TURN_ON_CONTACT_DEBOUNCER

/**
  * System Clock Configuration
  * The system Clock is configured as follow :
  *    System Clock source            = PLL (HSI/2)
  *    SYSCLK(Hz)                     = 48000000
  *    HCLK(Hz)                       = 48000000
  *    AHB Prescaler                  = 1
  *    APB1 Prescaler                 = 1
  *    HSI Frequency(Hz)              = 8000000
  *    PLLMUL                         = 12
  *    Flash Latency(WS)              = 1
  */
static void rcc_config()
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    LL_RCC_HSI_Enable();
    while (LL_RCC_HSI_IsReady() != 1);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2,
                                LL_RCC_PLL_MUL_12);
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    SystemCoreClock = 48000000;
}

static void gpio_config(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
    
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
    return;
}

__attribute__((naked)) static void delay_10ms(void)
{
    asm ("push {r7, lr}");
    asm ("ldr r6, [pc, #8]");
    asm ("sub r6, #1");
    asm ("cmp r6, #0");
    asm ("bne delay_10ms+0x4");
    asm ("pop {r7, pc}");
    asm (".word 0x3a60"); //60000
}

uint32_t dec2hex(uint32_t number)
{
    return ( (number % 10) + (((number % 100) / 10) << 4) + (((number / 100) % 10) << 8) + ((number / 1000) << 12) );
}
	
void dyn_display(uint16_t number)
{
	static int digit_num = 0;
	static uint32_t mask = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | \
                            LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
                            LL_GPIO_PIN_6;
    static const uint32_t decoder[] = {
         LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
         LL_GPIO_PIN_4 | LL_GPIO_PIN_5, // 0
         LL_GPIO_PIN_1 | LL_GPIO_PIN_2, // 1
         LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_4 | \
         LL_GPIO_PIN_3, // 2
         LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_2 | \
         LL_GPIO_PIN_3, // 3
         LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2, //4
         LL_GPIO_PIN_0 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
         LL_GPIO_PIN_5 | LL_GPIO_PIN_6, // 5
         LL_GPIO_PIN_0 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | \
         LL_GPIO_PIN_5 | LL_GPIO_PIN_6, // 6
         LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2, // 7
         LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
         LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6, //8
         LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
         LL_GPIO_PIN_5 | LL_GPIO_PIN_6, // 9
 		 LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4 | \
         LL_GPIO_PIN_5 | LL_GPIO_PIN_6, // A
		 LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
         LL_GPIO_PIN_6, // b
		 LL_GPIO_PIN_0 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
         LL_GPIO_PIN_6, // C
 		 LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
         LL_GPIO_PIN_6, // d
         LL_GPIO_PIN_0 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
         LL_GPIO_PIN_6, // e
		 LL_GPIO_PIN_0 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6,// f
		 LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
         LL_GPIO_PIN_6, // H
    };
    uint32_t port_state = 0;
    port_state = LL_GPIO_ReadOutputPort(GPIOB); 
	uint32_t position;
	if (digit_num == 0)
		position = number % 10;
	if (digit_num == 1)
		position = (number / 10) % 10;
	if (digit_num == 2)
		position = (number / 100) % 10;
	if (digit_num == 3)
		position = number / 1000;
	port_state = (port_state & ~mask) | decoder[position];
    LL_GPIO_WriteOutputPort(GPIOB, port_state);
    switch (digit_num) {
    case 0:
		{	
			LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 3));
        	break;
		}
    case 1:
		{
			LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 2));
        	break;
		}
    case 2:
        {
			LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 1));
        	break;
		}
    case 3:
		{
			LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 4));
        	break;
		}
    default:
        break;
    }
    digit_num = (digit_num + 1) % 4;
    return;
}

void run_string(uint32_t* stringLetter, short n)
{
	static int i = 0;
	static counter = 0;
	int number = stringLetter[i % n]*1000 + stringLetter[(i+1) % n]*100 + stringLetter[(i+2) % n]*10 + stringLetter[(i+3) % n];
	counter++;
	if (counter % 100 == 0)
		i++;
	static int digit_num = 0;
    static uint32_t mask = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | \
                             LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
                             LL_GPIO_PIN_6;
    static const uint32_t decoder[] = {	
	      LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4 | \
          LL_GPIO_PIN_5 | LL_GPIO_PIN_6, // A
          0, // " "
          LL_GPIO_PIN_0 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
          LL_GPIO_PIN_6, // C
          LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
          LL_GPIO_PIN_6, // d
          LL_GPIO_PIN_0 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
          LL_GPIO_PIN_6, // e
          LL_GPIO_PIN_0 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6,// f
          LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
          LL_GPIO_PIN_6, // H
          LL_GPIO_PIN_4 | LL_GPIO_PIN_5, // l
		  LL_GPIO_PIN_2 | LL_GPIO_PIN_4 | LL_GPIO_PIN_6 | LL_GPIO_PIN_3,// o
		  LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_4 | \
          LL_GPIO_PIN_5 | LL_GPIO_PIN_6, // p
	};
	uint32_t port_state = 0;
    port_state = LL_GPIO_ReadOutputPort(GPIOB);
    uint32_t position;
    if (digit_num == 0)
        position = number % 10;
    if (digit_num == 1)
        position = (number / 10) % 10;
    if (digit_num == 2)
        position = (number / 100) % 10;
    if (digit_num == 3)
        position = number / 1000;
    port_state = (port_state & ~mask) | decoder[position];
    LL_GPIO_WriteOutputPort(GPIOB, port_state);
	switch (digit_num) {
     case 0:
         {
             LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 3));
             break;
         }
     case 1:
         {
             LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 2));
             break;
         }
     case 2:
         {
             LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 1));
             break;
         }
     case 3:
         {
             LL_GPIO_WriteOutputPort(GPIOC, ~(1 << 4));
             break;
         }
     default:
         break;
     }
     digit_num = (digit_num + 1) % 4;
     return;
}

int main(void)
{
    uint32_t number = 250;
#if defined(TURN_ON_CONTACT_DEBOUNCER)
    uint32_t debouncer_clk = 0;
    uint32_t button_pressed = 0;
#endif
    rcc_config();
    gpio_config();	
//----------------------------------------------------------/
/*    while (1) {
#if defined(TURN_ON_CONTACT_DEBOUNCER)
        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)) {
            button_pressed = 1;
            debouncer_clk = 0;
        }
        if (button_pressed) {
            debouncer_clk++;
            delay_10ms();
        }
        
        if (debouncer_clk >= 5) {
            LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);				//кусок кода, отвечающий за инкрементацию счетчика по кнопке
            number++;
            button_pressed = 0;
            debouncer_clk = 0;
        }
		dyn_display(number);
		delay_10ms();
#else
        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)) {
            LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
            number++;
        }
		dyn_display(number);
		delay_10ms();
#endif*
//------------------------------------------------------------/	
*/    
    																//кусок кода отвечающий за командную строку
	/*int a[] = {6, 4, 7, 7, 8, 1, 9, 4, 8, 9, 7, 4, 1};

	while (1)
	{
		run_string(a, 13);
		delay_10ms();
	}
//-------------------------------------------------------------/

	}*/
    return 0;
}
