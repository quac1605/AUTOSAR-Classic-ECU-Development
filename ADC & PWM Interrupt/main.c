#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "Port.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Pwm.h"
#include "Pwm_Cfg.h"
#include "Std_Types.h"

static volatile uint32_t msTicks;
void SysTick_Handler(void) { msTicks++; }
static void Delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms) { }
}

Adc_ValueGroupType myGroup0Buffer[2];  // For 2 channels

void MyAdcGroup0_Notification(void)
{
    Adc_ReadGroup(0, &myGroup0Buffer[0]);
}

void Pwm_Channel0_Notification(void)
{
	if (Dio_ReadChannel(DIO_CHANNEL_C13) == STD_HIGH) {
		Dio_WriteChannel(DIO_CHANNEL_C13, STD_LOW);
	} else {
		Dio_WriteChannel(DIO_CHANNEL_C13, STD_HIGH);
	}
}

/* Port (PA0) */
Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
    {
      .PortNum           = PORT_ID_A,
      .PinNum            = 0,
      .Mode              = PORT_PIN_MODE_PWM,
      .Direction         = PORT_PIN_OUT,
      .speed             = 50,
      .DirectionChangeable = 0,
      .Level             = PORT_PIN_LEVEL_LOW,
      .Pull              = PORT_PIN_PULL_NONE,
      .ModeChangeable    = 0
    },
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

const Port_ConfigType PortCfg = {
	.PinConfigs = PortCfg_Pins,
	.PinCount   = sizeof(PortCfg_Pins)/sizeof(*PortCfg_Pins)
};

// Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
// 	{
// 	.PinNum = 0,				// Pin 45 corresponds to PC13
// 	.Mode = PORT_PIN_MODE_ADC,
// 	.Direction = PORT_PIN_IN,
// 	.speed = 50, 				// 50MHz
// 	.DirectionChangeable = 0, 	// Direction cannot be changed at runtime
// 	.Level = 0, 				// Initial level is low},
// 	.Pull = PORT_PIN_PULL_UP,
// 	.ModeChangeable = 0, 		// Mode cannot be changed at runtime
// 	.PortNum = PORT_ID_A		// Port C, Pin 13
// 	} 		

// };

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

Pwm_ChannelConfigType Pwm_Channels[] = {
	{
	.Channel = 4, // Channel 4
	.classType = PWM_FIXED_PERIOD,
	.defaultPeriode = 20000, // Default period in ms
	.compareValue = SERVO_CENTER_PULSE_US, // Default duty cycle 50%
	.polarity = PWM_HIGH,
	.idleState = PWM_HIGH,
	.NotificationEnable = PWM_NOTIFICATION_ON, // Enable notification
	.NotificationCb = Pwm_Channel0_Notification // Notification callback
	}
};

const Pwm_ConfigType PwmConfig = {
	.Channels = Pwm_Channels, // Pointer to the PWM channel configurations
	.numChannels = 1 // Number of configured PWM channels
};


int main(){

	SystemInit();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

	// Initialize the pin configuration
	Port_Init(&PortCfg);
	
	// Adc_Init(&Adc_Configs[0]);
	// Adc_StartGroupConversion(0);
    // Adc_EnableGroupNotification(0);
    

	Pwm_Init(&PwmConfig);
    // Enable both edges for channel 0
    Pwm_EnableNotification(0, PWM_BOTH_EDGES);

	/* let servo center */
    Delay_ms(500);

    while (1) {
        /* extreme “0” */
        Pwm_SetDutyCycle(0, 0);
        Delay_ms(500);

        /* extreme “100” */
        Pwm_SetDutyCycle(0, 100);
        Delay_ms(500);
    }
}
