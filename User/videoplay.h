/*
 * video.h
 *
 *  Created on: 2023��11��8��
 *      Author: 77249
 */

#ifndef USER_VIDEO_H_
#define USER_VIDEO_H_

#include "ch32v30x.h"
#include "sdio.h"
#include "ff.h"
#include "diskio.h"
#include "es8388.h"

/******************************************************����Ϊ�������漰�ı����뺯��***************************************************************/



/******************************************************����Ϊ��ʾ���漰�ı����뺯��***************************************************************/
// ����ͼƬ��ʾ�ĺ궨��
#define PIC_SIZE 32768
/*��LCD��X����*/
#define DISPLAY_START_X 0
/*��LCD��Y����*/
#define DISPLAY_START_Y 0
/*LCD��ȷֱ���*/
#define OLED_WIDTH   128
/*LCD�߶ȷֱ���*/
#define OLED_HEIGHT  128

void TIM8_Int_Init(u16 arr,u16 psc);            //ԭ��������Ƶ����֡�ʵ��жϣ�������
void VideoPlay(void);
void VideoStop(void);




/******************************************************����Ϊ��Ƶ���漰�ı����뺯��***************************************************************/

#define  AUDIO_LEN    2730 //��SRAM����16λ��ʽ������ݳ��ȡ�8K�����ʣ�˫ͨ����0.1s����3.2KB

/* WAV�ļ�ͷ��ʽ */
typedef __packed struct {

    uint32_t riff; /* = "RIFF" 0x46464952*/

    uint32_t size_8; /* ���¸���ַ��ʼ���ļ�β�����ֽ��� */

    uint32_t wave; /* = "WAVE" 0x45564157*/

    uint32_t fmt; /* = "fmt " 0x20746d66*/

    uint32_t fmtSize; /* ��һ���ṹ��Ĵ�С(һ��Ϊ16) */

    uint16_t wFormatTag; /* ���뷽ʽ,һ��Ϊ1 */

    uint16_t wChannels; /* ͨ������������Ϊ1��������Ϊ2 */

    uint32_t dwSamplesPerSec; /* ������ */

    uint32_t dwAvgBytesPerSec; /* ÿ���ֽ���(= ������ �� ÿ���������ֽ���) */

    uint16_t wBlockAlign; /* ÿ���������ֽ���(=����������/8*ͨ����) */

    uint16_t wBitsPerSample; /* ����������(ÿ��������Ҫ��bit��) */

    uint32_t data; /* = "data" 0x61746164*/

    uint32_t datasize; /* �����ݳ��� */

} WavHead;


//����Ϊ��Ƶ���õ��ĺ�����SD->SRAM->ES8388��ֻ�õ�TX����
//I2S�ĳ�ʼ������
void I2S2_Init_TX(void);
//I2S��DMA��Ӧ�ĳ�ʼ��
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);


/******************************************************����Ϊ���õı����뺯��***************************************************************/

void TIM1_Int_Init(u16 arr,u16 psc);
void stop(void);                        //��������
void play(void);




#endif /* USER_VIDEO_H_ */
