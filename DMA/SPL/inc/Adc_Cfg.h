#ifndef ADC_CFG_H
#define ADC_CFG_H

#include "Adc.h"

extern const Adc_GroupDmaConfigType AdcGroupDmaConfig[];
#define ADC_NUM_GROUPS_DMA  2


void ADC1_2_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void);

#endif

