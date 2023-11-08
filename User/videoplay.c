/*
 * video.c
 *
 *  Created on: 2023��11��8��
 *      Author: 77249
 */

#include"videoplay.h"

void I2S2_Init_TX(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    I2S_InitTypeDef  I2S_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_8k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI2, &I2S_InitStructure);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    I2S_Cmd(SPI2, ENABLE);
}

//I2S��DMA��Ӧ�ĳ�ʼ��
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA_CHx, &DMA_InitStructure );
}

void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
    NVIC_InitTypeDef         NVIC_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);       //ʹ��TIM2ʱ��

    TIM_TimeBaseStructure.TIM_Period = arr;                    //ָ���´θ����¼�ʱҪ���ص���Զ����¼��ؼĴ����е�����ֵ��
    TIM_TimeBaseStructure.TIM_Prescaler =psc;                  //ָ�����ڻ���TIMʱ�ӵ�Ԥ��Ƶ��ֵ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //ʱ�ӷ�Ƶ����
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM����ģʽ�����¼���ģʽ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);            //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM NVIC�������ж����ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;             //TIM1�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //������ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //������Ӧ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //ʹ��ͨ��1�ж�
    NVIC_Init(&NVIC_InitStructure);                            //��ʼ��NVIC

    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��TIM2�жϣ���������ж�

    /*TIM_IT_Update:�����жϣ��������������/�����������������ʼ��(ͨ����������ڲ�/�ⲿ����)
      TIM_IT_CC1~4�����ǲ���/�Ƚ��жϣ�ò�ƶ���ƽ�ȵģ������벶������Ƚ�
      TIM_IT_Trigger�������¼�(������������ֹͣ����ʼ���������ڲ�/�ⲿ��������*/
    //TIM_ch2��������PWM-PA9
    GPIO_InitTypeDef GPIO_InitStructure={0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 2*arr;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure );
    TIM_OC2Init( TIM1, &TIM_OCInitStructure );
    TIM_OC3Init( TIM1, &TIM_OCInitStructure );
    TIM_OC4Init( TIM1, &TIM_OCInitStructure );
    TIM_CtrlPWMOutputs(TIM1, ENABLE );
    TIM_OC2PreloadConfig( TIM1, TIM_OCPreload_Disable );
    //������ֹ�ڶ�ʱ������ʱ��ARR�Ļ�������д����ֵ���Ա��ڸ����¼�����ʱ���븲����ǰ��ֵ��
    TIM_ARRPreloadConfig( TIM1, ENABLE );
}



void VideoPlay(void){
    TIM_Cmd(TIM8, ENABLE); //TIM1ʹ��
}

void VideoStop(void){
    TIM_Cmd(TIM8, DISABLE);
}

