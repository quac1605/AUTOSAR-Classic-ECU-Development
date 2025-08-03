#include "Adc_Cfg.h"
#include "stm32f10x_adc.h"



void ADC1_2_IRQHandler(void)
{
    for (uint8 group = 0;group < MAX_ADC_GROUPS; group++ ){
        Adc_GroupDefType* groupDef = &Adc_Groups[group];
        Adc_ConfigType* config = &Adc_Configs[groupDef->AdcInstance];
        if (config->NotificationEnable == ADC_NOTIFICATION_ON) {
            if (groupDef->AdcInstance == ADC_1 && ADC_GetITStatus(ADC1, ADC_IT_EOC)) {
                ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
                if (config->Adc_NotificationCbType) {
                    config->Adc_NotificationCbType();
                }
            }
            else if (groupDef->AdcInstance == ADC_2 && ADC_GetITStatus(ADC2, ADC_IT_EOC)) {
                ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);
                if (config->Adc_NotificationCbType) {
                    config->Adc_NotificationCbType();
                }
            }
        }
    }
}

void DMA1_Channel1_IRQHandler(void)
{
    for (int i = 0; i < ADC_NUM_GROUPS_DMA; i++) {
        const Adc_GroupDmaConfigType* cfg = &AdcGroupDmaConfig[i];
        if (cfg->DMA_Channel != DMA1_Channel1) continue;
        if (DMA_GetITStatus(cfg->DMA_FLAG_TC)) {
            DMA_ClearITPendingBit(cfg->DMA_FLAG_TC);
            DMA_Cmd(cfg->DMA_Channel, DISABLE);
            ADC_DMACmd(cfg->ADCx, DISABLE);
            if (cfg->Adc_DmaNotificationCbType) cfg->Adc_DmaNotificationCbType();
        }
    }
}




