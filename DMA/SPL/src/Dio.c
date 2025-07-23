/**
 * @file    Dio.c
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

#include "Dio.h"

/* Implementation of AUTOSAR DIO APIs */


static inline GPIO_TypeDef* GetGPIOPortFromId(uint8 portId) {
    switch (portId) {
        case DIO_PORT_A: return GPIOA;
        case DIO_PORT_B: return GPIOB;
        case DIO_PORT_C: return GPIOC;
        case DIO_PORT_D: return GPIOD;
        default: return NULL;
    }
}

/*******************************************************************************
 * @fn Dio_ReadChannel
 * @brief Read the level of a specific DIO channel
 * @param[in] ChannelId Identifier of the DIO channel to read
 * @return Dio_LevelType: The logical level of the specified DIO channel (STD_HIGH or STD_LOW)
 ******************************************************************************/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    GPIO_TypeDef *GPIO_Port; /* pointer to indentify GPIO Port */
    uint16_t GPIO_Pin; /* GPIO Pin */

    /* Determine GPIO Port with DIO_GET_PORT */
    GPIO_Port = GetGPIOPortFromId(DIO_GET_PORT(ChannelId));

    /* Check if GPIO Port is legitimate */
    if (GPIO_Port == NULL_PTR)
    {
        return STD_LOW; /* Invalid port, return low level */
    }

    /* Determine GPIO Pin with DIO_GET_PIN */
    GPIO_Pin = DIO_GET_PIN(ChannelId);

    /* Read the pin level */
    if (GPIO_ReadInputDataBit(GPIO_Port, GPIO_Pin) == Bit_SET)
    {
        return STD_HIGH; /* Pin is high */
    }
    else
    {
        return STD_LOW; /* Pin is low */
    }
}

/*******************************************************************************
 * @fn Dio_WriteChannel
 * @brief Write a logical level to a specific DIO channel
 * @param[in] ChannelId Identifier of the DIO channel to write
 * @param[in] Level The logical level to write (STD_HIGH or STD_LOW)
 ******************************************************************************/
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
    GPIO_TypeDef *GPIO_Port; /* pointer to indentify GPIO Port */
    uint16_t GPIO_Pin; /* GPIO Pin */

    /* Determine GPIO Port with DIO_GET_PORT */
    GPIO_Port = GetGPIOPortFromId(DIO_GET_PORT(ChannelId));

    /* Check if GPIO Port is legitimate */
    if (GPIO_Port == NULL_PTR)
    {
        return; /* Invalid port, do nothing */
    }
    /* Determine GPIO Pin with DIO_GET_PIN */
    GPIO_Pin = DIO_GET_PIN(ChannelId);

    /* Write the pin level */
    if (Level == STD_HIGH)
    {
        GPIO_SetBits(GPIO_Port, GPIO_Pin); /* Set pin high */
    }
    else
    {
        GPIO_ResetBits(GPIO_Port, GPIO_Pin); /* Set pin low */
    }
}


/*******************************************************************************
 * @fn Dio_ReadPort
 * @brief Read the level of all pins in a specific DIO port
 * @param[in] PortId Identifier of the DIO port to read
 * @return Dio_PortLevelType: The logical levels of all pins in the specified DIO port
 ******************************************************************************/
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    /* Use DIO_GET_PORT to idenifiy GPIO Port */
    GPIO_TypeDef *GPIO_Port = GetGPIOPortFromId(PortId);


    /* Check if GPIO Port is legitimate */
    if (GPIO_Port == NULL_PTR)
    {
        return 0; /* Invalid port, return zero level */
    }

    /* Read the port level */
    return (Dio_PortLevelType)GPIO_ReadInputData(GPIO_Port);
}

/*******************************************************************************
 * @fn Dio_WritePort
 * @brief Write logical levels to all pins in a specific DIO port
 * @param[in] PortId Identifier of the DIO port to write
 * @param[in] Level The logical levels to write to the pins in the specified DIO port
 ******************************************************************************/
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
    /* Use DIO_GET_PORT to identify GPIO Port */
    GPIO_TypeDef *GPIO_Port = GetGPIOPortFromId(PortId);


    /* Check if GPIO Port is legitimate */
    if (GPIO_Port == NULL_PTR)
    {
        return; /* Invalid port, do nothing */
    }

    /* Write the port level */
    GPIO_Write(GPIO_Port, Level);
}

