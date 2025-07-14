#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "Dio.h"
#include "Port.h"
#include "Adc.h"
#include "Std_Types.h"

void TIM_Config(TIM_TypeDef *TIM_Struct){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Enable clock for TIM2

	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_Prescaler = 7200 -1;  	//0.1ms ++
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


Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
	{
	.PinNum = 0,				// Pin 45 corresponds to PC13
	.Mode = PORT_PIN_MODE_ADC,
	.Direction = PORT_PIN_IN,
	.speed = 50, 				// 50MHz
	.DirectionChangeable = 0, 	// Direction cannot be changed at runtime
	.Level = 0, 				// Initial level is low},
	.Pull = PORT_PIN_PULL_UP,
	.ModeChangeable = 0, 		// Mode cannot be changed at runtime
	.PortNum = PORT_ID_A		// Port C, Pin 13
	} 		

};

Adc_ConfigType Adc_Config = {
	.ConvMode = ADC_CONV_MODE_CONTINUOUS,
	.TriggerSource = ADC_TRIGG_SRC_SW,
	.NotificatioEnable = ADC_NOTIFICATION_OFF,
	.numChannels = 1,
	.Instance = ADC_1, // Using ADC1
	.ResultAlignment = ADC_ALIGN_RIGHT,
	.Channel = {{ 	.ChannelId = 0, 
					.SamplingTime = ADC_SampleTime_1Cycles5, // Sampling time of 1.5 cycles
					.Rank = 1 
				} // Channel 0, Sampling time 1, Rank 1
	}
};

int main(){

	TIM_Config(TIM2);
	Port_ConfigType PortCfg = {
	.PinConfigs = PortCfg_Pins, // Pointer to the array of pin configurations
	.PinCount = 1 				// Number of configured pins
	};
	// Initialize the GPIO pin for output
	Port_Init(&PortCfg); // Initialize pin 45 (PC13) as per configuration
	Adc_Init(&Adc_Config); // Initialize ADC with the configuration

	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // Start ADC conversion

	uint16 count = 0;
	
	while(1){
		// if(Adc_ReadGroup(0, &count) == E_NOT_OK) { 
		// 	// Handle error if reading fails
		// 	continue;	
		// }else {
			
		// }

        count = ADC_GetConversionValue(ADC1); // Read ADC value from channel 0
    }
}
