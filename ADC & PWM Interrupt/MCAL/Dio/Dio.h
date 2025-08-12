/**
 * @file    Dio.h
 * @brief   AUTOSAR Digital Input/Output (DIO) Driver Header File
 * @version 4.4.0
 * @date    2024
 * 
 * This file contains the AUTOSAR DIO driver interface according to 
 * AUTOSAR specification. The DIO driver provides services for reading
 * and writing digital values to/from GPIO pins, channels, ports and 
 * channel groups.
 * 
 * @note    This file implements AUTOSAR DIO SWS specification
 */

#ifndef DIO_H
#define DIO_H

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "Std_Types.h"
#include "stm32f10x_gpio.h"
#include <stddef.h>

/*******************************************************************************
 * Indentify Port ID for GPIO Pins
 ******************************************************************************/

#define DIO_PORT_A 0
#define DIO_PORT_B 1
#define DIO_PORT_C 2
#define DIO_PORT_D 3

/*******************************************************************************
 * Macro indentification for GPIO ports and GIO pins based on Channel ID
 ******************************************************************************/

/* Macro indentification for GPIO ports  based on Channel ID*/ 

#define DIO_GET_PORT(ChannelId) \
    ((ChannelId) < 16 ? DIO_PORT_A : \
     (ChannelId) < 32 ? DIO_PORT_B : \
     (ChannelId) < 48 ? DIO_PORT_C : \
     (ChannelId) < 64 ? DIO_PORT_D : \
     0xFF) /* Invalid Port ID */

/* Macro indentification for GPIO pins based on Channel ID */
#define DIO_GET_PIN(ChannelId) \
(1 << ((ChannelId) % 16)) 

/*******************************************************************************
 * Macro indentification of Channel ID based on GPIO Port and Pin
 * @param[in] GPIO_Port Port ID of the GPIO port
 * @param[in] GPIO_Pin Pin number of the GPIO pin
 ******************************************************************************/
#define DIO_CHANNEL(GPIO_Port, GPIO_Pin) \
    (((GPIO_Port) << 4) + (GPIO_Pin))

/* Channel for all the GPIOA Pin*/

#define DIO_CHANNEL_A0 DIO_CHANNEL(DIO_PORT_A, 0)
#define DIO_CHANNEL_A1 DIO_CHANNEL(DIO_PORT_A, 1)
#define DIO_CHANNEL_A2 DIO_CHANNEL(DIO_PORT_A, 2)
#define DIO_CHANNEL_A3 DIO_CHANNEL(DIO_PORT_A, 3)
#define DIO_CHANNEL_A4 DIO_CHANNEL(DIO_PORT_A, 4)
#define DIO_CHANNEL_A5 DIO_CHANNEL(DIO_PORT_A, 5)
#define DIO_CHANNEL_A6 DIO_CHANNEL(DIO_PORT_A, 6)
#define DIO_CHANNEL_A7 DIO_CHANNEL(DIO_PORT_A, 7)
#define DIO_CHANNEL_A8 DIO_CHANNEL(DIO_PORT_A, 8)
#define DIO_CHANNEL_A9 DIO_CHANNEL(DIO_PORT_A, 9)
#define DIO_CHANNEL_A10 DIO_CHANNEL(DIO_PORT_A, 10)
#define DIO_CHANNEL_A11 DIO_CHANNEL(DIO_PORT_A, 11)
#define DIO_CHANNEL_A12 DIO_CHANNEL(DIO_PORT_A, 12)
#define DIO_CHANNEL_A13 DIO_CHANNEL(DIO_PORT_A, 13)
#define DIO_CHANNEL_A14 DIO_CHANNEL(DIO_PORT_A, 14)
#define DIO_CHANNEL_A15 DIO_CHANNEL(DIO_PORT_A, 15)

