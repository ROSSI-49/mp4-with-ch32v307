/*
 * @ author       : ROSSI-49
 * @ date         : 2023-07-03 12:55
 * @ last_editors : ROSSI-49
 * @ last_edit_time: 2023-10-25 12:04
 * @ path         : \undefinedc:\Users\77249\Desktop\ES8388\�Ʋ���MP4\User\main.c
 * @ description  : һ���Ʋ����MP4 with ch32v307--ѩ��
 * 
 * Copyright (c) 2023 by ROSSI, All Rights Reserved. 
 */
#include "headfile.h"


/* Global Variable */

// Ԥ��һ���ֶ�д������������
FATFS fs;                               /* FatFs�ļ�ϵͳ���� */
FIL fnew;                               /* �ļ����� */
UINT fnum;                              /* �ļ��ɹ���д���� */
FRESULT res_sd;                         /* �ļ�������� */
BYTE    work[FF_MAX_SS];
BYTE WriteBuffer[] = {"txt.test\r\n"};  /* д������*/

////������Ƶ��ʾ��ȫ�ֱ���
char *(video_paths)[4]={"1:/01.bin","1:/02.bin","1:/03.bin","1:/04.bin"};
char * video_path;

/*������ʾ�Ļ�����*/
unsigned char framebuffer[PIC_SIZE] = {1};

//���µ���Ϣ��������ʾ���м����
char *videopath;                        //��Ƶ�ļ��Ļ���ַ
int videooffset ;                       //ƫ������ͨ��ƫ����һ�ζ�ȡ��Ƶ�е�ÿһ֡ͼ��
int file_size ;                         //��Ƶ�ļ��ܹ��ж���֡��Ŀ�����ж��Ƿ�����˲���
int framenumber ;                       //Ŀǰ���ŵ��ڼ�֡
UINT video_br;
FIL video_fil ;

//���ڴ��ڵ�ȫ�ֱ���
char USART_ReceiveString[10];                                                   //���մ����ַ���
int Receive_Flag = 0;                                                           //�Ƿ����������Ϣ
int Receive_sum = 0;                                                            //�����±�

//char *(audio_paths)[3] = {*audio_path1, *audio_path2, *audio_path3};
char *(audio_paths)[4] = {"1:/01.wav","1:/02.wav", "1:/03.wav","1:/04.wav"};
char * audio_path;

uint16_t audiobuffer[AUDIO_LEN] = {1};        //������ֵĻ��棬���԰����е�����ȡ���������LED��������

//���µ���Ϣ�����ڲ������ֵ��м������
char *audiopath;                        //�����ļ��Ļ���ַ,ÿһ֡�ļ���ȡ�ı�־
int audio_offset;                       //ƫ��������һ֡���ļ��еĵ�ַ
WavHead audio_inf;                      //�����ļ��������Ϣ������������ʾ��Ҳ���Բ���ʾ
UINT audio_br;
FIL audio_fil ;

int play_flag=0;
//������Ƶ��ʾ��ȫ�ֱ���
int path_index=0;
FRESULT res ;


void Audio_Init(char* path){
    audiopath = path;
    res = f_open(&audio_fil,path, FA_READ);
    audio_offset = 0;
    //    ��wav�ļ�����ȡ���ļ�ͷ�е���Ϣ
    if(res != FR_OK){
        printf("audio open error\r\n");
        printf("audio address:%d\r\n",path);
        //return;
    }
    int temp = (int)f_size(&audio_fil);
    printf("audio size:%d\r\n",temp);
    res = f_read(&audio_fil,&audio_inf,sizeof(audio_inf),&audio_br);
    if (res != FR_OK){
        printf("audio information get error\r\n");
        //return;
    }
    else {
        printf("audio open success\r\n");
    }
    //����ͷ�ļ��Ĳ���
    audio_offset = sizeof(audio_inf);
    /*res = f_lseek(&audio_fil, audio_offset);
    if(FR_OK != res)
        printf("Lseek ERROR!\r\n");
     */
    /*res = f_close(&audio_fil);
    if (res!=FR_OK){
        printf("file close error!!\r\n");
    }
     */
    printf("Init 8388\r\n");
    ES8388_Init();              //ES8388��ʼ��
    ES8388_Set_Volume(16);      //���ö�������

    ES8388_I2S_Cfg(0,3);    //����Ϊ�����ָ�ʽ��16bit����
    //DA mode
    ES8388_ADDA_Cfg(0,1);
    ES8388_Output_Cfg(1);
    //GPIO_WriteBit(GPIOD,GPIO_Pin_11,0);  // ����PA8��Ϊ1������������307

    I2S2_Init_TX();

    // TIM1_Int_Init(10000/20-1,14400-1);  // ÿ0.1s����һ���жϣ��ŵ�Video�н��г�ʼ��������֡����

}

