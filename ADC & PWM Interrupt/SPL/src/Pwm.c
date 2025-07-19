/*
 * Pwm.c
 * AUTOSAR ADC Driver implementation for STM32F103C8
 */

#include "Pwm.h"
#include "Port.h"
#include "stm32f10x.h"
#include "misc.h"  // NVIC_InitTypeDef
#include <stddef.h>

Pwm_ConfigType* Pwm_CurrentConfigPtr; // Global configuration structure pointer

/**
 * @brief Initializes the PWM driver with the given configuration.
 * @param ConfigPtr Pointer to the configuration structure.
 */
void Pwm_Init(const Pwm_ConfigType* ConfigPtr) {
    if (ConfigPtr == NULL || ConfigPtr->Channels == NULL) {
        return; // Invalid configuration
    }

    Pwm_CurrentConfigPtr = ConfigPtr; // Store the configuration pointer
    for (uint8 i = 0; i < ConfigPtr->numChannels; i++) {
        const Pwm_ChannelConfigType* channelConfig = &ConfigPtr->Channels[i];
        TIM_TypeDef* tim = ConfigPtr->Channels[i].TIMx; // Get the TIM instance for the channel
        
        if (tim == NULL) {
            continue; // Invalid channel
        }

        // Initialize the TIM peripheral with the channel configuration
        TIM_TimeBaseInitTypeDef TIM_InitStruct;
        TIM_InitStruct.TIM_Prescaler = 8-1;
        TIM_InitStruct.TIM_Period = channelConfig->defaultPeriode - 1;
        TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

        TIM_TimeBaseInit(tim, &TIM_InitStruct);
        
        // Set the PWM mode and output state
        TIM_OCInitTypeDef TIM_OC;
        TIM_OC.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OC.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OC.TIM_Pulse = channelConfig->compareValue;
        if (channelConfig->polarity == PWM_HIGH) {
            TIM_OC.TIM_OCPolarity = TIM_OCPolarity_High;
        } else {
            TIM_OC.TIM_OCPolarity = TIM_OCPolarity_Low;
        }

        // Set the output compare mode for the specific channel
        switch (channelConfig->Channel % 4) {
            case 0: 
                TIM_OC1Init(tim, &TIM_OC);
                break;
            case 1: 
                TIM_OC2Init(tim, &TIM_OC);
                break;
            case 2: 
                TIM_OC3Init(tim, &TIM_OC);
                break;
            case 3: 
                TIM_OC4Init(tim, &TIM_OC);
                break;
            default:
                break; // Invalid channel
        }

        // Enable the timer
        TIM_Cmd(tim, ENABLE);
    }

}

/**
 * @brief De-initializes the PWM driver.
 */
void Pwm_DeInit(void) {
    if (Pwm_CurrentConfigPtr == NULL) {
        return; // No configuration to de-initialize
    }

    for (uint8 i = 0; i < Pwm_CurrentConfigPtr->numChannels; i++) {
        const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[i];
        TIM_TypeDef* tim = channelConfig->TIMx; // Get the TIM instance for the channel
        TIM_Cmd(tim, DISABLE); // Disable the TIM peripheral

        // Re-apply that “disabled” configuration to whichever channel it was
        TIM_OCInitTypeDef ocInit;
        TIM_OCStructInit(&ocInit);
        switch (channelConfig->Channel % 4) {
            case 0: 
                TIM_OC1Init(tim, &ocInit);
                break;
            case 1: 
                TIM_OC2Init(tim, &ocInit);
                break;
            case 2: 
                TIM_OC3Init(tim, &ocInit);
                break;
            case 3: 
                TIM_OC4Init(tim, &ocInit);
                break;
            default:
                break; // Invalid channel
        }

        // Reset the TIM peripheral
        if (tim == TIM1) {
            TIM_CtrlPWMOutputs(TIM1, DISABLE); // De-initialize the TIM peripheral
        }
    }

    Pwm_CurrentConfigPtr = NULL; // Clear the configuration pointer
}

/**
 * @brief Sets the duty cycle for a specific PWM channel.
 * @param ChannelNumber The PWM channel to set the duty cycle for.
 * @param DutyCycle The duty cycle value to set (0-100%).
 */
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle) {
    if (Pwm_CurrentConfigPtr == NULL || ChannelNumber >= Pwm_CurrentConfigPtr->numChannels) {
        return; // Invalid configuration or channel
    }

    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    TIM_TypeDef* tim = channelConfig->TIMx; // Get the TIM instance for the channel
    
    if (tim == NULL) {
        return; // Invalid channel
    }

    uint16 compareValue = (DutyCycle * channelConfig->defaultPeriode) >> 15;
    
    switch (channelConfig->Channel % 4) {
        case 0: 
            TIM_SetCompare1(tim, compareValue);
            break;
        case 1: 
            TIM_SetCompare2(tim, compareValue);
            break;
        case 2: 
            TIM_SetCompare3(tim, compareValue);
            break;
        case 3: 
            TIM_SetCompare4(tim, compareValue);
            break;
        default:
            break; // Invalid channel
    }
}

