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
#include "misc.h" 

#define PWM_VENDOR_ID         1234
#define PWM_MODULE_ID         5678
#define PWM_SW_MAJOR_VERSION  1
#define PWM_SW_MINOR_VERSION  0
#define PWM_SW_PATCH_VERSION  0

// Servo pulse specs from the SG90 datasheet:
#define SERVO_MIN_PULSE_US   600u    /**<  0° →  0.6 ms  */
#define SERVO_MAX_PULSE_US  2400u    /**< 180° →  2.4 ms  */
#define SERVO_CENTER_PULSE_US 1500u  /**<  90° →  1.5 ms  */



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
    PWM_RISING_EDGE = 0x01,             /**< Notification will be called when a rising edge occurs on the PWM output signal. */
    PWM_FALLING_EDGE = 0x02,            /**< Notification will be called when a falling edge occurs on the PWM output signal. */
    PWM_BOTH_EDGES = PWM_RISING_EDGE | PWM_FALLING_EDGE,              /**< Notification will be called when a rising or falling edge occurs on the PWM output signal. */
} Pwm_EdgeNotificationType;

/** Defines the class of a PWM channel */
typedef enum {
    PWM_VARIABLE_PERIOD = 0x00,         /**< The PWM channel has a variable period. The duty cycle and the period can be changed. */
    PWM_FIXED_PERIOD = 0x01,            /**< The PWM channel has a fixed period. Only the duty cycle can be changed. */
} Pwm_ChannelClassType;

typedef enum {
    PWM_NOTIFICATION_OFF = 0x00, /**< No notification */
    PWM_NOTIFICATION_ON = 0x01, /**< Notification enabled */
} Pwm_NotificationType;

/** 
 * @brief Configuration structure for a PWM channel
 */
typedef struct {
    Pwm_ChannelType Channel;                /**< Channel number (0-15) */
    Pwm_ChannelClassType classType;         /**< Class of the PWM channel */
    Pwm_PeriodType defaultPeriode;          /**< Period of the PWM channel */
    uint16 compareValue;                    /**< Compare value for the PWM channel */
    Pwm_OutputStateType polarity;           /**< Polarity of the PWM channel output */
    Pwm_OutputStateType idleState;          /**< Idle state of the PWM channel output */
    Pwm_NotificationType NotificationEnable;/**< Enable notification for the PWM channel */
    void (*NotificationCb)(void);           /**< Callback to the notification function */
} Pwm_ChannelConfigType;


/** 
 * @brief This is the type of data structure containing the initialization data for the PWM driver
 */
typedef struct {
    Pwm_ChannelConfigType* Channels;      /**< Pointer to the array of channel configurations */
    uint8 numChannels;                          /**< Number of PWM channels configured */
} Pwm_ConfigType;

extern const Pwm_ConfigType* Pwm_CurrentConfigPtr;
TIM_TypeDef* GetChannelTIM(Pwm_ChannelType ch);

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


#endif /* PWM_H */