void play(void){
    TIM_Cmd(TIM1, ENABLE);
    play_flag=1;
}

void stop(void){
    TIM_Cmd(TIM1, DISABLE);
    play_flag=0;
}

// ��������Ƶ�������õ��ĺ���
// ȫ������TIM1��������
void TIM8_Int_Init(u16 arr,u16 psc){

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
    NVIC_InitTypeDef         NVIC_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);       //ʹ��TIM1ʱ��

    TIM_TimeBaseStructure.TIM_Period = arr;                    //ָ���´θ����¼�ʱҪ���ص���Զ����¼��ؼĴ����е�����ֵ��
    TIM_TimeBaseStructure.TIM_Prescaler =psc;                  //ָ�����ڻ���TIMʱ�ӵ�Ԥ��Ƶ��ֵ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //ʱ�ӷ�Ƶ����
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM����ģʽ�����¼���ģʽ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);            //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM NVIC�������ж����ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;         //TIM1�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //������ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //������Ӧ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //ʹ��ͨ��1�ж�
    NVIC_Init(&NVIC_InitStructure);                            //��ʼ��NVIC

    TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE ); //ʹ��TIM1�жϣ���������ж�

    /*TIM_IT_Update:�����жϣ��������������/�����������������ʼ��(ͨ����������ڲ�/�ⲿ����)
      TIM_IT_CC1~4�����ǲ���/�Ƚ��жϣ�ò�ƶ���ƽ�ȵģ������벶������Ƚ�
      TIM_IT_Trigger�������¼�(������������ֹͣ����ʼ���������ڲ�/�ⲿ��������*/

    //TIM_Cmd(TIM1, ENABLE); //TIM1ʹ��
}
// ͨ���ϸ�������TIM1��ʼ����������Ƶ��������Ҫ�ļ�ʱ��������жϣ�
void VideoNVICInit(char *path, int framerate){
    videopath = path;
    res = f_open(&video_fil,videopath, FA_READ);
    if(FR_OK != res)
    {
        printf("VIDEO FILE OPEN ERROR\r\n");
        printf("video address:%d\r\n",path);

    }
    //���ļ��ɹ�����ʼ���ļ���ȡ������������֡�ʳ�ʼ����ʱ��
    else{
        //��Ƶ�Ĳ�����TIM1���ж������

        videooffset = 0;
        int temp = (int)f_size(&video_fil);
        file_size = temp/PIC_SIZE;
        framenumber = 0;

        TIM1_Int_Init(10000/framerate-1,7200-1);

    }
    /*    res = f_close(&video_fil);
    if (res!=FR_OK){
        printf("file close error!!\r\n");
    }*/

}

void VideoSet(char *path, int framerate){
    res = f_close(&video_fil);
    if (res!=FR_OK){
        printf("file close error!!\r\n");
    }
    videopath = path;
    res = f_open(&video_fil,videopath, FA_READ);
    if(FR_OK != res)
    {
        printf("VIDEO FILE OPEN ERROR\r\n");
        printf("video address:%d\r\n",path);

    }
    //���ļ��ɹ�����ʼ���ļ���ȡ������������֡�ʳ�ʼ����ʱ��
    else{
        //��Ƶ�Ĳ�����TIM1���ж������

        videooffset = 0;
        int temp = (int)f_size(&video_fil);
        file_size = temp/PIC_SIZE;
        framenumber = 0;

        //TIM1_Int_Init(10000/framerate-1,7200-1);

    }
    /*    res = f_close(&video_fil);
    if (res!=FR_OK){
        printf("file close error!!\r\n");
    }*/

}

