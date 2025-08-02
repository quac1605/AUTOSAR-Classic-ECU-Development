#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "system_stm32f10x.h"
#include "Port.h"
#include "Pwm.h"

/* Port (PA0) */
Port_PinConfigType PortCfg_Pins[PIN_COUNT] = {
    {
      .PortNum           = PORT_ID_A,
      .PinNum            = 0,
      .Mode              = PORT_PIN_MODE_PWM,
      .Direction         = PORT_PIN_OUT,
      .speed             = 50,
      .DirectionChangeable = 0,
      .Level             = PORT_PIN_LEVEL_LOW,
      .Pull              = PORT_PIN_PULL_NONE,
      .ModeChangeable    = 0
    }
};
const Port_ConfigType PortCfg = {
    .PinConfigs = PortCfg_Pins,
    .PinCount   = 1
};

/* PWM channel 0 → TIM2_CH1 (PA0) */
Pwm_ChannelConfigType Pwm_Channels[] = {
    {
      .Channel        = 4,                /* 4→TIM2_CH1 */
      .classType      = PWM_FIXED_PERIOD,
      .defaultPeriode = 20000,            /* Counts @1 MHz →20 ms */
      .compareValue   = SERVO_CENTER_PULSE_US,
      .polarity       = PWM_HIGH,
      .idleState      = PWM_HIGH,
      .Notification   = NULL
    }
};
const Pwm_ConfigType PwmCfg = {
    .Channels    = Pwm_Channels,
    .numChannels = 1
};

static volatile uint32_t msTicks;
void SysTick_Handler(void) { msTicks++; }
static void Delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms) { }
}

int main(void) {
    SystemInit();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    /* bring up port & PWM driver */
    Port_Init(&PortCfg);
    Pwm_Init(&PwmCfg);

    /* let servo center */
    Delay_ms(500);

    while (1) {
        /* extreme “0” */
        Pwm_SetDutyCycle(0, 0);
        Delay_ms(500);

        /* extreme “100” */
        Pwm_SetDutyCycle(0, 100);
        Delay_ms(500);
    }
}
