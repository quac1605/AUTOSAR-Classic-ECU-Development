/**
 * @file    Adc.h
 * @brief   AUTOSAR ADC Driver Header File for STM32F103C8
 * @version 1.0
 * @date    2025
 */

#ifndef ADC_H
#define ADC_H

#define MAX_ADC_GROUPS 4 // Maximum number of ADC groups supported

#include "Std_Types.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"
#include "Port.h"
/** Logical identifier for an ADC channel */
typedef uint8 Adc_ChannelType;

/** Logical identifier for an ADC group */
typedef uint8 Adc_GroupType;

/** Type for holding ADC conversion result value */
typedef uint8 Adc_ValueGroupType;

/** ADC prescaler */
typedef uint8 Adc_PrescaleType;

/** ADC conversion time */
typedef uint8 Adc_ConversionTimeType;

/** Sampling time in ADC clock cycles */
typedef uint8 Adc_SamplingTimeType;

/** ADC resolution */
typedef uint8 Adc_ResolutionType;

/**
 * @brief ADC Status Type 
 */
typedef enum {
    ADC_IDLE                = 0x00,
    ADC_BUSY                = 0x01,
    ADC_COMPLETED           = 0x02,
    ADC_STREAM_COMPLETED    = 0x03
} Adc_StatusType;

/**
 * @brief Trigger source type
 */
typedef enum {
    ADC_TRIGG_SRC_SW = 0x00,
    ADC_TRIGG_SRC_HW = 0x01
} Adc_TriggerSourceType;

/**
 * @brief Conversion mode of an ADC Channel group
 */
typedef enum {
    ADC_CONV_MODE_ONESHOT = 0x00,   /**< One-shot conversion mode */
    ADC_CONV_MODE_CONTINUOUS = 0x01 /**< Continuous conversion mode */
} Adc_GroupConvModeType;

/** ADC group priority */
typedef uint8 Adc_GroupPriorityType;

/** ADC channels in group */
typedef struct{
    Adc_GroupType GroupId; /**< Logical identifier for the ADC group */
    Adc_ChannelType Channels[16]; /**< Array of channels in the group, max 16 channels */
    Adc_GroupPriorityType Priority; /**< Priority of the group */
    uint8 numChannels; /**< Number of channels in the group */
    Adc_StatusType Status; /**< Current status of the group */
    Adc_ValueGroupType *Result; /**< Group transmit result */
} Adc_GroupDefType;

/** Number of group conversions in streaming access mode */
typedef uint8 Adc_StreamNumSampleType;

/**
 * @brief ADC buffer handling mode
 */
typedef enum {
    ADC_STREAM_BUFFER_LINEAR = 0x00,
    ADC_STREAM_BUFFER_CIRCULAR = 0x01
} Adc_StreamBufferModeType;

/** ADC access mode: polling, interrupt, DMA */
typedef enum {
    ADC_ACCESS_MODE_SINGLE = 0x00,
    ADC_ACCESS_MODE_STREAMING = 0x01,
} Adc_GroupAccessModeType;

/**
 * @brief Hardware trigger signal edge type
 */
typedef enum {
    ADC_HW_TRIG_RISING_EDGE = 0x00,
    ADC_HW_TRIG_FALLING_EDGE = 0x01,
    ADC_HW_TRIG_BOTH_EDGES = 0x02
} Adc_HwTriggerSignalType;

/** Hardware timer trigger type */
typedef uint8 Adc_HwTriggerTimerType;

/**
 * @brief ADC prioritization mechanism
 */
typedef enum {
    ADC_PRIORITY_NONE = 0x00,  /**< No priority available*/
    ADC_PRIORITY_HW = 0x01,   /**< Hardware priority available*/
    ADC_PRIORITY_HW_SW = 0x02    /**< Hardware and Software priority available*/
}Adc_PriorityImplementationType;

/** 
 * @brief Group replacement strategy 
 */
typedef enum {
    ADC_GROUP_REPL_ABORT_RESTART = 0x00,
    ADC_GROUP_REPL_SUSPEND_RESUME = 0x01
}Adc_GroupReplacementType;

