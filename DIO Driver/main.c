#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "Dio.h"

void TIM_Config(TIM_TypeDef *TIM_Struct){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_Prescaler = 7200 -1;  		//0.1ms ++
	TIM_InitStruct.TIM_Period = 0xFFFF;				//No overflow
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM_Struct, &TIM_InitStruct);
	TIM_Cmd(TIM_Struct, ENABLE);
}

void delay_ms(TIM_TypeDef *TIM_Struct,uint16_t time){
	TIM_SetCounter(TIM_Struct, 0);
	while(TIM_GetCounter(TIM_Struct) < time * 10);
}

void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}	



int main(){
	RCC_Config();
	TIM_Config(TIM2);
	GPIO_Config();
	while(1){
//		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1){
//			GPIO_SetBits(GPIOC, GPIO_Pin_13);
//			delay_ms(TIM2, 10000);
//			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//			delay_ms(TIM2, 10000);
//		}  
//		else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//	}
//			GPIO_SetBits(GPIOC, GPIO_Pin_13);
//			delay_ms(TIM2, 100);
//			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//			delay_ms(TIM2, 100);

        Dio_WriteChannel(45, STD_HIGH);
		delay_ms(TIM2, 100); // Delay for 1 second
		Dio_WriteChannel(45, STD_LOW);
		delay_ms(TIM2, 100); // Delay for 1 second
		
    }
}
