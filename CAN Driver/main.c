#include <stdint.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#include "can.h"     // Can_ConfigType, Can_Init, ...
#include "canif.h"   // CanIf_ConfigType, CanIf_Init, ...

/* ================= UART1 115200-8N1 (PA9 TX, PA10 RX) ================= */
static void UART1_Init_115200_8N1(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef gpio;
    // PA9 TX
    gpio.GPIO_Pin   = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);
    // PA10 RX
    gpio.GPIO_Pin   = GPIO_Pin_10;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    USART_InitTypeDef us;
    USART_StructInit(&us);
    us.USART_BaudRate = 115200;
    us.USART_Mode     = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &us);
    USART_Cmd(USART1, ENABLE);
}

static inline void UART1_WriteByte(uint8_t b){
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = b;
}
static void UART1_WriteStr(const char* s){ while(*s) UART1_WriteByte((uint8_t)*s++); }
static inline char nyb2hex(uint8_t n){ n &= 0xF; return (n<10)?('0'+n):('A'+n-10); }
static void UART1_WriteHex2(uint8_t b){
    UART1_WriteByte(nyb2hex(b>>4));
    UART1_WriteByte(nyb2hex(b));
}
static void UART1_WriteHex3_StdID(uint16_t id11){
    // In 3 hex digits cho 11-bit ID (0x000..0x7FF)
    UART1_WriteHex2((id11>>8)&0xFF);
    UART1_WriteHex2(id11&0xFF);
}

/* ================= App callbacks (được CanIf gọi) ================= */
void App_TxConfirm(uint32_t TxPduId){
    UART1_WriteStr("TX-Confirm: TxPduId=");
    // In số nhỏ gọn (chỉ 1 chữ số nếu bạn dùng 0/1); có thể bỏ qua
    UART1_WriteByte('0' + (TxPduId % 10));
    UART1_WriteStr("\r\n");
}

/* In UART ngay trong callback nhận */
void App_RxCallback(uint32_t RxPduId, uint8_t* data, uint8_t len){
    UART1_WriteStr("RX: RxPduId=");
    UART1_WriteByte('0' + (RxPduId % 10));
    UART1_WriteStr(", len=");
    UART1_WriteByte('0' + (len % 10));
    UART1_WriteStr(", data: ");
    for(uint8_t i=0;i<len && i<8;i++){
        UART1_WriteHex2(data[i]);
        UART1_WriteByte(' ');
    }
    UART1_WriteStr("\r\n");
}

/* ================= CanIf config (routing inline) =================
   isTx = 1 cho TX, isTx = 0 cho RX  */
static CanIf_ConfigType canIfCfg = {
    .numControllers        = 1,
    .defaultControllerMode = { CANIF_CONTROLLER_STARTED },
    .numTxPdus             = 1,
    .defaultTxPduMode      = { CANIF_ONLINE },
    .numRxPdus             = 1,
    .defaultRxPduMode      = { CANIF_ONLINE },

    .numRoutingEntry       = 2,
    .routingTable          = {
        { 0, 0x321, 1 },  // TxPduId=0 -> CAN ID 0x321 (TX)
        { 0, 0x123, 0 }   // RxPduId=0 -> CAN ID 0x123 (RX)  <-- quan trọng
    },

    .txConfirmation        = App_TxConfirm,
    .rxIndication          = App_RxCallback
}; // :contentReference[oaicite:4]{index=4} :contentReference[oaicite:5]{index=5}

/* ================= CAN 250 kbps @ PCLK1=36MHz =================
   16TQ: Prescaler=9, SJW=1, BS1=13, BS2=2.
   Filter nhận hết (mask=0) để test nhanh. */
static Can_ConfigType canHwCfg = {
    .CAN_Prescaler    = 9,
    .CAN_Mode         = CAN_Mode_Normal,
    .CAN_SJW          = CAN_SJW_1tq,
    .CAN_BS1          = CAN_BS1_13tq,
    .CAN_BS2          = CAN_BS2_2tq,
    .FilterIdHigh     = 0x0000,
    .FilterIdLow      = 0x0000,
    .FilterMaskIdHigh = 0x0000,
    .FilterMaskIdLow  = 0x0000,
}; // :contentReference[oaicite:6]{index=6}

int main(void){
    SystemInit();                // gọi từ startup rồi, nhưng idempotent
    UART1_Init_115200_8N1();

    Can_Init(&canHwCfg);         // driver bật NVIC + ISR USB_LP_CAN1_RX0_IRQHandler :contentReference[oaicite:7]{index=7}
    CanIf_Init(&canIfCfg);       // đăng ký CanIf_RxIndication với driver :contentReference[oaicite:8]{index=8}

    // (tuỳ chọn) đặt ưu tiên NVIC cho CAN RX0 nếu muốn
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 5);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

    // Gửi thử 1 frame (không bắt buộc)
    // uint8_t d[2]={0xAB,0xCD}; CanIf_Transmit(0, d, 2);

    for(;;){
        __WFI(); // chờ ngắt; khi có CAN, ISR -> CanIf -> App_RxCallback (UART in-place)
    }
}