/* Channel for all the GPIOB Pin*/
#define DIO_CHANNEL_B0 DIO_CHANNEL(DIO_PORT_B, 0)
#define DIO_CHANNEL_B1 DIO_CHANNEL(DIO_PORT_B, 1)
#define DIO_CHANNEL_B2 DIO_CHANNEL(DIO_PORT_B, 2)
#define DIO_CHANNEL_B3 DIO_CHANNEL(DIO_PORT_B, 3)
#define DIO_CHANNEL_B4 DIO_CHANNEL(DIO_PORT_B, 4)
#define DIO_CHANNEL_B5 DIO_CHANNEL(DIO_PORT_B, 5)
#define DIO_CHANNEL_B6 DIO_CHANNEL(DIO_PORT_B, 6)
#define DIO_CHANNEL_B7 DIO_CHANNEL(DIO_PORT_B, 7)
#define DIO_CHANNEL_B8 DIO_CHANNEL(DIO_PORT_B, 8)
#define DIO_CHANNEL_B9 DIO_CHANNEL(DIO_PORT_B, 9)
#define DIO_CHANNEL_B10 DIO_CHANNEL(DIO_PORT_B, 10)
#define DIO_CHANNEL_B11 DIO_CHANNEL(DIO_PORT_B, 11)
#define DIO_CHANNEL_B12 DIO_CHANNEL(DIO_PORT_B, 12)
#define DIO_CHANNEL_B13 DIO_CHANNEL(DIO_PORT_B, 13)
#define DIO_CHANNEL_B14 DIO_CHANNEL(DIO_PORT_B, 14)
#define DIO_CHANNEL_B15 DIO_CHANNEL(DIO_PORT_B, 15)

/* Channel for all the GPIOC Pin*/
#define DIO_CHANNEL_C0 DIO_CHANNEL(DIO_PORT_C, 0)
#define DIO_CHANNEL_C1 DIO_CHANNEL(DIO_PORT_C, 1)
#define DIO_CHANNEL_C2 DIO_CHANNEL(DIO_PORT_C, 2)
#define DIO_CHANNEL_C3 DIO_CHANNEL(DIO_PORT_C, 3)
#define DIO_CHANNEL_C4 DIO_CHANNEL(DIO_PORT_C, 4)
#define DIO_CHANNEL_C5 DIO_CHANNEL(DIO_PORT_C, 5)
#define DIO_CHANNEL_C6 DIO_CHANNEL(DIO_PORT_C, 6)
#define DIO_CHANNEL_C7 DIO_CHANNEL(DIO_PORT_C, 7)
#define DIO_CHANNEL_C8 DIO_CHANNEL(DIO_PORT_C, 8)
#define DIO_CHANNEL_C9 DIO_CHANNEL(DIO_PORT_C, 9)
#define DIO_CHANNEL_C10 DIO_CHANNEL(DIO_PORT_C, 10)
#define DIO_CHANNEL_C11 DIO_CHANNEL(DIO_PORT_C, 11)
#define DIO_CHANNEL_C12 DIO_CHANNEL(DIO_PORT_C, 12)
#define DIO_CHANNEL_C13 DIO_CHANNEL(DIO_PORT_C, 13)
#define DIO_CHANNEL_C14 DIO_CHANNEL(DIO_PORT_C, 14)
#define DIO_CHANNEL_C15 DIO_CHANNEL(DIO_PORT_C, 15)
    

/*******************************************************************************
 * DIO Driver API
 ******************************************************************************/

 /*******************************************************************************
 * @typedef Dio_ChannelType
 * @brief  Type definition for DIO channel identifiers
 * @details This type is used to identify individual DIO channels.
 *          Each channel corresponds to a specific GPIO pin.
 ******************************************************************************/
typedef uint8 Dio_ChannelType;

/*******************************************************************************
 * @typedef Dio_PortType
 * @brief  Type definition for DIO port identifiers
 * @details This type is used to identify individual DIO ports.
 *          Each port corresponds to a group of GPIO pins.
 ******************************************************************************/
typedef uint8 Dio_PortType;