void AudioSet(char* path){
    res = f_close(&audio_fil);
    if (res!=FR_OK){
        printf("file close error!!\r\n");
    }
    audiopath = path;
    res = f_open(&audio_fil,path, FA_READ);
    audio_offset = 0;
    //    ��wav�ļ�����ȡ���ļ�ͷ�е���Ϣ
    if(res != FR_OK){
        printf("audio open error\r\n");
        printf("audio address:%d\r\n",path);
        //return;
    }
    int temp = (int)f_size(&audio_fil);
    printf("audio size:%d\r\n",temp);
    res = f_read(&audio_fil,&audio_inf,sizeof(audio_inf),&audio_br);
    if (res != FR_OK){
        printf("audio information get error\r\n");
        //return;
    }
    else {
        printf("audio open success\r\n");
    }
    //����ͷ�ļ��Ĳ���
    audio_offset = sizeof(audio_inf);
    /*res = f_lseek(&audio_fil, audio_offset);
    if(FR_OK != res)
        printf("Lseek ERROR!\r\n");
     */
    /*res = f_close(&audio_fil);
    if (res!=FR_OK){
        printf("file close error!!\r\n");
    }
     */


    // TIM1_Int_Init(10000/20-1,14400-1);  // ÿ0.1s����һ���жϣ��ŵ�Video�н��г�ʼ��������֡����

}



//�����жϵĳ�ʼ��
/*SW1~SW4:PE0~PE3*/
//SW1������&��ͣ
void EXTI0_INT_INIT()
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct;
    EXTI_InitTypeDef EXTI_InitTypdefStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO , ENABLE);

    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_0;//SW1
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
    EXTI_InitTypdefStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitTypdefStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//SW2����һ��
void EXTI1_INT_INIT()
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct;
    EXTI_InitTypeDef EXTI_InitTypdefStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO , ENABLE);

    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_1;//SW1
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
    EXTI_InitTypdefStruct.EXTI_Line = EXTI_Line1;
    EXTI_InitTypdefStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//SW3����һ��
void EXTI2_INT_INIT()
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct;
    EXTI_InitTypeDef EXTI_InitTypdefStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO , ENABLE);

    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_2;//SW1
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
    EXTI_InitTypdefStruct.EXTI_Line = EXTI_Line2;
    EXTI_InitTypdefStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//SW4���ز�
void EXTI3_INT_INIT()
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct;
    EXTI_InitTypeDef EXTI_InitTypdefStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO , ENABLE);

    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_3;//SW1
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
    EXTI_InitTypdefStruct.EXTI_Line = EXTI_Line3;
    EXTI_InitTypdefStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//�ⲿ�жϳ�ʼ��
void EXTI_INT_INIT()
{
    EXTI0_INT_INIT();
    EXTI1_INT_INIT();
    EXTI2_INT_INIT();
    EXTI3_INT_INIT();
}

/*******************************************************************************
 Function Name  : main
 Description    : Main program.
 Input          : None
 Return         : None
 *******************************************************************************/
int main(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    EXTI_INT_INIT();//�ⲿ�жϳ�ʼ��
    Delay_Ms(100);
    audio_path=audio_paths[path_index];
    video_path=video_paths[path_index];
    printf("Start \r\n");


    res_sd = f_mount(&fs,"1:",1);

    if(res_sd == FR_NO_FILESYSTEM)
    {
        printf("formatting\r\n");
        res_sd=f_mkfs("1:",0,work, sizeof(work));
        res_sd = f_mount(NULL,"1:",1);
        /* ���¹��� */
        res_sd = f_mount(&fs,"1:",1);
    }

    LCD_Init();//LCD��ʼ��

    VideoNVICInit(video_path, 2);

    Audio_Init(audio_path);
    play();
    // VideoPlay();

    while (1)
    {

    }

}

