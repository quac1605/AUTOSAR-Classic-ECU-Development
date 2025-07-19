#ifndef ADC_CFG_H
#define ADC_CFG_H

#include "Adc.h"

typedef void (*Adc_NotificationCbType)(void);

typedef struct {
    ADC_TypeDef*             ADCx;           // ADC instance (ADC1/ADC2)
    Adc_GroupType            groupId;        // Group numeric ID
    uint8_t                  channelCount;   // Số kênh trong group
    Adc_NotificationCbType   NotificationCb; // Callback khi conversion xong
} Adc_GroupConfigType;

void ADC1_2_IRQHandler(void);

extern const Adc_GroupConfigType AdcGroupConfig[];
#define ADC_NUM_GROUPS  (sizeof(AdcGroupConfig)/sizeof(AdcGroupConfig[0]))

#endif

