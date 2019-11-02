#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_cortex.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int button_is_pressed = 0;
int digit = 0;
int random_number = 0;

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
    //LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    //LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
    return;
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

static void exti_config(void)
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE1);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_1);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_1);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_1);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_0);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_0);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_SetPriority(EXTI0_1_IRQn, 0);
}

static int counter_top = 1000;
void EXTI0_1_IRQHandler(void)
{
    button_is_pressed = 1;
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
}

static void systick_config(void)
{
    LL_InitTick(48000000, 1000);
    LL_SYSTICK_EnableIT();
    NVIC_SetPriority(SysTick_IRQn, 0);
    return;
}

void SysTick_Handler(void)
{
    static int counter = 0;
    random_number = (counter % 10) * 1000000;
    counter++;
    dyn_display(digit);
}

int main(void)
{
    rcc_config();
    gpio_config();
    exti_config();
    systick_config();
    int count = 1;
    while (count)
	{	
		int flag = 1;
		int score = 0;
		int delta = 1000;
		int level = 25;
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
		int delay = random_number;
		for (int i = 0; i < delay / level; i++)
		{
			if (button_is_pressed == 1 && flag == 1)
			{
				digit++;
				level = level + 300;
				flag = 0;	
			}
		}
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
		//if (flag == 1 && count++ > 1000)
		//	digit = digit - delta;
		delay = random_number;
		for (int i = 0; i < delay; i++);
		button_is_pressed = 0;	
	}
    return 0;
}