// TIM1�ж��������ֲ���
void TIM1_UP_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void){
    audio_offset=audio_offset+AUDIO_LEN;
    res = f_lseek(&audio_fil, audio_offset);
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);//�����־λ
    if(FR_OK != res)
        printf("Lseek ERROR!\r\n");
    res = f_read(&audio_fil, audiobuffer, AUDIO_LEN, &audio_br);
    if (res != FR_OK){
        printf("audio play read error\r\n");
        return;
    }
    else {
        // printf("audio frame %d\r\n",audio_offset);
    }
    DMA_Tx_Init( DMA1_Channel5, (u32)&SPI2->DATAR, &audiobuffer, AUDIO_LEN/2  );
    DMA_Cmd( DMA1_Channel5, ENABLE );

    if(framenumber < file_size-2){
        res = f_read(&video_fil, framebuffer, PIC_SIZE, &video_br);
        if(FR_OK != res)
            printf("TIM8 ERROR!\r\n");
        else{
            //��ӡͼƬ
            //printf("print\r\n");
            LCD_ShowPicture(DISPLAY_START_X,DISPLAY_START_Y,128,128,framebuffer);
            //�������һ��ͼƬ��ƫ��
            videooffset += PIC_SIZE;
            //���ļ�����ַ����ƫ��
            res = f_lseek(&video_fil, videooffset);
            ++framenumber;
            if(FR_OK != res)
                printf("Lseek ERROR!\r\n");
            TIM_OCInitTypeDef TIM_OCInitStructure={0};
            TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
            TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
            u16 temp=0;
            for(int i=0;i<15;i++){
                temp+=audiobuffer[4*i];
            }
            TIM_OCInitStructure.TIM_Pulse = temp/5;
            TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
            TIM_OC1Init( TIM1, &TIM_OCInitStructure );
            temp=0;

            for(int i=0;i<15;i++){
                            temp+=audiobuffer[4*i+1];
                        }
            TIM_OCInitStructure.TIM_Pulse = temp;
            TIM_OC2Init( TIM1, &TIM_OCInitStructure );
            temp=0;
            for(int i=0;i<15;i++){
                            temp+=audiobuffer[4*i+2];
                        }
            TIM_OCInitStructure.TIM_Pulse = temp;
            TIM_OC3Init( TIM1, &TIM_OCInitStructure );
            temp=0;
            for(int i=0;i<15;i++){
                            temp+=audiobuffer[4*i+3];
                        }
            TIM_OCInitStructure.TIM_Pulse = temp;
            TIM_OC4Init( TIM1, &TIM_OCInitStructure );

            while( (!DMA_GetFlagStatus(DMA1_FLAG_TC5))){
            };
        }
    }
    else{
        //ͼƬ�Ѿ���ɶ�ȡ���ر��ļ�����ֹ�ж�
        res = f_close(&video_fil);
        if(FR_OK != res)
            printf("CLOSE FILE ERROR!\r\n");
        else{
            stop();
            VideoSet(video_path, 2);
            AudioSet(audio_path);
            Delay_Ms(10);
            play();

            printf("VIDEO FINISHED!\r\n");
        }
    }


}



//SW1������&��ͣ
void EXTI0_IRQHandler(void)__attribute__ ((interrupt("WCH-Interrupt-fast")));
void EXTI0_IRQHandler(void)
{
    if(play_flag==1){
        stop();
    }
    else {
        play();
    }
    Delay_Ms(10);
    printf("�жϴ�����\r\n");
    EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
}

//SW2����һ��
void EXTI1_IRQHandler(void)__attribute__ ((interrupt("WCH-Interrupt-fast")));
void EXTI1_IRQHandler(void)
{
    stop();
    path_index=path_index-1;//path_index��0~2
    if(path_index<0){
        path_index=3;
    }
    audio_path=audio_paths[path_index];
    video_path=video_paths[path_index];
    printf("last one\r\n");
    VideoSet(video_path, 2);
    AudioSet(audio_path);
    Delay_Ms(50);
    play();
    EXTI_ClearITPendingBit(EXTI_Line1);     /* Clear Flag */
}

//SW3����һ��
void EXTI2_IRQHandler(void)__attribute__ ((interrupt("WCH-Interrupt-fast")));
void EXTI2_IRQHandler(void)
{
    stop();
    path_index++;//path_index��0~2
    if(path_index>=4){
        path_index=0;
    }
    audio_path=audio_paths[path_index];
    video_path=video_paths[path_index];
    printf("next one\r\n");
    VideoSet(video_path, 2);
    AudioSet(audio_path);
    Delay_Ms(50);
    play();
    EXTI_ClearITPendingBit(EXTI_Line2);     /* Clear Flag */
}

//SW4���ز�
void EXTI3_IRQHandler(void)__attribute__ ((interrupt("WCH-Interrupt-fast")));
void EXTI3_IRQHandler(void)
{
    printf("replay\r\n");
    stop();
    VideoSet(video_path, 2);
    AudioSet(audio_path);
    Delay_Ms(50);
    play();
    EXTI_ClearITPendingBit(EXTI_Line3);     /* Clear Flag */
}

