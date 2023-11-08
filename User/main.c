/*
 * @ author       : ROSSI-49
 * @ date         : 2023-07-03 12:55
 * @ last_editors : ROSSI-49
 * @ last_edit_time: 2023-10-25 12:04
 * @ path         : \undefinedc:\Users\77249\Desktop\ES8388\破产版MP4\User\main.c
 * @ description  : 一个破产版的MP4 with ch32v307--雪龙
 * 
 * Copyright (c) 2023 by ROSSI, All Rights Reserved. 
 */
#include "headfile.h"


/* Global Variable */

// 预留一部分读写变量用来测试
FATFS fs;                               /* FatFs文件系统对象 */
FIL fnew;                               /* 文件对象 */
UINT fnum;                              /* 文件成功读写数量 */
FRESULT res_sd;                         /* 文件操作结果 */
BYTE    work[FF_MAX_SS];
BYTE WriteBuffer[] = {"txt.test\r\n"};  /* 写缓冲区*/

////用于视频显示的全局变量
char *(video_paths)[4]={"1:/01.bin","1:/02.bin","1:/03.bin","1:/04.bin"};
char * video_path;

/*用于显示的缓存区*/
unsigned char framebuffer[PIC_SIZE] = {1};

//以下的信息是用于显示的中间变量
char *videopath;                        //视频文件的基地址
int videooffset ;                       //偏置量，通过偏置来一次读取视频中的每一帧图像
int file_size ;                         //视频文件总共有多少帧，目的是判断是否完成了播放
int framenumber ;                       //目前播放到第几帧
UINT video_br;
FIL video_fil ;

//用于串口的全局变量
char USART_ReceiveString[10];                                                   //接收串口字符串
int Receive_Flag = 0;                                                           //是否允许接收消息
int Receive_sum = 0;                                                            //数组下标

//char *(audio_paths)[3] = {*audio_path1, *audio_path2, *audio_path3};
char *(audio_paths)[4] = {"1:/01.wav","1:/02.wav", "1:/03.wav","1:/04.wav"};
char * audio_path;

uint16_t audiobuffer[AUDIO_LEN] = {1};        //存放音乐的缓存，可以吧其中的数读取出来输入给LED做呼吸灯

//以下的信息是用于播放音乐的中间变量，
char *audiopath;                        //音乐文件的基地址,每一帧文件读取的标志
int audio_offset;                       //偏移量，下一帧在文件中的地址
WavHead audio_inf;                      //音乐文件的相关信息，可以用来显示，也可以不显示
UINT audio_br;
FIL audio_fil ;

int play_flag=0;
//用于视频显示的全局变量
int path_index=0;
FRESULT res ;


void Audio_Init(char* path){
    audiopath = path;
    res = f_open(&audio_fil,path, FA_READ);
    audio_offset = 0;
    //    打开wav文件，读取其文件头中的信息
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
    //跳过头文件的部分
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
    ES8388_Init();              //ES8388初始化
    ES8388_Set_Volume(16);      //设置耳机音量

    ES8388_I2S_Cfg(0,3);    //配置为飞利浦格式，16bit数据
    //DA mode
    ES8388_ADDA_Cfg(0,1);
    ES8388_Output_Cfg(1);
    //GPIO_WriteBit(GPIOD,GPIO_Pin_11,0);  // 控制PA8，为1设置数据输入307

    I2S2_Init_TX();

    // TIM1_Int_Init(10000/20-1,14400-1);  // 每0.1s触发一次中断，放到Video中进行初始化来控制帧率了

}

void play(void){
    TIM_Cmd(TIM1, ENABLE);
    play_flag=1;
}

void stop(void){
    TIM_Cmd(TIM1, DISABLE);
    play_flag=0;
}

