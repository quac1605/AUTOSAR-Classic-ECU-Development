/**********************************************************
 * @file    Port.c
 * @brief   Port Driver Source File (AUTOSAR Standard)
 * @details This file implements the AUTOSAR standard APIs for the Port Driver,
 *          designed for STM32F103 using SPL. The driver supports configuration,
 *          mode switching, direction control, refreshing, and version retrieval
 *          for ports and pins.
 *
 * @version 1.0
 * @date    2024-06-19
 * @author  HALA Academy
 **********************************************************/

/* ===============================
 *           Includes
 * =============================== */
#include "Port.h"
#include "stm32f10x_rcc.h"
#include <stddef.h>

/* ===============================
 *     Static/Internal Variables
 * =============================== */
static uint8_t Port_Initialized = 0;  /* Status variable to check if Port is initialized */

/* ===============================
 *      Internal Helper Function
 * =============================== */

/**********************************************************
 * @brief Configure a GPIO pin based on AUTOSAR parameters
 * @param[in] pinCfg Pointer to pin configuration structure
 **********************************************************/
static void Port_ApplyPinConfig(const Port_PinConfigType* pinCfg) {
    GPIO_InitTypeDef GPIO_InitStruct;
    uint16_t pinMask = PORT_GET_PIN_MASK(pinCfg->PinNum);

    /* Enable clock for the PORT */
    switch (pinCfg->PortNum) {
        case PORT_ID_A: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); break;
        case PORT_ID_B: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); break;
        case PORT_ID_C: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); break;
        case PORT_ID_D: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); break;
        default: return;
    }

    GPIO_InitStruct.GPIO_Pin = pinMask;
    switch (pinCfg->speed) {
        case 2: GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; break;
        case 10: GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz; break;
        case 50: GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; break;
        default: GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; break;
    }

     /* Handle all supported modes */
    switch (pinCfg->Mode) {
        case PORT_PIN_MODE_DIO:
            if (pinCfg->Direction == PORT_PIN_OUT) {
                GPIO_InitStruct.GPIO_Mode = (pinCfg->Pull == PORT_PIN_PULL_UP) ? GPIO_Mode_Out_PP : GPIO_Mode_Out_OD;
            } else {
                if (pinCfg->Pull == PORT_PIN_PULL_UP)
                    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
                else if (pinCfg->Pull == PORT_PIN_PULL_DOWN)
                    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
                else
                    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            }
            break;

        case PORT_PIN_MODE_ADC:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
            break;

        case PORT_PIN_MODE_PWM:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            break;

        case PORT_PIN_MODE_SPI:
            GPIO_InitStruct.GPIO_Mode = (pinCfg->Direction == PORT_PIN_OUT) ? GPIO_Mode_AF_PP : GPIO_Mode_IN_FLOATING;
            break;

        case PORT_PIN_MODE_CAN:
            if (pinCfg->Direction == PORT_PIN_OUT)
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            else
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // RX line typically pulled up
            break;

        case PORT_PIN_MODE_LIN:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // LIN typically uses AF_PP for TX
            break;

        default:
            return;  // Unsupported mode
    }

    /* Initialize the pin */
    GPIO_Init(PORT_GET_PORT(pinCfg->PortNum), &GPIO_InitStruct);

    /* If output, set initial level */
    if (pinCfg->Direction == PORT_PIN_OUT) {
        if (pinCfg->Level == PORT_PIN_LEVEL_HIGH)
            GPIO_SetBits(PORT_GET_PORT(pinCfg->PortNum), pinMask);
        else
            GPIO_ResetBits(PORT_GET_PORT(pinCfg->PortNum), pinMask);
    }
}

/* ===============================
 *     Function Definitions
 * =============================== */

/**********************************************************
 * @brief Initialize all Ports/Pins based on configuration
 * @details This function configures each pin according to the config table.
 * @param[in] ConfigPtr Pointer to Port configuration
 **********************************************************/
void Port_Init(const Port_ConfigType* ConfigPtr) {
    if (ConfigPtr == NULL) return;

    for (uint16_t i = 0; i < ConfigPtr->PinCount; i++) {
        Port_ApplyPinConfig(&ConfigPtr->PinConfigs[i]);
    }
    Port_Initialized = 1;
}

/**********************************************************
 * @brief Change the direction of a Port pin (if runtime allowed)
 * @details
 * This function will change the pin direction (IN/OUT), if allowed in config.
 * @param[in] Pin Pin number (0..n-1)
 * @param[in] Direction Desired direction
 **********************************************************/
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction) {
    if (!Port_Initialized) return;
    if (Pin >= PIN_COUNT) return;
    if (!PortCfg_Pins[Pin].DirectionChangeable) return;

    Port_PinConfigType* pinCfg = (Port_PinConfigType*)&PortCfg_Pins[Pin]; // cast away const for runtime update (if needed)
    pinCfg->Direction = Direction;
    Port_ApplyPinConfig(pinCfg);
}

/**********************************************************
 * @brief Refresh the direction of pins that are not runtime-changeable
 * @details Only pins configured with DirectionChangeable = 0 will be refreshed
 *          to their configured direction values
 **********************************************************/
void Port_RefreshPortDirection(void) {
    if (!Port_Initialized) return;
    for (uint16_t i = 0; i < PIN_COUNT; i++) {
        if (!PortCfg_Pins[i].DirectionChangeable) {
            Port_ApplyPinConfig(&PortCfg_Pins[i]);
        }
    }
}

/**********************************************************
 * @brief Retrieve version information of the Port Driver
 * @param[out] versioninfo Pointer to Std_VersionInfoType structure to receive version info
 **********************************************************/
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo) {
    if (versioninfo == NULL) return;
    versioninfo->vendorID = PORT_VENDOR_ID;
    versioninfo->moduleID = PORT_MODULE_ID;
    versioninfo->sw_major_version = PORT_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = PORT_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = PORT_SW_PATCH_VERSION;
}

/**********************************************************
 * @brief Change the functional mode of a pin (if runtime allowed)
 * @param[in] Pin Pin number
 * @param[in] Mode Functional mode to switch to
 **********************************************************/
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode) {
    if (!Port_Initialized) return;
    if (Pin >= PIN_COUNT) return;
    if (!PortCfg_Pins[Pin].ModeChangeable) return;

    Port_PinConfigType* pinCfg = (Port_PinConfigType*)&PortCfg_Pins[Pin];
    pinCfg->Mode = Mode;
    Port_ApplyPinConfig(pinCfg);
}