//UART2
__attribute__((interrupt("WCH-Interrupt-fast")))
void USART2_IRQHandler(void)
{
    int rx_mode = 0;                                                              //0��ֻ��ȡ���ݣ����жϲ�����ģʽ��1���жϲ�����ģʽ
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == 1)
    {
        if(Receive_sum > 9)                                                     //�ܹ�������10������
        {
            USART_ReceiveString[9] = '\0';                                      //�������ܷ�Χ����ֹ����
            Receive_Flag = 1;                                                   //���ձ�־λ��1��ֹͣ��������
            Receive_sum = 0;                                                    //�����±���0
            printf("usart rx out of space\r\n");
        }

        if(Receive_Flag == 0)                                                   //���ձ�־λ����0����ʼ��������
        {
            USART_ReceiveString[Receive_sum] = USART_ReceiveData(USART2);       //ͨ��USART2���ڽ����ַ�
            Receive_sum++;                                                      //�����±�++
        }

        if(Receive_sum >= 2)                                                    //�����±����2,\r\n��Ϊ��ֹ��
        {
            if(USART_ReceiveString[Receive_sum-2] == '\r' && USART_ReceiveString[Receive_sum-1] == '\n' )
            {
                USART_ReceiveString[Receive_sum-1] = '\0';
                USART_ReceiveString[Receive_sum-2] = '\0';
                Receive_sum = 0;                                                //�����±���0
                printf("%s\r\n",USART_ReceiveString);
                rx_mode = 1;
            }
        }
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);                          //��ձ�־λ
    }
    if(rx_mode == 1){
        if(strcmp(USART_ReceiveString,"play") == 0){
            printf("play cmd\r\n");
            if(play_flag==0){
                play();
                play_flag=1;
            }
        }
        else if(strcmp(USART_ReceiveString,"stop") == 0){
            printf("stop cmd\r\n");
            if(play_flag==1){
                stop();
                play_flag=0;
            }
        }
        else if(strcmp(USART_ReceiveString,"prev") == 0){
            printf("prev cmd\r\n");
            stop();
            path_index=path_index-1;//path_index��0~2
            if(path_index<0){
                path_index=3;
            }
            audio_path=audio_paths[path_index];
            video_path=video_paths[path_index];
            printf("last one\r\n");
            VideoSet(video_path, 2);
            AudioSet(audio_path);
            Delay_Ms(50);
            play();
        }
        else if(strcmp(USART_ReceiveString,"next") == 0){
            printf("next cmd\r\n");
            //EXTI2_IRQHandler();
            stop();
            path_index++;//path_index��0~2
            if(path_index>=4){
                path_index=0;
            }
            audio_path=audio_paths[path_index];
            video_path=video_paths[path_index];
            printf("next one\r\n");
            VideoSet(video_path, 2);
            AudioSet(audio_path);
            Delay_Ms(50);
            play();
        }
        else if(strcmp(USART_ReceiveString,"replay") == 0){
            printf("replay cmd\r\n");
            stop();
            VideoSet(video_path, 2);
            AudioSet(audio_path);
            Delay_Ms(50);
            play();
        }
        else {
            printf("not valid cmd!\r\n");
        }
    }
}

// TIM8���жϺ���ʵ�̶ֹ�֡�ʵ���Ƶ���ţ������ã�����ȫ��TIM1��
/*void TIM8_UP_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM8_UP_IRQHandler(void)
{
    printf("NVIC TIM8\r\n");
    printf("frame size:%d\r\n",file_size);
    printf("frame number:%d\r\n",framenumber);
    TIM_ClearFlag(TIM8, TIM_FLAG_Update);//�����־λ
    if(framenumber < file_size){
        res = f_read(&video_fil, framebuffer, PIC_SIZE, &video_br);
        if(FR_OK != res)
            printf("TIM8 ERROR!\r\n");
        else{
            //��ӡͼƬ
            printf("print\r\n");
            LCD_ShowPicture(DISPLAY_START_X,DISPLAY_START_Y,128,128,framebuffer);
            //�������һ��ͼƬ��ƫ��
            videooffset += PIC_SIZE;
            //���ļ�����ַ����ƫ��
            res = f_lseek(&video_fil, videooffset);
            ++framenumber;
            if(FR_OK != res)
                printf("Lseek ERROR!\r\n");
        }
    }
    else{
        //ͼƬ�Ѿ���ɶ�ȡ���ر��ļ�����ֹ�ж�
        res = f_close(&video_fil);
        if(FR_OK != res)
            printf("CLOSE FILE ERROR!\r\n");
        else{
            TIM_ITConfig(TIM8,TIM_IT_Update,DISABLE);
            TIM_Cmd(TIM8, DISABLE);
            printf("VIDEO FINISHED!\r\n");
        }
    }


}*/
