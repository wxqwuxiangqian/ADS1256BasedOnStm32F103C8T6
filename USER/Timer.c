
#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include "Timer.h" 
#include "misc.h"
#include "bit.h"

/*
ͨ�ö�ʱ��3�жϳ�ʼ��
arr���Զ���װ��ֵ
psc��ʱ��Ԥ��Ƶϵ��
*/
void TIM3_Int_Init(u16 arr,u16 psc)//��ʱ��ʽ��T=(arr+1)*(psc+1)/72M
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��


    TIM_TimeBaseStructure.TIM_Period = arr; //�Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //ʱ��Ԥ��Ƶϵ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 


    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx                     
}

//��ʱ��3�жϺ���
void TIM3_IRQHandler(void)   
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
    {
//    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  
//		PCOUT(13)=!	PCOUT(13);
    }
}
