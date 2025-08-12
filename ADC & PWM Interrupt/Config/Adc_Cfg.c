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