typedef enum {
    ADC_RANGE_UNDER_LOW = 0x00, /**< Value is below the lower limit */
    ADC_RANGE_BETWEEN = 0x01, /**< Value is between the lower and upper limits */
    ADC_RANGE_OVER_HIGH = 0x02, /**< Value is above the upper limit */
    ADC_RANGE_ALWAYS = 0x03, /**< Complete range - independent from channel limit settings*/
    ADC_RANGE_NOT_UNDER_LOW = 0x04, /**< Value is not below the lower limit */
    ADC_RANGE_NOT_BETWEEN = 0x05, /**< Value is not between the lower and upper limits */
    ADC_RANGE_NOT_OVER_HIGH = 0x06 /**< Value is not above the upper limit */
} Adc_ChannelRangeSelectType;

/**
 * @brief ADC result alignment
 */
typedef enum {
    ADC_ALIGN_RIGHT = 0x00,
    ADC_ALIGN_LEFT = 0x01
} Adc_ResultAlignmentType;

/**
 * @brief Result of the requests related to power state transitions.
 */
typedef enum {
    ADC_SERVICE_ACCEPTED= 0x00, 
    ADC_NOT_INIT = 0x01, 
    ADC_SEQUENCE_ERROR = 0x02,
    ADC_HW_FAILURE = 0x03,
    ADC_POWER_STATE_NOT_SUPP = 0x04,
    ADC_TRANS_NOT_POSSIBLE = 0x05,
} Adc_PowerStateRequestResultType;

typedef enum {
    ADC_NOTIFICATION_OFF = 0x00, /**< No notification */
    ADC_NOTIFICATION_ON = 0x01, /**< Notification enabled */
} Adc_NotificationType;

/**
 * @brief ADC instance type
 */
typedef enum {
    ADC_1 = 0x00, /**< ADC Instance 1 */
    ADC_2 = 0x01 /**< ADC Instance 2 */
} Adc_InstanceType;

typedef void (*Adc_NotificationCbType)(void);


/**
 * @brief Configuration structure for a single ADC group
 */
typedef struct {
    Adc_GroupConvModeType ConvMode; /**< Conversion mode of the group */
    Adc_TriggerSourceType TriggerSource; /**< Trigger source for the group */
    Adc_NotificationType NotificatioEnabe; /**< Notification enable flag when transmit process finish */
    uint8 numChannels; /**< Number of channels in the group */
    Adc_InstanceType Instance; /**< ADC instance for the group */
    Adc_ResultAlignmentType ResultAlignment; /**< Result alignment for the group */
    /**
     * @typedef Adc_ChannelConfigType
     * @brief Configuration structure for each ADC channel
     */
    struct {
        Adc_ChannelType ChannelId; /**< Logical identifier for the ADC channel */
        Adc_SamplingTimeType SamplingTime; /**< Sampling time for the channel */
        uint8 Rank; /**< Prority of the channel in the group */    
    } Channel[16]; // Maximum 16 channels per group
} Adc_ConfigType;

/* =============================== */
/*           API Prototypes        */
/* =============================== */

void Adc_Init(const Adc_ConfigType* ConfigPtr);
void Adc_DeInit(void);

Std_ReturnType Adc_SetupResultBuffer ( Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr);

void Adc_StartGroupConversion(Adc_GroupType Group);
void Adc_StopGroupConversion(Adc_GroupType Group);
void Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr);

void Adc_EnableHardwareTrigger (Adc_GroupType Group);
void Adc_DisableHardwareTrigger (Adc_GroupType Group);

void Adc_EnableGroupNotification(Adc_GroupType Group);
void Adc_DisableGroupNotification(Adc_GroupType Group);

Adc_StatusType Adc_GetGroupStatus (Adc_GroupType Group);

Adc_StreamNumSampleType Adc_GetStreamLastPointer (Adc_GroupType Group,Adc_ValueGroupType** PtrToSamplePtr);

void Adc_GetVersionInfo (Std_VersionInfoType* versioninfo);

void DMA1_Channel1_IRQHandler(void);
void ADC1_2_IRQHandler(void);

void Port_ConfigAdcPin(uint8 portNum, uint8 pinNum);
#endif /* ADC_H */