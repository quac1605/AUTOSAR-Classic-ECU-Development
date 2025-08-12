#ifndef CANIF_H_
#define CANIF_H_

#include <stdint.h>

// =================== Hằng số giới hạn hệ thống (demo) ===================
#define CANIF_MAX_CONTROLLERS   2
#define CANIF_MAX_TX_PDUS       4
#define CANIF_MAX_RX_PDUS       4

// =================== Enum trạng thái chuẩn AUTOSAR ===================
typedef enum {
    CANIF_OFFLINE,
    CANIF_ONLINE,
    CANIF_TX_OFFLINE,
    CANIF_RX_OFFLINE
} CanIf_PduModeType;

typedef enum {
    CANIF_CONTROLLER_STOPPED,
    CANIF_CONTROLLER_STARTED
} CanIf_ControllerModeType;

// =================== Prototype callback ===================
typedef void (*CanIf_TxConfirmationCallback)(uint32_t TxPduId);
typedef void (*CanIf_RxIndicationCallback)(uint32_t RxPduId, uint8_t *data, uint8_t len);

// =================== Struct ánh xạ Routing Table ===================
typedef struct {
    uint32_t PduId;     // Định danh logic cho PDU
    uint32_t CanId;     // CAN ID vật lý
    uint8_t  isTx;      // 1 = TX, 0 = RX
} CanIf_RoutingEntry;

// =================== Struct cấu hình tổng thể cho CanIf ===================
typedef struct {
    uint8_t numControllers;
    CanIf_ControllerModeType defaultControllerMode[CANIF_MAX_CONTROLLERS];
    uint8_t numTxPdus;
    CanIf_PduModeType defaultTxPduMode[CANIF_MAX_TX_PDUS];
    uint8_t numRxPdus;
    CanIf_PduModeType defaultRxPduMode[CANIF_MAX_RX_PDUS];

    uint8_t numRoutingEntry;
    CanIf_RoutingEntry routingTable[CANIF_MAX_TX_PDUS + CANIF_MAX_RX_PDUS];

    CanIf_TxConfirmationCallback txConfirmation; // Callback xác nhận truyền
    CanIf_RxIndicationCallback rxIndication;     // Callback nhận dữ liệu
} CanIf_ConfigType;

// =================== Prototype API CanIf ===================
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Khởi tạo module CanIf
 * @param ConfigPtr: con trỏ tới struct cấu hình tổng thể
 */
void CanIf_Init(const CanIf_ConfigType *ConfigPtr);

/**
 * @brief Truyền dữ liệu CAN qua tầng CanIf (dựa trên TxPduId)
 * @param TxPduId: ID logic PDU truyền
 * @param data: buffer data
 * @param len: số byte data
 * @return 0 nếu thành công, <0 nếu lỗi
 */
int  CanIf_Transmit(uint32_t TxPduId, const uint8_t *data, uint8_t len);

/**
 * @brief Callback xác nhận truyền (do Driver gọi lên)
 * @param canId: CAN Identifier vừa truyền
 */
void CanIf_TxConfirmation(uint32_t canId);

/**
 * @brief Callback nhận dữ liệu CAN (do Driver gọi lên)
 * @param canId: CAN Identifier vừa nhận
 * @param data: buffer data
 * @param len: số byte data
 */
void CanIf_RxIndication(uint32_t canId, uint8_t* data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif // CANIF_H_