/*******************************************************************************
 * @fn Dio_ReadChannelGroup
 * @brief Read the levels of a group of DIO channels
 * @param[in] ChannelGroup Pointer to the channel group structure defining the group
 * @return Dio_PortLevelType: The logical levels of the channels in the specified group
 ******************************************************************************/
Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr)
{
    /* Use DIO_GET_PORT to identify GPIO Port */
    GPIO_TypeDef *GPIO_Port = GetGPIOPortFromId(ChannelGroupIdPtr->port);

    /* Check if GPIO Port is legitimate */
    if (GPIO_Port == NULL_PTR)
    {
        return 0; /* Invalid port, do nothing */
    }

    /* Read logic states of all pin */
    uint16_t portData = GPIO_ReadInputData(GPIO_Port);

    /* Use mask and Offset to  get the Channel Group value */
    return (Dio_PortLevelType)((portData & ChannelGroupIdPtr->mask) >> ChannelGroupIdPtr->offset);
}

/*******************************************************************************
 * @fn Dio_WriteChannelGroup
 * @brief Write logical levels to a group of DIO channels
 * @param[in] ChannelGroup Pointer to the channel group structure defining the group
 * @param[in] Level The logical levels to write to the channels in the specified group
 ******************************************************************************/
void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level)
{
    /* Use DIO_GET_PORT to identify GPIO Port */
    GPIO_TypeDef *GPIO_Port = GetGPIOPortFromId(ChannelGroupIdPtr->port);

    /* Check if GPIO Port is legitimate */
    if (GPIO_Port == NULL_PTR)
    {
        return; /* Invalid port, do nothing */
    }
    
    /* Read logic states of all pin */
    uint16_t portData = GPIO_ReadInputData(GPIO_Port);

    /* Delete old bits in the group and write with new bits */
    portData &= ~(ChannelGroupIdPtr->mask); /* Clear bits according to mask */
    portData |= ((Level << ChannelGroupIdPtr->offset) & ChannelGroupIdPtr->mask); /* Write new value */

    /* Write new data to GPIO */
    GPIO_Write(GPIO_Port, portData);   
}

/*******************************************************************************
 * @fn Dio_FlipChannel
 * @brief Toggle the level of a specific DIO channel
 * @param[in] ChannelId Identifier of the DIO channel to toggle
 ******************************************************************************/
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
    Dio_LevelType currentLevel = Dio_ReadChannel(ChannelId);

    if (currentLevel == STD_HIGH)
    {
        Dio_WriteChannel(ChannelId, STD_LOW);
        return STD_LOW;
    }
    else
    {
        Dio_WriteChannel(ChannelId, STD_HIGH);
        return STD_HIGH;
    }
}

/*******************************************************************************
 * @fn Dio_GetVersionInfo
 * @brief Get version information of the DIO driver
 * @param[out] VersionInfo Pointer to a structure to receive the version information
 ******************************************************************************/
void Dio_GetVersionInfo (Std_VersionInfoType* VersionInfo)
{
    VersionInfo->vendorID = 0x1234;
    VersionInfo->moduleID = 0x5678;
    VersionInfo->sw_major_version = 1;
    VersionInfo->sw_minor_version = 0;
    VersionInfo->sw_patch_version = 0;
}

/*******************************************************************************
 * @fn Dio_MaskedWritePort
 * @brief Write logical levels to selected pins in a specific DIO port using a mask
 * @param[in] PortId Identifier of the DIO port to write
 * @param[in] Level The logical levels to write to the pins in the specified DIO port
 * @param[in] Mask The mask indicating which pins to modify
 ******************************************************************************/
void Dio_MaskedWritePort(
    Dio_PortType PortId,
    Dio_PortLevelType Level,
    Dio_PortLevelType Mask
)
{
     GPIO_TypeDef *GPIO_Port = GetGPIOPortFromId(PortId);

    if (GPIO_Port == NULL_PTR)
    {
        return; /* Invalid port, do nothing */
    }

    uint16_t portData = GPIO_ReadOutputData(GPIO_Port);
    portData = (portData & ~Mask) | (Level & Mask);
    GPIO_Write(GPIO_Port, portData);
}
/* End of file */