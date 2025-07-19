#include "Pwm_Cfg.h"   // prototype Pwm_IsrHandler
void TIM2_IRQHandler(void)
{
    Pwm_IsrHandler(TIM2);
}


