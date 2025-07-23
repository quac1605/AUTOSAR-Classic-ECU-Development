/**
 * @file    Pwm.h
 * @brief   AUTOSAR ADC Driver Header File for STM32F103C8
 * @version 1.0
 * @date    2025
 */

#ifndef PWM_H
#define PWM_H

#define MAX_PWM_CHANNELS 16 // Maximum number of PWM channels supported

#include "Std_Types.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "Port.h"
#include "Dio.h"

typedef void (*Pwm_NotificationCbType)(void);

/** Logical identifier for an PWM channel 0 -> 15 */
typedef uint8 Pwm_ChannelType;

/** Definition of the period of a PWM channel  */
typedef uint16 Pwm_PeriodType;

/** Output state of a PWM channel. */
typedef enum {
    PWM_HIGH = 0x00,                    /**< The PWM channel is in high state. */
    PWM_LOW = 0x01,                     /**< The PWM channel is in low state */
} Pwm_OutputStateType;

/** Definition of the type of edge notification of a PWM channel */
typedef enum {
    PWM_RISING_EDGE = 0x00,             /**< Notification will be called when a rising edge occurs on the PWM output signal. */
    PWM_FALLING_EDGE = 0x01,            /**< Notification will be called when a falling edge occurs on the PWM output signal. */
    PWM_BOTH_EDGES = 0x02,              /**< Notification will be called when a rising or falling edge occurs on the PWM output signal. */
} Pwm_EdgeNotificationType;

/** Defines the class of a PWM channel */
typedef enum {
    PWM_VARIABLE_PERIOD = 0x00,         /**< The PWM channel has a variable period. The duty cycle and the period can be changed. */
    PWM_FIXED_PERIOD = 0x01,            /**< The PWM channel has a fixed period. Only the duty cycle can be changed. */
} Pwm_ChannelClassType;

/** 
 * @brief Configuration structure for a PWM channel
 */
typedef struct {
    TIM_TypeDef* *TIMx; /**< Function to get the TIM instance for the channel */
    Pwm_ChannelType Channel;            /**< Channel number (0-15) */
    Pwm_ChannelClassType classType;     /**< Class of the PWM channel */
    Pwm_PeriodType defaultPeriode;      /**< Period of the PWM channel */
    uint16 compareValue;                /**< Compare value for the PWM channel */
    Pwm_OutputStateType polarity;       /**< Polarity of the PWM channel output */
    Pwm_OutputStateType idleState;      /**< Idle state of the PWM channel output */
    uint8 notificationEnable;           /**< Enable notification for the PWM channel */
    Pwm_NotificationCbType NotificationCb;
} Pwm_ChannelConfigType;


/** 
 * @brief This is the type of data structure containing the initialization data for the PWM driver
 */
typedef struct {
    const Pwm_ChannelConfigType* Channels;      /**< Pointer to the array of channel configurations */
    uint8 numChannels;                          /**< Number of PWM channels configured */
} Pwm_ConfigType;

/**
 * @brief Initializes the PWM driver with the given configuration.
 * @param ConfigPtr Pointer to the configuration structure.
*/
void Pwm_Init(const Pwm_ConfigType* ConfigPtr);

/**
 * @brief De-initializes the PWM driver.
 */
void Pwm_DeInit(void);

/**
 * @brief Sets the duty cycle for a specific PWM channel.
 * @param ChannelNumber The PWM channel to set the duty cycle for.
 * @param DutyCycle The duty cycle value to set (0-100%).
 */
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle);

/**
 * @brief Sets the period and duty for a specific PWM channel.
 * @param ChannelNumber The PWM channel to set the period for.
 * @param Period The period value to set.
 * @param DutyCycle The duty cycle value to set (0-100%).
 */
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle);

/**
 * @brief Service sets the PWM output to the configured Idle state
 * @param ChannelNumber The PWM channel to start.
 */
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber);

/**
 * @brief Service to read the internal state of the PWM output signal.
 * @param ChannelNumber The PWM channel to start.
 */
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber);

/**
 * @brief Service to disable the PWM signal edge notification
 * @param ChannelNumber The PWM channel to disable notification for.
 */
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber);

/**
 * @brief Service to enable the PWM signal edge notification
 * @param ChannelNumber The PWM channel to enable notification for.
 * @param Notification The type of edge notification to enable.
 */
void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification);

/**
 * @brief Service returns the version information of this module.
 */
void Pwm_GetVersionInfo(Std_VersionInfoType* versioninfo);

extern const Pwm_ChannelConfigType PwmChannelsConfig[];
#define PWM_NUM_CHANNELS  (sizeof(PwmChannelsConfig)/sizeof(PwmChannelsConfig[0]))
#endif /* PWM_H */