/**
 * @brief Sets the period and duty for a specific PWM channel.
 * @param ChannelNumber The PWM channel to set the period for.
 * @param Period The period value to set.
 * @param DutyCycle The duty cycle value to set (0-100%).
 */
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle) {
    if (Pwm_CurrentConfigPtr == NULL || ChannelNumber >= Pwm_CurrentConfigPtr->numChannels || Pwm_CurrentConfigPtr->Channels[ChannelNumber].classType == PWM_FIXED_PERIOD) {
        return; // Invalid configuration or channel
    }

    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    TIM_TypeDef* tim = channelConfig->TIMx; // Get the TIM instance for the channel
    
    if (tim == NULL) {
        return; // Invalid channel
    }

    // Update the period
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = 8 - 1;
    TIM_InitStruct.TIM_Period = Period - 1;
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(tim, &TIM_InitStruct);

    // Update the duty cycle
    uint16 compareValue = (DutyCycle * Period) >> 15;
    
    switch (channelConfig->Channel % 4) {
        case 0: 
            TIM_SetCompare1(tim, compareValue);
            break;
        case 1: 
            TIM_SetCompare2(tim, compareValue);
            break;
        case 2: 
            TIM_SetCompare3(tim, compareValue);
            break;
        case 3: 
            TIM_SetCompare4(tim, compareValue);
            break;
        default:
            break; // Invalid channel
    }
}

/**
 * @brief Service sets the PWM output to the configured Idle state
 * @param ChannelNumber The PWM channel to start.
 */
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber) {
    if (Pwm_CurrentConfigPtr == NULL || ChannelNumber >= Pwm_CurrentConfigPtr->numChannels) {
        return; // Invalid configuration or channel
    }
    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    TIM_TypeDef* tim = channelConfig->TIMx; // Get the TIM instance for the channel

    
    if (tim == NULL) {
        return; // Invalid channel
    }
    Pwm_ChannelType channel = channelConfig->Channel;
    if (channelConfig->idleState == PWM_HIGH) {
       switch (channel){
            case 0: 
                Dio_WriteChannel(8, STD_HIGH);
                break;
            case 1:
                Dio_WriteChannel(0, STD_HIGH);
                break;
            case 2:
                Dio_WriteChannel(6, STD_HIGH);
                break;
            case 3:
                Dio_WriteChannel(22, STD_HIGH);
                break;
            case 4:
                Dio_WriteChannel(9, STD_HIGH);
                break;
            case 5: 
                Dio_WriteChannel(1, STD_HIGH);
                break;
            case 6:
                Dio_WriteChannel(7, STD_HIGH);
                break;
            case 7:
                Dio_WriteChannel(23, STD_HIGH);
                break;
            case 8:
                Dio_WriteChannel(10, STD_HIGH);
                break;
            case 9: 
                Dio_WriteChannel(2, STD_HIGH);
                break;
            case 10:
                Dio_WriteChannel(16, STD_HIGH);
                break;
            case 11:
                Dio_WriteChannel(24, STD_HIGH);
                break;
            case 12:
                Dio_WriteChannel(11, STD_HIGH);
                break;
            case 13: 
                Dio_WriteChannel(3, STD_HIGH);
                break;
            case 14:
                Dio_WriteChannel(17, STD_HIGH);
                break;
            case 15:
                Dio_WriteChannel(25, STD_HIGH);
                break;
            default:
                break; // Invalid channel            
       } 
    } else {
        switch (channel) {
            case 0: 
                Dio_WriteChannel(8, STD_LOW);
                break;
            case 1:
                Dio_WriteChannel(0, STD_LOW);
                break;
            case 2:
                Dio_WriteChannel(6, STD_LOW);
                break;
            case 3:
                Dio_WriteChannel(22, STD_LOW);
                break;
            case 4:
                Dio_WriteChannel(9, STD_LOW);
                break;
            case 5: 
                Dio_WriteChannel(1, STD_LOW);
                break;
            case 6:
                Dio_WriteChannel(7, STD_LOW);
                break;
            case 7:
                Dio_WriteChannel(23, STD_LOW);
                break;
            case 8:
                Dio_WriteChannel(10, STD_LOW);
                break;
            case 9: 
                Dio_WriteChannel(2, STD_LOW);
                break;
            case 10:
                Dio_WriteChannel(16, STD_LOW);
                break;
            case 11:
                Dio_WriteChannel(24, STD_LOW);
                break;
            case 12:
                Dio_WriteChannel(11, STD_LOW);
                break;
            case 13: 
                Dio_WriteChannel(3, STD_LOW);
                break;
            case 14:
                Dio_WriteChannel(17, STD_LOW);
                break;
            case 15:
                Dio_WriteChannel(25, STD_LOW);
                break;
            default:
                break; // Invalid channel            
        }
    }
}

