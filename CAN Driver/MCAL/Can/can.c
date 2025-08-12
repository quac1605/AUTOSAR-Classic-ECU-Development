#include "can.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

// Biến callback nhận từ CanIf (lưu function pointer)
static void (*rxCallback)(Can_IdType, uint8_t*, uint8_t) = 0;

void Can_Init(const Can_ConfigType* Config)
{
    // 1. Enable clock for CAN1, GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. Cấu hình GPIO PA11 (RX), PA12 (TX)
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_IPU; // RX: Input Pull-up
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_12;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP; // TX: AF Push-pull
    GPIO_Init(GPIOA, &gpio);

    // 3. Init cấu hình CAN
    CAN_InitTypeDef can_init;
    CAN_DeInit(CAN1);
    CAN_StructInit(&can_init);

    can_init.CAN_TTCM = DISABLE;
    can_init.CAN_ABOM = ENABLE;
    can_init.CAN_AWUM = DISABLE;
    can_init.CAN_NART = DISABLE;
    can_init.CAN_RFLM = DISABLE;
    can_init.CAN_TXFP = DISABLE;
    can_init.CAN_Mode = Config->CAN_Mode;
    can_init.CAN_SJW = Config->CAN_SJW;
    can_init.CAN_BS1 = Config->CAN_BS1;
    can_init.CAN_BS2 = Config->CAN_BS2;
    can_init.CAN_Prescaler = Config->CAN_Prescaler;
    CAN_Init(CAN1, &can_init);

    // 4. Init bộ lọc (filter)
    CAN_FilterInitTypeDef filter;
    filter.CAN_FilterNumber = 0;
    filter.CAN_FilterMode = CAN_FilterMode_IdMask;
    filter.CAN_FilterScale = CAN_FilterScale_32bit;
    filter.CAN_FilterIdHigh     = Config->FilterIdHigh;
    filter.CAN_FilterIdLow      = Config->FilterIdLow;
    filter.CAN_FilterMaskIdHigh = Config->FilterMaskIdHigh;
    filter.CAN_FilterMaskIdLow  = Config->FilterMaskIdLow;
    filter.CAN_FilterFIFOAssignment = CAN_FIFO0;
    filter.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&filter);

    // 5. Enable interrupt nhận FIFO0
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn); // ĐÚNG IRQ cho F103
}

// Gửi 1 frame CAN (chuẩn AUTOSAR: truyền vào Hth, PduInfo)
uint8_t Can_Write(Can_HwHandleType Hth, const Can_PduType* PduInfo)
{
    (void)Hth; // demo: 1 mailbox
    CanTxMsg tx;
    tx.StdId = (uint16_t)(PduInfo->id & 0x7FF);
    tx.ExtId = 0;
    tx.IDE   = CAN_Id_Standard;  // Sửa IDE cho đúng SPL
    tx.RTR   = CAN_RTR_DATA;
    tx.DLC   = PduInfo->length;
    for (uint8_t i = 0; i < PduInfo->length && i < 8; ++i)
        tx.Data[i] = PduInfo->sdu[i];

    uint8_t mbox = CAN_Transmit(CAN1, &tx);
    return (mbox < 3) ? 0 : 1; // 0=E_OK, 1=E_NOT_OK
}

// Đăng ký callback nhận frame (CanIf sẽ truyền function pointer vào)
void Can_RegisterRxCallback(void (*cb)(Can_IdType canId, uint8_t* data, uint8_t len))
{
    rxCallback = cb;
}

// ISR nhận dữ liệu từ hardware (tên vector đúng với F103)
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) {
        CanRxMsg rx;
        CAN_Receive(CAN1, CAN_FIFO0, &rx);
        if (rxCallback) {
            rxCallback(rx.StdId, rx.Data, rx.DLC);
        }
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}
