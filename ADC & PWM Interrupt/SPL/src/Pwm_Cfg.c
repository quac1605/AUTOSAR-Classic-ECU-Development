#include "Pwm_Cfg.h"
#include "stm32f10x_adc.h"

#define PWM_NUM_CHANNELS  2

uint8_t Pwm_GetNumChannels(void) {
    return PWM_NUM_CHANNELS;
}

void Pwm_Channel0_Notification(void){
	GPIOC->ODR ^= GPIO_Pin_13;
}

// --- Bảng cấu hình PWM ---
const Pwm_ChannelConfigType PwmChannelsConfig[PWM_NUM_CHANNELS] = {
    // Channel 0: TIM2_CH1 (PA0), có callback
    {
        .TIMx               = TIM2,
        .Channel            = 1,
        .notificationEnable = 0,
        .NotificationCb     = Pwm_Channel0_Notification
    },
    // Channel 1: TIM3_CH2 (PA7), không dùng callback
    {
        .TIMx               = TIM3,
        .Channel            = 2,
        .notificationEnable = 0,
        .NotificationCb     = NULL
    }
};


void Pwm_IsrHandler(TIM_TypeDef *TIMx)
{
    for (uint8_t i = 0; i < PWM_NUM_CHANNELS; ++i) {
        const Pwm_ChannelConfigType *cfg = &PwmChannelsConfig[i];
        if (cfg->TIMx == TIMx) {
            uint16_t flag = TIM_IT_CC1 << (cfg->Channel - 1);
            if (TIM_GetITStatus(TIMx, flag) != RESET) {
                TIM_ClearITPendingBit(TIMx, flag);
                if (cfg->notificationEnable && cfg->NotificationCb)
                    cfg->NotificationCb(); // Call the notification callback if enabled
            }
            if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET) {
                TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
                if (cfg->notificationEnable && cfg->NotificationCb)
                    cfg->NotificationCb();
            }
        }
    }
}
