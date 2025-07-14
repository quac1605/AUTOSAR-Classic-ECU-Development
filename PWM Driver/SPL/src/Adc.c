/*
 * Adc.c
 * AUTOSAR ADC Driver implementation for STM32F103C8
 */

#include "Adc.h"
#include "Port.h"
#include "stm32f10x.h"
#include <stddef.h>

Adc_GroupDefType Adc_Groups[MAX_ADC_GROUPS] = { 0 };

ADC_InitTypeDef ADC_InitStruct;
void Port_ConfigAdcPin(uint8 portNum, uint8 pinNum)
{
    Port_PinConfigType adcPinCfg;

    adcPinCfg.PortNum = portNum;
    adcPinCfg.PinNum = pinNum;
    adcPinCfg.Mode = PORT_PIN_MODE_ADC;
    adcPinCfg.Direction = PORT_PIN_IN;
    adcPinCfg.speed = 2;  // Speed doesn't matter for analog, but must be set
    adcPinCfg.Pull = PORT_PIN_PULL_NONE;
    adcPinCfg.Level = PORT_PIN_LEVEL_LOW;
    adcPinCfg.DirectionChangeable = 0;
    adcPinCfg.ModeChangeable = 0;

    Port_ApplyPinConfig(&adcPinCfg);
}

void Adc_Init(const Adc_ConfigType* ConfigPtr)
{
    if (ConfigPtr == NULL_PTR) return;
    // Initialize ADC peripheral based on ConfigPtr
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // DMA1 for ADC1/ADC2

    // Select ADC instance

    ADC_TypeDef* adcInstance = NULL;
    if (ConfigPtr->Instance == ADC_1) {
        adcInstance = ADC1;
    } else if (ConfigPtr->Instance == ADC_2) {
        adcInstance = ADC2;
    } else {
        return; // Invalid ADC instance
    }

    // Configure all channel GPIOs 
    for (uint8 i = 0; i < ConfigPtr->numChannels; i++) {
        uint8 channel = ConfigPtr->Channel[i].ChannelId;   
        uint8 portNum = (channel <= 7) ? PORT_ID_A :
                        (channel <= 9) ? PORT_ID_B :
                        (channel <= 15) ? PORT_ID_C : 0xFF;  
        uint8 pinNum = (channel <= 7) ? channel :
                       (channel <= 9) ? (channel - 8) :
                       (channel <= 15) ? (channel - 10) : 0xFF;  
        if (portNum != 0xFF && pinNum != 0xFF) {
            Port_ConfigAdcPin(portNum, pinNum);
        }
    }
    // Initialize ADC peripheral
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; // Independent mode for ADC1/ADC2
    ADC_InitStruct.ADC_ContinuousConvMode = (ConfigPtr->ConvMode == ADC_CONV_MODE_CONTINUOUS) ? ENABLE : DISABLE; // Single channel conversion
    ADC_InitStruct.ADC_ScanConvMode = DISABLE; // Single channel
    ADC_InitStruct.ADC_ExternalTrigConv = (ConfigPtr->TriggerSource == ADC_TRIGG_SRC_SW) ? ADC_ExternalTrigConv_None : ADC_ExternalTrigConv_T1_CC1; // No external trigger
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;

    ADC_Init(adcInstance, &ADC_InitStruct);

    // Setting up channel configurations
    for (uint8 i = 0;  i < ConfigPtr->numChannels;  i++) {
        Adc_ChannelType chanId = ConfigPtr->Channel[i].ChannelId;
        uint8           rank   = ConfigPtr->Channel[i].Rank;
        Adc_SamplingTimeType samp = ConfigPtr->Channel[i].SamplingTime;

        if (ConfigPtr->Instance == ADC_1) {
            // ADC1 channels are 0–15
            ADC_RegularChannelConfig(adcInstance,
                                     chanId,
                                     rank,
                                     samp);
        } else {
            // ADC2 channels are 16–31
            ADC_RegularChannelConfig(adcInstance,
                                     chanId - 16,
                                     rank,
                                     samp);
        }
    }

    //Turn on ADC
    ADC_Cmd(adcInstance, ENABLE);

}

void Adc_DeInit(void)
{
    // Deinitialize ADC peripheral
    ADC_StructInit(&ADC_InitStruct); 

    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);  // Disable DMA clock
}
        
extern Adc_GroupDefType Adc_Groups[];

Std_ReturnType Adc_SetupResultBuffer(Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr)
{
    // Check for null pointer
    if (DataBufferPtr == NULL_PTR) {
        return E_NOT_OK;
    }

    // Basic range check - assumes max group count is known (you can define MAX_ADC_GROUPS)
    if (Group >= MAX_ADC_GROUPS) {
        return E_NOT_OK;
    }

    // Assign the buffer pointer to the group
    Adc_Groups[Group].Result = DataBufferPtr;

    // Optionally reset group status
    Adc_Groups[Group].Status = ADC_IDLE;

    return E_OK;
}

