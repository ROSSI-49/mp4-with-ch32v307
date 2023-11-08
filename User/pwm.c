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

    /*TIM_OCMode_PWM1和TIM_OCMode_PWM2的区别：
    若TIM_OCInitTypeDef.TIM_OCMode = TIM_OCMode_PWM1时：
         当计时器值小于比较器设定值时则TIMX输出脚此时输出有效高电位。
         当计时器值大于或等于比较器设定值时则TIMX输出脚此时输出低电位。

    若TIM_OCInitTypeDef.TIM_OCMode = TIM_OCMode_PWM2时：
         当计时器值小于比较器设定值时则TIMX输出脚此时输出有效低电位。
         当计时器值大于或等于比较器设定值时则TIMX输出脚此时输出高电位。*/

#if (PWM_MODE == PWM_MODE1)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

#elif (PWM_MODE == PWM_MODE2)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;

#endif

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // 初始化定时器TIM的PWM输出通道1的函数，其中只有高级时钟（1，8，9，10）和通用定时器（2，3，4，5）支持PWM的输出，每个时钟提供了四个通道
    TIM_OC1Init( TIM8, &TIM_OCInitStructure );

    //使能TIM8的PWM输出
    TIM_CtrlPWMOutputs(TIM8, ENABLE );
    /*是否启用预装载的功能
    预装载功能是指在定时器的计数器值更新到比较寄存器的值时，是否立即将新的比较值加载到比较寄存器中。
    如果预加载使能，新的比较值将在下一个更新事件时加载到比较寄存器，否则在计数器值更新到比较寄存器值时直接生效。*/
    TIM_OC1PreloadConfig( TIM8, TIM_OCPreload_Disable );
    //允许或禁止在定时器工作时向ARR的缓冲器中写入新值，以便在更新事件发生时载入覆盖以前的值。
    TIM_ARRPreloadConfig( TIM8, ENABLE );
    TIM_Cmd( TIM8, ENABLE );
}
