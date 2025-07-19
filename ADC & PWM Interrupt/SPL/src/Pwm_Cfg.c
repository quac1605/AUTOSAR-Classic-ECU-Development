#include "Pwm_Cfg.h"
#include "stm32f10x_adc.h"

void Pwm_Channel0_Notification(void){
	blinkLed(); //để test
}

void Pwm_IsrHandler(TIM_TypeDef *TIMx)
{
    for (uint8_t i = 0; i < PWM_NUM_CHANNELS; ++i) {
        const Pwm_ChannelConfigType *cfg = &PwmChannelsConfig[i];
        if (cfg->TIMx == TIMx) {
            uint16_t flag = TIM_IT_CC1 << (cfg->channel - 1);
            if (TIM_GetITStatus(TIMx, flag) != RESET) {
                TIM_ClearITPendingBit(TIMx, flag);
                if (cfg->notificationEnable && cfg->NotificationCb)
                    cfg->NotificationCb(); đây chính là con trỏ hàm, cụ thể hơn chính là thằng Pwm_Channel0_Notification()
            }
            if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET) {
                TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
                if (cfg->notificationEnable && cfg->NotificationCb)
                    cfg->NotificationCb();
            }
        }
    }
}
