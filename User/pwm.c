#include "pwm.h"

void TIM8_PWMOut_Init( u16 arr, u16 psc, u16 ccp )
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM8, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM8, &TIM_TimeBaseInitStructure);

    /*TIM_OCMode_PWM1��TIM_OCMode_PWM2������
    ��TIM_OCInitTypeDef.TIM_OCMode = TIM_OCMode_PWM1ʱ��
         ����ʱ��ֵС�ڱȽ����趨ֵʱ��TIMX����Ŵ�ʱ�����Ч�ߵ�λ��
         ����ʱ��ֵ���ڻ���ڱȽ����趨ֵʱ��TIMX����Ŵ�ʱ����͵�λ��

    ��TIM_OCInitTypeDef.TIM_OCMode = TIM_OCMode_PWM2ʱ��
         ����ʱ��ֵС�ڱȽ����趨ֵʱ��TIMX����Ŵ�ʱ�����Ч�͵�λ��
         ����ʱ��ֵ���ڻ���ڱȽ����趨ֵʱ��TIMX����Ŵ�ʱ����ߵ�λ��*/

#if (PWM_MODE == PWM_MODE1)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

#elif (PWM_MODE == PWM_MODE2)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;

#endif

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // ��ʼ����ʱ��TIM��PWM���ͨ��1�ĺ���������ֻ�и߼�ʱ�ӣ�1��8��9��10����ͨ�ö�ʱ����2��3��4��5��֧��PWM�������ÿ��ʱ���ṩ���ĸ�ͨ��
    TIM_OC1Init( TIM8, &TIM_OCInitStructure );

    //ʹ��TIM8��PWM���
    TIM_CtrlPWMOutputs(TIM8, ENABLE );
    /*�Ƿ�����Ԥװ�صĹ���
    Ԥװ�ع�����ָ�ڶ�ʱ���ļ�����ֵ���µ��ȽϼĴ�����ֵʱ���Ƿ��������µıȽ�ֵ���ص��ȽϼĴ����С�
    ���Ԥ����ʹ�ܣ��µıȽ�ֵ������һ�������¼�ʱ���ص��ȽϼĴ����������ڼ�����ֵ���µ��ȽϼĴ���ֵʱֱ����Ч��*/
    TIM_OC1PreloadConfig( TIM8, TIM_OCPreload_Disable );
    //������ֹ�ڶ�ʱ������ʱ��ARR�Ļ�������д����ֵ���Ա��ڸ����¼�����ʱ���븲����ǰ��ֵ��
    TIM_ARRPreloadConfig( TIM8, ENABLE );
    TIM_Cmd( TIM8, ENABLE );
}
