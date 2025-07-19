#ifndef PWM_CFG_H
#define PWM_CFG_H

#include "Pwm.h"

// --- Callback ứng dụng ---
void Pwm_Channel0_Notification(void);
void Pwm_IsrHandler(TIM_TypeDef *TIMx);

// --- Bảng cấu hình PWM ---
const Pwm_ChannelConfigType PwmChannelsConfig[] = {
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
        .channel            = 2,
        .notificationEnable = 0,
        .NotificationCb     = NULL
    }
};
#define PWM_NUM_CHANNELS  (sizeof(PwmChannelsConfig)/sizeof(PwmChannelsConfig[0]))

#endif // PWM_CFG_H

