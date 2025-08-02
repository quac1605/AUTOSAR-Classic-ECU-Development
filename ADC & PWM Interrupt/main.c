#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "Dio.h"
#include "Port.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Pwm.h"
#include "Pwm_Cfg.h"
#include "Std_Types.h"

Adc_ValueGroupType myGroup0Buffer[2];  // For 2 channels

void MyAdcGroup0_Notification(void)
{
     Adc_ReadGroup(0, &myGroup0Buffer[0]);
}
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

Adc_ConfigType Adc_Configs[2] = {
	{
	.ConvMode = ADC_CONV_MODE_CONTINUOUS,
	.TriggerSource = ADC_TRIGG_SRC_SW,
	.NotificationEnable = ADC_NOTIFICATION_ON,
	.numChannels = 1,
	.Instance = ADC_1, // Using ADC1
	.ResultAlignment = ADC_ALIGN_RIGHT,
	.Adc_NotificationCbType = MyAdcGroup0_Notification, // Callback function for notifications
	.Channel = {{ 	.ChannelId = 0, 
					.SamplingTime = ADC_SampleTime_1Cycles5, // Sampling time of 1.5 cycles
					.Rank = 1 
				} // Channel 0, Sampling time 1, Rank 1
	}
	}
};

// Define the group configuration array
Adc_GroupDefType Adc_Groups[] = {
    {
        .GroupId = 0,
		.AdcInstance = ADC_1, 		// Using ADC1
        .Channels = {0},         	// Use channel 0
        .Priority = 0,           	// Low priority
        .numChannels = 1,
        .Status = ADC_IDLE,
        .Result = myGroup0Buffer, // Pointer to the result buffer
		.Adc_StreamEnableType = 1, // Enable DMA streaming
		.Adc_StreamBufferSize = 1,  // Size of the DMA buffer
		.Adc_StreamBufferMode = ADC_STREAM_BUFFER_CIRCULAR // Circular buffer mode
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
	
	Adc_Init(&Adc_Configs[0]);
	Adc_StartGroupConversion(0);
    Adc_EnableGroupNotification(0);
    

	// Pwm_Init(&PwmDriverConfig);
    // // Enable both edges for channel 0
    // Pwm_EnableNotification(0, PWM_BOTH_EDGES);

	while(1){
		
    }
}
