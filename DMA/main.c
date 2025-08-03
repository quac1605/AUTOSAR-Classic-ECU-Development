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

Adc_ValueGroupType myGroup0Buffer[2];  // For 2 channels

Adc_ValueGroupType* Adc_DmaBuffer[ADC_NUM_GROUPS_DMA];
Adc_ValueGroupType myDmaBuf[2];

void MyAdcGroup0_Notification(void)
{
    Adc_ReadGroup(0, &myGroup0Buffer[0]);
}

void MyAdcGroup0_DmaCb(void){
	
}


Port_PinConfigType PortCfg_Pins[] = {
  { 
    .PortNum           = PORT_ID_A,
    .PinNum            = 0,
    .Mode              = PORT_PIN_MODE_ADC,
    .Direction         = PORT_PIN_IN,
    .speed             = 50,
    .DirectionChangeable = 0,
    .Level             = PORT_PIN_LEVEL_LOW,
    .Pull              = PORT_PIN_PULL_UP,
    .ModeChangeable    = 0
  },
  { 
    .PortNum           = PORT_ID_C,
    .PinNum            = 13,
    .Mode              = PORT_PIN_MODE_DIO,
    .Direction         = PORT_PIN_OUT,
    .speed             = 50,
    .DirectionChangeable = 0,
    .Level             = PORT_PIN_LEVEL_LOW,
    .Pull              = PORT_PIN_PULL_UP,
    .ModeChangeable    = 0
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
		.Adc_StreamBufferSize = 2,  // Size of the DMA buffer
		.Adc_StreamBufferMode = ADC_STREAM_BUFFER_CIRCULAR // Circular buffer mode
    }
};

const Adc_GroupDmaConfigType AdcGroupDmaConfig[] = {
    { 
		.ADCx = ADC1, 
		.DMA_Channel = DMA1_Channel1, 
		.DMA_FLAG_TC = DMA1_FLAG_TC1, 
		.groupId = 0, 
		.channelCount = 2, 
		.Adc_DmaNotificationCbType = MyAdcGroup0_DmaCb 
	},
    { 
		.ADCx = ADC1, 
		.DMA_Channel = DMA1_Channel1, 
		.DMA_FLAG_TC = DMA1_FLAG_TC1, 
		.groupId = 1, 
		.channelCount = 1, 
		.Adc_DmaNotificationCbType = NULL_PTR 
	}
};

const Port_ConfigType PortCfg = {
	.PinConfigs = PortCfg_Pins,
	.PinCount   = sizeof(PortCfg_Pins)/sizeof(*PortCfg_Pins)
};

int main(){

	// Initialize the pin configuration
	Port_Init(&PortCfg);
	
	Adc_Init(&Adc_Configs[0]);
	Adc_SetupResultBuffer_Dma(0, myDmaBuf);
    Adc_EnableDma(0);
	Adc_StartGroupConversion(0);
    Adc_EnableGroupNotification(0);

    while (1) {

    }
}
