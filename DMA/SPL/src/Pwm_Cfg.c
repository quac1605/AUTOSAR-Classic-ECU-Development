#include "Pwm_Cfg.h"
#include "stm32f10x_adc.h"



void TIM2_IRQHandler(void)
{
    TIM_TypeDef *TIMx = TIM2; // Assuming TIM2 is used for PWM channels
    for (uint8_t i = 0; i < MAX_PWM_CHANNELS; ++i) {
        const Pwm_ChannelConfigType *cfg = &Pwm_CurrentConfigPtr->Channels[i];
        if (GetChannelTIM(cfg->Channel) == TIMx) {
            uint16_t flag = TIM_IT_CC1 << (cfg->Channel - 1);
            if (TIM_GetITStatus(TIMx, flag) != RESET) {
                TIM_ClearITPendingBit(TIMx, flag);
                if (cfg->NotificationEnable && cfg->NotificationCb)
                    cfg->NotificationCb(); // Call the notification callback if enabled
            }
            if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET) {
                TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
                if (cfg->NotificationEnable && cfg->NotificationCb)
                    cfg->NotificationCb();
            }
        }
    }
}
