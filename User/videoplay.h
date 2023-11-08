/*
 * video.h
 *
 *  Created on: 2023年11月8日
 *      Author: 77249
 */

#ifndef USER_VIDEO_H_
#define USER_VIDEO_H_

#include "ch32v30x.h"
#include "sdio.h"
#include "ff.h"
#include "diskio.h"
#include "es8388.h"

/******************************************************以下为挂载所涉及的变量与函数***************************************************************/



/******************************************************以下为显示所涉及的变量与函数***************************************************************/
// 用于图片显示的宏定义
#define PIC_SIZE 32768
/*在LCD的X坐标*/
#define DISPLAY_START_X 0
/*在LCD的Y坐标*/
#define DISPLAY_START_Y 0
/*LCD宽度分辨率*/
#define OLED_WIDTH   128
/*LCD高度分辨率*/
#define OLED_HEIGHT  128

void TIM8_Int_Init(u16 arr,u16 psc);            //原本用来视频控制帧率的中断，已弃用
void VideoPlay(void);
void VideoStop(void);




/******************************************************以下为音频所涉及的变量与函数***************************************************************/

#define  AUDIO_LEN    2730 //在SRAM中以16位形式存放数据长度。8K采样率，双通道，0.1s数据3.2KB

/* WAV文件头格式 */
typedef __packed struct {

    uint32_t riff; /* = "RIFF" 0x46464952*/

    uint32_t size_8; /* 从下个地址开始到文件尾的总字节数 */

    uint32_t wave; /* = "WAVE" 0x45564157*/

    uint32_t fmt; /* = "fmt " 0x20746d66*/

    uint32_t fmtSize; /* 下一个结构体的大小(一般为16) */

    uint16_t wFormatTag; /* 编码方式,一般为1 */

    uint16_t wChannels; /* 通道数，单声道为1，立体声为2 */

    uint32_t dwSamplesPerSec; /* 采样率 */

    uint32_t dwAvgBytesPerSec; /* 每秒字节数(= 采样率 × 每个采样点字节数) */

    uint16_t wBlockAlign; /* 每个采样点字节数(=量化比特数/8*通道数) */

    uint16_t wBitsPerSample; /* 量化比特数(每个采样需要的bit数) */

    uint32_t data; /* = "data" 0x61746164*/

    uint32_t datasize; /* 纯数据长度 */

} WavHead;


//以下为音频所用到的函数：SD->SRAM->ES8388：只用到TX部分
//I2S的初始化函数
void I2S2_Init_TX(void);
//I2S的DMA对应的初始化
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);


/******************************************************以下为公用的变量与函数***************************************************************/

void TIM1_Int_Init(u16 arr,u16 psc);
void stop(void);                        //字如其名
void play(void);




#endif /* USER_VIDEO_H_ */
