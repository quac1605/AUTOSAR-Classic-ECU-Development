#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>
#include "stm32f10x.h"   // Thư viện SPL STM32F1
#include "stm32f10x_can.h"
#include "stm32f10x_usart.h"
// ===================== AUTOSAR Type Definitions =====================

// Chuẩn AUTOSAR: Can_IdType, Can_HwHandleType, Can_PduType
typedef uint32_t Can_IdType;              // Chuẩn AUTOSAR: CAN ID 11/29 bit
typedef uint8_t  Can_HwHandleType;        // Handle phần cứng cho TX/RX

typedef struct {
    Can_IdType id;            // CAN ID (chuẩn hoặc mở rộng)
    uint8_t length;           // Số byte dữ liệu (1-8)
    uint8_t* sdu;             // Con trỏ buffer dữ liệu
    uint32_t swPduHandle;     // PDU handle từ CanIf (nếu cần dùng mapping ngược)
} Can_PduType;

// Cấu hình timing, filter, feature tối giản
typedef struct {
    uint16_t CAN_Prescaler;
    uint8_t  CAN_Mode;
    uint8_t  CAN_SJW;
    uint8_t  CAN_BS1;
    uint8_t  CAN_BS2;
    // Có thể mở rộng thêm các trường khác nếu cần
    uint16_t FilterIdHigh;
    uint16_t FilterIdLow;
    uint16_t FilterMaskIdHigh;
    uint16_t FilterMaskIdLow;
} Can_ConfigType;

// ===================== API Prototype (chuẩn AUTOSAR) =====================
void Can_Init(const Can_ConfigType* Config);
uint8_t Can_Write(Can_HwHandleType Hth, const Can_PduType* PduInfo);
void Can_RegisterRxCallback(void (*cb)(Can_IdType canId, uint8_t* data, uint8_t len));

// Gọi từ ISR hardware khi nhận được frame (trong USB_LP_CAN1_RX0_IRQHandler)
void USB_LP_CAN1_RX0_IRQHandler(void);

#endif // CAN_H_