void Adc_StartGroupConversion(Adc_GroupType Group){
    // Check if group is valid
    if (Group >= MAX_ADC_GROUPS) {
        return; // Invalid group
    }

    // Get the ADC instance for the group       
    Adc_ChannelType channel = Adc_Groups[Group].Channels[0];
    ADC_TypeDef* adcInstance = (channel <= 15) ? ADC1 : ADC2;

    // Start conversion
    ADC_SoftwareStartConvCmd(adcInstance, ENABLE);

    // Update group status
    Adc_Groups[Group].Status = ADC_BUSY;
}

void Adc_StopGroupConversion(Adc_GroupType Group){
    // Check if group is valid
    if (Group >= MAX_ADC_GROUPS) {
        return; // Invalid group
    }

    // Get the ADC instance for the group
    Adc_ChannelType channel = Adc_Groups[Group].Channels[0];
    ADC_TypeDef* adcInstance = (channel <= 15) ? ADC1 : ADC2;

    // Stop conversion
    ADC_SoftwareStartConvCmd(adcInstance, DISABLE);

    // Update group status
    Adc_Groups[Group].Status = ADC_IDLE;
}

void Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr)
{
    // Check for valid group and buffer pointer
    if (Group >= MAX_ADC_GROUPS || DataBufferPtr == NULL_PTR) {
        return;
    }

    Adc_GroupDefType* group = &Adc_Groups[Group];

    // Get first channel to identify ADC instance
    Adc_ChannelType channel = group->Channels[0];
    ADC_TypeDef* adcInstance = NULL_PTR;

    if (channel <= 15) {
        adcInstance = ADC1;
    } else if (channel <= 31) {
        adcInstance = ADC2;
    } else {
        return; // Invalid channel
    }

    // Read ADC result register
    uint16_t adcValue = ADC_GetConversionValue(adcInstance);

    // Save value into user buffer
    *DataBufferPtr = (Adc_ValueGroupType)(adcValue & 0xFF);  // Trim to 8-bit if needed

    // Update internal buffer pointer if available
    if (group->Result != NULL_PTR) {
        group->Result[0] = *DataBufferPtr;
    }

    // Mark conversion as completed
    group->Status = ADC_COMPLETED;
}

void Adc_EnableHardwareTrigger(Adc_GroupType Group)
{
    // Validate group index
    if (Group >= MAX_ADC_GROUPS) {
        return;
    }

    Adc_GroupDefType* group = &Adc_Groups[Group];
    Adc_ChannelType channel = group->Channels[0];
    ADC_TypeDef* adc = NULL;

    // Determine ADC instance from channel ID
    if (channel <= 15) {
        adc = ADC1;
    } else if (channel <= 31) {
        adc = ADC2;
    } else {
        return; // Invalid channel ID
    }

    // Step 1: Clear existing external trigger selection bits (EXTSEL)
    adc->CR2 &= ~(0xE0000);  // Clear bits 17:19

    // Step 2: Set external trigger to ADC_ExternalTrigConv_T1_CC1 (value = 0x00000000)
    // No need to OR anything, already cleared to 0, but you can do it explicitly:
    adc->CR2 |= ADC_ExternalTrigConv_T1_CC1;

    // Step 3: Enable external trigger by setting EXTTRIG (bit 20)
    adc->CR2 |= (1 << 20);  // EXTTRIG bit

    // Optional: Set group status if you track it
    group->Status = ADC_IDLE;
}

void Adc_DisableHardwareTrigger(Adc_GroupType Group)
{
    // Validate the group ID
    if (Group >= MAX_ADC_GROUPS) {
        return;
    }

    Adc_GroupDefType* group = &Adc_Groups[Group];
    Adc_ChannelType channel = group->Channels[0];
    ADC_TypeDef* adc = NULL;

    // Determine which ADC peripheral to use
    if (channel <= 15) {
        adc = ADC1;
    } else if (channel <= 31) {
        adc = ADC2;
    } else {
        return;  // Invalid channel
    }

    // Step 1: Clear the EXTSEL bits (bits 17:19)
    adc->CR2 &= ~(0xE0000);  // Clear EXTSEL

    // Step 2: Set EXTSEL to ADC_ExternalTrigConv_None (0x000E0000)
    adc->CR2 |= ADC_ExternalTrigConv_None;

    // Step 3: Clear EXTTRIG bit (bit 20) to disable external trigger
    adc->CR2 &= ~(1 << 20);
}

Adc_StatusType Adc_GetGroupStatus(Adc_GroupType Group)
{
    return Adc_Groups[Group].Status;
}

void Adc_GetVersionInfo (Std_VersionInfoType* VersionInfo)
{
    VersionInfo->vendorID = 0x1234;
    VersionInfo->moduleID = 0x5678;
    VersionInfo->sw_major_version = 1;
    VersionInfo->sw_minor_version = 0;
    VersionInfo->sw_patch_version = 0;
}





    
