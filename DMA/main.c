#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "Dio.h"
#include "Port.h"
#include "Pwm.h"
#include "Std_Types.h"


// Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
// 	{
// 	.PinNum = 0,				
// 	.Mode = PORT_PIN_MODE_PWM, // PWM mode
// 	.Direction = PORT_PIN_OUT, // Output direction
// 	.speed = 50, 				// 50MHz
// 	.DirectionChangeable = 0, 	// Direction cannot be changed at runtime
// 	.Level = 0, 				// Initial level is low},
// 	.Pull = PORT_PIN_PULL_UP,
// 	.ModeChangeable = 0, 		// Mode cannot be changed at runtime
// 	.PortNum = PORT_ID_A		// Port A, Pin 0
// 	} 		

// };

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

// Pwm_ChannelConfigType Pwm_Channels = {
// 	{
// 	.Channel = 5, // Channel 5
// 	.classType = PWM_FIXED_PERIOD,
// 	.defaultPeriode = 20000, // Default period in ms
// 	.compareValue = 500, // Default duty cycle 50%
// 	.polarity = PWM_HIGH,
// 	.idleState = PWM_High,
// 	.Notification = NULL // No notification callback
// 	}
// };


// void Pwm_Channel0_Notification(void)
// {
//     GPIOC->ODR ^= GPIO_Pin_13; // Toggles LED
// }

int main(){

	// Initialize the pin configuration
	Port_ConfigType PortCfg = {
	.PinConfigs = PortCfg_Pins, // Pointer to the array of pin configurations
	.PinCount = 1 				// Number of configured pins
	};
	Port_Init(&PortCfg); 
	
	// // Initialize the PWM driver with the configuration
	// Pwm_ConfigType PwmConfig = {
	// .Channels = Pwm_Channels, 	// Pointer to the PWM channel configurations
	// .numChannels = 1 			// Number of configured PWM channels
	// };
	// Pwm_Init(&PwmConfig);
	
	Adc_Init(&Adc_Config);
	// 2. Setup buffer cho group 0
    Adc_SetupResultBuffer(0, myGroup0Buffer);
    // 3. Enable notification và start conversion
	Adc_EnableDma(0);
    Adc_EnableGroupNotification(0);
    Adc_StartGroupConversion(0);


	// Pwm_Init(&PwmDriverConfig);
    // // Enable both edges for channel 0
    // Pwm_EnableNotification(0, PWM_BOTH_EDGES);

	while(1){
		
    }
}