/*******************************************************************************
 * @typedef Dio_LevelType
 * @brief  Type definition for DIO pin levels
 * @details This type is used to represent the logical state of a DIO pin.
 *          It can be either STD_HIGH (1) or STD_LOW (0).
 ******************************************************************************/
typedef uint8 Dio_LevelType;

/*******************************************************************************
 * @typedef Dio_PortLevelType
 * @brief  Type definition for DIO port levels
 * @details This type is used to represent the logical state of all pins in a DIO port.
 *          It is a bitmask where each bit corresponds to a pin in the port.
 ******************************************************************************/
typedef uint16 Dio_PortLevelType;

/*******************************************************************************
 * @struct Dio_ChannelGroupType
 * @brief Type definition for DIO channel group
 * @details This type is used to define a group of channels within a port.
 */
typedef struct
{
    Dio_PortType        port;       /**< Port identifier */
    uint16              mask;       /**< Channel mask within the port */
    uint8               offset;     /**< Bit offset within the port */
} Dio_ChannelGroupType;

/*******************************************************************************
 * @fn Dio_ReadChannel
 * @brief Read the level of a specific DIO channel
 * @param[in] ChannelId Identifier of the DIO channel to read
 * @return Dio_LevelType: The logical level of the specified DIO channel (STD_HIGH or STD_LOW)
 ******************************************************************************/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);

/*******************************************************************************
 * @fn Dio_WriteChannel
 * @brief Write a logical level to a specific DIO channel
 * @param[in] ChannelId Identifier of the DIO channel to write
 * @param[in] Level The logical level to write (STD_HIGH or STD_LOW)
 ******************************************************************************/
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);

/*******************************************************************************
 * @fn Dio_ReadPort
 * @brief Read the level of all pins in a specific DIO port
 * @param[in] PortId Identifier of the DIO port to read
 * @return Dio_PortLevelType: The logical levels of all pins in the specified DIO port
 ******************************************************************************/
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId);

/*******************************************************************************
 * @fn Dio_WritePort
 * @brief Write logical levels to all pins in a specific DIO port
 * @param[in] PortId Identifier of the DIO port to write
 * @param[in] Level The logical levels to write to the pins in the specified DIO port
 ******************************************************************************/
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level);

/*******************************************************************************
 * @fn Dio_ReadChannelGroup
 * @brief Read the levels of a group of DIO channels
 * @param[in] ChannelGroup Pointer to the channel group structure defining the group
 * @return Dio_PortLevelType: The logical levels of the channels in the specified group
 ******************************************************************************/
Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroup);

/*******************************************************************************
 * @fn Dio_WriteChannelGroup
 * @brief Write logical levels to a group of DIO channels
 * @param[in] ChannelGroup Pointer to the channel group structure defining the group
 * @param[in] Level The logical levels to write to the channels in the specified group
 ******************************************************************************/
void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroup, Dio_PortLevelType Level);

/*******************************************************************************
 * @fn Dio_FlipChannel
 * @brief Toggle the level of a specific DIO channel
 * @param[in] ChannelId Identifier of the DIO channel to toggle
 ******************************************************************************/
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId);

/*******************************************************************************
 * @fn Dio_GetVersionInfo
 * @brief Get version information of the DIO driver
 * @param[out] VersionInfo Pointer to a structure to receive the version information
 ******************************************************************************/
void Dio_GetVersionInfo(Std_VersionInfoType* VersionInfo);

/*******************************************************************************
 * @fn Dio_MaskedWritePort
 * @brief Write logical levels to selected pins in a specific DIO port using a mask
 * @param[in] PortId Identifier of the DIO port to write
 * @param[in] Level The logical levels to write to the pins in the specified DIO port
 * @param[in] Mask Bitmask indicating which pins to modify
 ******************************************************************************/
void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask);



#endif /* DIO_H */

/*******************************************************************************
 * END OF FILE
 ******************************************************************************/