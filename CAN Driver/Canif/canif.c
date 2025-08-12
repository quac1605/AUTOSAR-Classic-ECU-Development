#include "canif.h"
#include <string.h>
#include "can.h"  // Đúng tên file driver bạn khai báo

// ===============================
// Biến trạng thái runtime (static)
// ===============================
static CanIf_ControllerModeType controllerMode[CANIF_MAX_CONTROLLERS];
static CanIf_PduModeType txPduMode[CANIF_MAX_TX_PDUS];
static CanIf_PduModeType rxPduMode[CANIF_MAX_RX_PDUS];
static CanIf_RoutingEntry routingTable[CANIF_MAX_TX_PDUS + CANIF_MAX_RX_PDUS];

static uint8_t g_numControllers;
static uint8_t g_numTxPdus;
static uint8_t g_numRxPdus;
static uint8_t g_numRoutingEntry;

static CanIf_TxConfirmationCallback txCb = 0;
static CanIf_RxIndicationCallback rxCb = 0;

// ================================
// Hàm khởi tạo module CanIf
// ================================
void CanIf_Init(const CanIf_ConfigType *ConfigPtr)
{
    if (!ConfigPtr) return;

    g_numControllers  = ConfigPtr->numControllers;
    g_numTxPdus       = ConfigPtr->numTxPdus;
    g_numRxPdus       = ConfigPtr->numRxPdus;
    g_numRoutingEntry = ConfigPtr->numRoutingEntry;

    memcpy(controllerMode, ConfigPtr->defaultControllerMode, sizeof(controllerMode));
    memcpy(txPduMode,      ConfigPtr->defaultTxPduMode,      sizeof(txPduMode));
    memcpy(rxPduMode,      ConfigPtr->defaultRxPduMode,      sizeof(rxPduMode));
    memcpy(routingTable,   ConfigPtr->routingTable,          sizeof(routingTable));

    txCb = ConfigPtr->txConfirmation;
    rxCb = ConfigPtr->rxIndication;

    // Đăng ký callback với CAN driver
    Can_RegisterRxCallback(CanIf_RxIndication);
    // Nếu bạn có callback xác nhận gửi (Tx confirmation), hãy đăng ký hàm tương tự (tùy driver)
    // Can_RegisterTxCallback(CanIf_TxConfirmation);
}

// ================================
// API truyền dữ liệu CAN qua CanIf
// ================================
int CanIf_Transmit(uint32_t TxPduId, const uint8_t *data, uint8_t len)
{
    // Kiểm tra trạng thái PDU phải online mới cho phép truyền
    if (TxPduId >= g_numTxPdus || txPduMode[TxPduId] != CANIF_ONLINE)
        return -1;

    // Tìm CAN ID vật lý từ bảng ánh xạ
    uint32_t canId = 0xFFFFFFFF;
    for (uint8_t i = 0; i < g_numRoutingEntry; i++)
        if (routingTable[i].PduId == TxPduId && routingTable[i].isTx)
            canId = routingTable[i].CanId;

    if (canId == 0xFFFFFFFF)
        return -2;

    // Tạo struct PDU truyền xuống driver (chuẩn AUTOSAR)
    Can_PduType pdu;
    pdu.id = canId;
    pdu.length = len;
    pdu.sdu = (uint8_t*)data;  // Bỏ const cho đúng prototype driver
    pdu.swPduHandle = TxPduId; // Giữ lại TxPduId nếu cần mapping ngược

    // Gọi driver truyền dữ liệu CAN vật lý, Hth: thường là 0 nếu chỉ có 1 mailbox
    return Can_Write(0, &pdu);
}

// ================================
// Callback xác nhận truyền dữ liệu (do Driver gọi lên)
// ================================
void CanIf_TxConfirmation(uint32_t canId)
{
    uint32_t pduId = 0xFFFFFFFF;
    for (uint8_t i = 0; i < g_numRoutingEntry; i++)
        if (routingTable[i].CanId == canId && routingTable[i].isTx)
            pduId = routingTable[i].PduId;

    if (txCb && pduId != 0xFFFFFFFF)
        txCb(pduId);
}

// ================================
// Callback nhận dữ liệu CAN (do Driver gọi lên)
// ================================
void CanIf_RxIndication(uint32_t canId, uint8_t* data, uint8_t len)
{
    uint32_t pduId = 0xFFFFFFFF;
    for (uint8_t i = 0; i < g_numRoutingEntry; i++)
        if (routingTable[i].CanId == canId && !routingTable[i].isTx)
            pduId = routingTable[i].PduId;

    if (rxCb && pduId != 0xFFFFFFFF)
        rxCb(pduId, data, len);
}