// 以下是视频播放所用到的函数
// 全部改用TIM1，弃用了
void TIM8_Int_Init(u16 arr,u16 psc){

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
    NVIC_InitTypeDef         NVIC_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);       //使能TIM1时钟

    TIM_TimeBaseStructure.TIM_Period = arr;                    //指定下次更新事件时要加载到活动自动重新加载寄存器中的周期值。
    TIM_TimeBaseStructure.TIM_Prescaler =psc;                  //指定用于划分TIM时钟的预分频器值。
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分频因子
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM计数模式，向下计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);            //根据指定的参数初始化TIMx的时间基数单位

    //初始化TIM NVIC，设置中断优先级分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;         //TIM1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //设置抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //设置响应优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //使能通道1中断
    NVIC_Init(&NVIC_InitStructure);                            //初始化NVIC

    TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE ); //使能TIM1中断，允许更新中断

    /*TIM_IT_Update:更新中断，计数器向上溢出/向下溢出，计数器初始化(通过软件或者内部/外部触发)
      TIM_IT_CC1~4：都是捕获/比较中断，貌似都是平等的，即输入捕获，输出比较
      TIM_IT_Trigger：触发事件(计数器启动、停止、初始化或者由内部/外部触发计数*/

    //TIM_Cmd(TIM1, ENABLE); //TIM1使能
}
// 通过上个函数对TIM1初始化，用作视频播放所需要的计时器及其的中断，
void VideoNVICInit(char *path, int framerate){
    videopath = path;
    res = f_open(&video_fil,videopath, FA_READ);
    if(FR_OK != res)
    {
        printf("VIDEO FILE OPEN ERROR\r\n");
        printf("video address:%d\r\n",path);

    }
    //打开文件成功，初始化文件读取参数，并根据帧率初始化定时器
    else{
        //视频的播放在TIM1的中断中完成

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
    //打开文件成功，初始化文件读取参数，并根据帧率初始化定时器
    else{
        //视频的播放在TIM1的中断中完成

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
    //    打开wav文件，读取其文件头中的信息
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
    //跳过头文件的部分
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


    // TIM1_Int_Init(10000/20-1,14400-1);  // 每0.1s触发一次中断，放到Video中进行初始化来控制帧率了

}



//按键中断的初始化
/*SW1~SW4:PE0~PE3*/
//SW1：播放&暂停
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
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//SW2：上一曲
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
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//SW3：下一曲
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
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//SW4：重播
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
    EXTI_InitTypdefStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitTypdefStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitTypdefStruct);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

//外部中断初始化
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
    EXTI_INT_INIT();//外部中断初始化
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
        /* 重新挂载 */
        res_sd = f_mount(&fs,"1:",1);
    }

    LCD_Init();//LCD初始化

    VideoNVICInit(video_path, 2);

    Audio_Init(audio_path);
    play();
    // VideoPlay();

    while (1)
    {

    }

}

// TIM1中断用于音乐播放
void TIM1_UP_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void){
    audio_offset=audio_offset+AUDIO_LEN;
    res = f_lseek(&audio_fil, audio_offset);
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清除标志位
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
            //打印图片
            //printf("print\r\n");
            LCD_ShowPicture(DISPLAY_START_X,DISPLAY_START_Y,128,128,framebuffer);
            //计算出下一张图片的偏置
            videooffset += PIC_SIZE;
            //对文件基地址进行偏移
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
        //图片已经完成读取，关闭文件并终止中断
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



//SW1：播放&暂停
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
    printf("中断触发了\r\n");
    EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
}

//SW2：上一曲
void EXTI1_IRQHandler(void)__attribute__ ((interrupt("WCH-Interrupt-fast")));
void EXTI1_IRQHandler(void)
{
    stop();
    path_index=path_index-1;//path_index：0~2
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

//SW3：下一曲
void EXTI2_IRQHandler(void)__attribute__ ((interrupt("WCH-Interrupt-fast")));
void EXTI2_IRQHandler(void)
{
    stop();
    path_index++;//path_index：0~2
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

//SW4：重播
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
    int rx_mode = 0;                                                              //0：只读取数据，不判断操作的模式；1：判断操作的模式
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == 1)
    {
        if(Receive_sum > 9)                                                     //总共最多接受10个数据
        {
            USART_ReceiveString[9] = '\0';                                      //超出接受范围，终止接收
            Receive_Flag = 1;                                                   //接收标志位置1，停止接收数据
            Receive_sum = 0;                                                    //数组下标置0
            printf("usart rx out of space\r\n");
        }

        if(Receive_Flag == 0)                                                   //接收标志位等于0，开始接收数据
        {
            USART_ReceiveString[Receive_sum] = USART_ReceiveData(USART2);       //通过USART2串口接收字符
            Receive_sum++;                                                      //数组下标++
        }

        if(Receive_sum >= 2)                                                    //数组下标大于2,\r\n作为终止符
        {
            if(USART_ReceiveString[Receive_sum-2] == '\r' && USART_ReceiveString[Receive_sum-1] == '\n' )
            {
                USART_ReceiveString[Receive_sum-1] = '\0';
                USART_ReceiveString[Receive_sum-2] = '\0';
                Receive_sum = 0;                                                //数组下标置0
                printf("%s\r\n",USART_ReceiveString);
                rx_mode = 1;
            }
        }
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);                          //清空标志位
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
            path_index=path_index-1;//path_index：0~2
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
            path_index++;//path_index：0~2
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

// TIM8的中断函数实现固定帧率的视频播放（已弃用，现在全用TIM1）
/*void TIM8_UP_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM8_UP_IRQHandler(void)
{
    printf("NVIC TIM8\r\n");
    printf("frame size:%d\r\n",file_size);
    printf("frame number:%d\r\n",framenumber);
    TIM_ClearFlag(TIM8, TIM_FLAG_Update);//清除标志位
    if(framenumber < file_size){
        res = f_read(&video_fil, framebuffer, PIC_SIZE, &video_br);
        if(FR_OK != res)
            printf("TIM8 ERROR!\r\n");
        else{
            //打印图片
            printf("print\r\n");
            LCD_ShowPicture(DISPLAY_START_X,DISPLAY_START_Y,128,128,framebuffer);
            //计算出下一张图片的偏置
            videooffset += PIC_SIZE;
            //对文件基地址进行偏移
            res = f_lseek(&video_fil, videooffset);
            ++framenumber;
            if(FR_OK != res)
                printf("Lseek ERROR!\r\n");
        }
    }
    else{
        //图片已经完成读取，关闭文件并终止中断
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
