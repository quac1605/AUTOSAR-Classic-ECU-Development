#include "Adc_Cfg.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h" 

// Kiểm tra ADC hoàn thành
void MyAdcGroup0_Notification(void)
{
    GPIOC->ODR ^= GPIO_Pin_13;
}

// Kiểm tra DMA hoàn thành
void MyAdcGroup0_DmaCb(void)
{
    // Toggle LED C13 để kiểm tra DMA hoàn thành
    GPIOC->ODR ^= GPIO_Pin_13;
}

// Xử lý ngắt ADC
void ADC1_2_IRQHandler(void)
{
    Adc_IsrHandler(ADC1);
    // Nếu có ADC2: Adc_IsrHandler(ADC2);
}

// Xử lý ngắt DMA1 Channel 1 (ADC1)
void DMA1_Channel1_IRQHandler(void)
{
    for (int i = 0; i < ADC_NUM_GROUPS_DMA; i++) {
        const Adc_GroupDmaConfigType* cfg = &AdcGroupDmaConfig[i];
        if (cfg->DMA_Channel != DMA1_Channel1) continue;
        if (DMA_GetITStatus(cfg->DMA_FLAG_TC)) {
            DMA_ClearITPendingBit(cfg->DMA_FLAG_TC);
            DMA_Cmd(cfg->DMA_Channel, DISABLE);
            ADC_DMACmd(cfg->ADCx, DISABLE);
            if (cfg->NotificationCb) cfg->NotificationCb();
        }
    }
}

const Adc_GroupConfigType AdcGroupConfig[] = {
    {   
        .ADCx = ADC1, 
        .groupId = 0, 
        .channelCount = 2, 
        .NotificationCb = MyAdcGroup0_Notification 
    },
    { 
        .ADCx = ADC1, 
        .groupId = 1, 
        .channelCount = 1, 
        .NotificationCb = NULL 
    }
};

const Adc_GroupDmaConfigType AdcGroupDmaConfig[] = {
    { 
        .ADCx = ADC1, 
        .DMA_Channel = DMA1_Channel1, 
        .DMA_FLAG_TC = DMA1_FLAG_TC1, 
        .groupId = 0, 
        .channelCount = 2, 
        .NotificationCb = MyAdcGroup0_DmaCb 
    },
    { 
        .ADCx = ADC1, 
        .DMA_Channel = DMA1_Channel1, 
        .DMA_FLAG_TC = DMA1_FLAG_TC1, 
        .groupId = 1, 
        .channelCount = 1, 
        .NotificationCb = NULL 
    }
};

// Buffer cho mỗi group (driver quản lý)
static Adc_ValueGroupType* Adc_DmaBuffer[ADC_NUM_GROUPS_DMA] = {0};

// API gán buffer (nội bộ)
Std_ReturnType Adc_SetupResultBuffer_Dma(Adc_GroupType group, Adc_ValueGroupType* buf)
{
    for (int i = 0; i < ADC_NUM_GROUPS_DMA; i++) {
        if (AdcGroupDmaConfig[i].groupId == group) {
            Adc_DmaBuffer[i] = buf;
            return E_OK;
        }
    }
    return E_NOT_OK;
}

// API kích hoạt DMA cho group
Std_ReturnType Adc_EnableDma(Adc_GroupType group)
{
    for (int i = 0; i < ADC_NUM_GROUPS_DMA; i++) {
        const Adc_GroupDmaConfigType* cfg = &AdcGroupDmaConfig[i];
        if (cfg->groupId != group) continue;

        // 1. Tắt & reset DMA channel
        DMA_Cmd(cfg->DMA_Channel, DISABLE);
        DMA_DeInit(cfg->DMA_Channel);

        // 2. Cấu hình DMA
        DMA_InitTypeDef dinit;
        DMA_StructInit(&dinit);
        dinit.DMA_PeripheralBaseAddr = (uint32_t)&(cfg->ADCx->DR);
        dinit.DMA_MemoryBaseAddr     = (uint32_t)Adc_DmaBuffer[i];
        dinit.DMA_DIR                = DMA_DIR_PeripheralSRC;
        dinit.DMA_BufferSize         = cfg->channelCount;
        dinit.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
        dinit.DMA_MemoryInc          = DMA_MemoryInc_Enable;
        dinit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        dinit.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
        dinit.DMA_Mode               = DMA_Mode_Normal;
        dinit.DMA_Priority           = DMA_Priority_High;
        DMA_Init(cfg->DMA_Channel, &dinit);

        // 3. Bật ngắt TC
        DMA_ITConfig(cfg->DMA_Channel, DMA_IT_TC, ENABLE);
        NVIC_EnableIRQ(DMA1_Channel1_IRQn);

        // 4. Bật DMA & ADC<->DMA link
        DMA_Cmd(cfg->DMA_Channel, ENABLE);
        ADC_DMACmd(cfg->ADCx, ENABLE);

        return E_OK;
    }
    return E_NOT_OK;
}

// API tắt DMA cho group
Std_ReturnType Adc_DisableDma(Adc_GroupType group)
{
    for (int i = 0; i < ADC_NUM_GROUPS_DMA; i++) {
        const Adc_GroupDmaConfigType* cfg = &AdcGroupDmaConfig[i];
        if (cfg->groupId == group) {
            ADC_DMACmd(cfg->ADCx, DISABLE);
            DMA_Cmd(cfg->DMA_Channel, DISABLE);
            DMA_ITConfig(cfg->DMA_Channel, DMA_IT_TC, DISABLE);
            return E_OK;
        }
    }
    return E_NOT_OK;
}






