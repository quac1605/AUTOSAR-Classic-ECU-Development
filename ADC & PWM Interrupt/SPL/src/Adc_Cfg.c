#include "Adc_Cfg.h"
#include "stm32f10x_adc.h"

void MyAdcGroup0_Notification(void)
{
    // Xử lý kết quả, ví dụ: toggle LED
}

void ADC1_2_IRQHandler(void)
{
    Adc_IsrHandler(ADC1);
    // Nếu có ADC2: Adc_IsrHandler(ADC2);
}

const Adc_GroupConfigType AdcGroupConfig[] = {
    {   
        .ADCx = ADC1, 
        .groupId = 0, 
        .channelCount = 2, 
        .NotificationCb = MyAdcGroup0_Notification },
    { 
        .ADCx = ADC1, 
        .groupId = 1, 
        .channelCount = 1, 
        .NotificationCb = NULL }
};