#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "Dio.h"
#include "Port.h"

void TIM_Config(TIM_TypeDef *TIM_Struct){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // ✅ Enable TIM2 clock

    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = 7200 - 1;  	// 0.1ms tick
    TIM_InitStruct.TIM_Period = 0xFFFF;
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM_Struct, &TIM_InitStruct);
    TIM_Cmd(TIM_Struct, ENABLE);
}

void delay_ms(TIM_TypeDef *TIM_Struct,uint16_t time){
	TIM_SetCounter(TIM_Struct, 0);
	while(TIM_GetCounter(TIM_Struct) < time * 10);
}


Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
	{
	.PinNum = 13,				// Pin 45 corresponds to PC13
	.Mode = PORT_PIN_MODE_DIO,
	.Direction = PORT_PIN_OUT,
	.speed = 50, 				// 50MHz
	.DirectionChangeable = 0, 	// Direction cannot be changed at runtime
	.Level = 0, 				// Initial level is low},
	.Pull = PORT_PIN_PULL_UP,
	.ModeChangeable = 0, 		// Mode cannot be changed at runtime
	.PortNum = PORT_ID_C		// Port C, Pin 13
	} 		

};

int main(){

	// Initialize the system clock
	TIM_Config(TIM2);

	// Setup PC13
    Port_ConfigType PortCfg = {
        .PinConfigs = PortCfg_Pins,
        .PinCount = 1
    };
    Port_Init(&PortCfg);
	
	while(1){
        Dio_WriteChannel(DIO_CHANNEL_C13, STD_HIGH);
		delay_ms(TIM2, 100); 
		Dio_WriteChannel(DIO_CHANNEL_C13, STD_LOW);
		delay_ms(TIM2, 100);
    }
}
