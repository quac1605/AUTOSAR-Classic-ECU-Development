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


Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
	{
	.PinNum = 0,				
	.Mode = PORT_PIN_MODE_PWM, // PWM mode
	.Direction = PORT_PIN_OUT, // Output direction
	.speed = 50, 				// 50MHz
	.DirectionChangeable = 0, 	// Direction cannot be changed at runtime
	.Level = 0, 				// Initial level is low},
	.Pull = PORT_PIN_PULL_UP,
	.ModeChangeable = 0, 		// Mode cannot be changed at runtime
	.PortNum = PORT_ID_A		// Port A, Pin 0
	} 		

};

Pwm_ChannelConfigType Pwm_Channels = {
	{
	.Channel = 5, // Channel 5
	.classType = PWM_FIXED_PERIOD,
	.defaultPeriode = 20000, // Default period in ms
	.compareValue = 500, // Default duty cycle 50%
	.polarity = PWM_HIGH,
	.idleState = PWM_High,
	.Notification = NULL // No notification callback
	}
};

int main(){

	// Initialize the pin configuration
	Port_ConfigType PortCfg = {
	.PinConfigs = PortCfg_Pins, // Pointer to the array of pin configurations
	.PinCount = 1 				// Number of configured pins
	};
	Port_Init(&PortCfg); 
	
	// Initialize the PWM driver with the configuration
	Pwm_ConfigType PwmConfig = {
	.Channels = Pwm_Channels, 	// Pointer to the PWM channel configurations
	.numChannels = 1 			// Number of configured PWM channels
	};
	Pwm_Init(&PwmConfig);
	
	
	while(1){
		
    }
}
