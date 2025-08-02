#ifndef PWM_CFG_H
#define PWM_CFG_H

#include "Pwm.h"

uint8_t Pwm_GetNumChannels(void);

// --- Callback ứng dụng ---
void Pwm_Channel0_Notification(void);
void Pwm_IsrHandler(TIM_TypeDef *TIMx);

extern const Pwm_ChannelConfigType PwmChannelsConfig[];

#endif // PWM_CFG_H