/**
 * @brief Service to read the internal state of the PWM output signal.
 * @param ChannelNumber The PWM channel to start.
 * @return The current output state of the PWM channel.
 */
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber) {
    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    TIM_TypeDef* tim = channelConfig->TIMx; // Get the TIM instance for the channel
    
    uint16_t isOutputEnabled = 0;
    switch (channelConfig->Channel % 4) {
        case 0: 
            isOutputEnabled = (tim.CCER & TIM_CCER_CC1E);
        case 1: 
            isOutputEnabled = (tim.CCER & TIM_CCER_CC2E);
        case 2: 
            isOutputEnabled = (tim.CCER & TIM_CCER_CC3E);
        case 3: 
            isOutputEnabled = (tim.CCER & TIM_CCER_CC4E);
        default:
            break; // Invalid channel
    }
    return (isOutputEnabled) ? PWM_HIGH : PWM_LOW;
}

/**
 * @brief Service to disable the PWM signal edge notification
 * @param ChannelNumber The PWM channel to disable notification for.
 */
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber) {
    (void)Nitification;
    const Pwm_ChannelConfigType* channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    TIM_TypeDef* tim = channelConfig->TIMx; // Get the TIM instance for the channel
    if (tim == NULL) {
        return; // Invalid channel
    }

    switch (channelConfig->Channel % 4) {
        case 0: 
            TIM_ITConfig(tim, TIM_IT_CC1, DISABLE);
            break;
        case 1: 
            TIM_ITConfig(tim, TIM_IT_CC2, DISABLE);
            break;
        case 2: 
            TIM_ITConfig(tim, TIM_IT_CC3, DISABLE);
            break;
        case 3: 
            TIM_ITConfig(tim, TIM_IT_CC4, DISABLE);
            break;
        default:
            break; // Invalid channel
    }
}

/**
 * @brief Service to enable the PWM signal edge notification
 * @param ChannelNumber The PWM channel to enable notification for.
 * @param Notification The type of edge notification to enable.
 */
void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType notification) {
    if (ChannelNumber >= PWM_NUM_CHANNELS) return;
    Pwm_ChannelConfigType *cfg = (Pwm_ChannelConfigType*)&PwmChannelsConfig[channelId];
    TIM_TypeDef *TIMx = cfg->TIMx;
    uint16_t cc_flag = TIM_IT_CC1 << (cfg->channel - 1);
    cfg->notificationEnable = 1;

    if (notification & PWM_RISING_EDGE)
        TIM_ITConfig(TIMx, cc_flag, ENABLE);
    if (notification & PWM_FALLING_EDGE)
        TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

    IRQn_Type irq = (TIMx == TIM2)? TIM2_IRQn :
                    (TIMx == TIM3)? TIM3_IRQn :
                    (TIMx == TIM4)? TIM4_IRQn : (IRQn_Type)0xFF;
    if (irq != (IRQn_Type)0xFF) {
        NVIC_InitTypeDef n;
        n.NVIC_IRQChannel = irq;
        n.NVIC_IRQChannelPreemptionPriority = 1;
        n.NVIC_IRQChannelSubPriority = 0;
        n.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&n);
    }

}

/**
 * @brief Service returns the version information of this module.
 */
void Pwm_GetVersionInfo(Std_VersionInfoType* versioninfo) {
    if (versioninfo == NULL) {
        return; // Invalid pointer
    }

    versioninfo->vendorID = PWM_VENDOR_ID;
    versioninfo->moduleID = PWM_MODULE_ID;
    versioninfo->sw_major_version = PWM_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = PWM_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = PWM_SW_PATCH_VERSION;
}



Pwm_ChannelConfigType Pwm_Channels[MAX_PWM_CHANNELS] = { 0 };