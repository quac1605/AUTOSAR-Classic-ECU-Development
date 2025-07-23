#ifndef ADC_CFG_H
#define ADC_CFG_H

#include "Adc.h"

typedef void (*Adc_NotificationCbType)(void);

// Cấu hình cho từng nhóm ADC
typedef struct {
    ADC_TypeDef*             ADCx;           // ADC instance (ADC1/ADC2)
    Adc_GroupType            groupId;        // Group numeric ID
    uint8_t                  channelCount;   // Số kênh trong group
    Adc_NotificationCbType   NotificationCb; // Callback khi conversion xong
} Adc_GroupConfigType;

// Cấu hình 1 group ADC ↔ 1 DMA channel
typedef struct {
    ADC_TypeDef*           ADCx;           // ADC instance (ADC1)
    DMA_Channel_TypeDef*   DMA_Channel;    // DMA channel (DMA1_Channel1)
    uint32_t               DMA_FLAG_TC;    // Flag Transfer Complete
    Adc_GroupType          groupId;        // ID nhóm ADC
    uint8_t                channelCount;   // Số kênh trong nhóm
    Adc_NotificationCbType NotificationCb; // Callback khi DMA TC
} Adc_GroupDmaConfigType;

void ADC1_2_IRQHandler(void);

extern const Adc_GroupConfigType AdcGroupConfig[];
extern const Adc_GroupDmaConfigType AdcGroupDmaConfig[];

#define ADC_NUM_GROUPS  (sizeof(AdcGroupConfig)/sizeof(AdcGroupConfig[0]))
#define ADC_NUM_GROUPS_DMA  (sizeof(AdcGroupDmaConfig)/sizeof(AdcGroupDmaConfig[0]))
#endif

