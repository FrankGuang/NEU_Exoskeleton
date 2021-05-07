#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32_tim2.h"
#include "misc.h"

/* TIM2�ж����ȼ����� */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//�ж�����  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//������жϵ���ռ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//������жϵ���Ӧ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
}


//��ʼ��TIM2��ʱ�����ж�
//ÿ�ν����жϷ��������ʱ��Ϊ
//((1+arr )/72M)*(1+psc )=((1+1999)/72M)*(1+35999)=1��
void TIM2_Config(u16 arr,u16 psc)
{
    //��ʱ��ʱ����1s

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    //����жϱ�־λ
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//TIM_ClearFlag(TIM2,TIM_FLAG_Update);//����������ͬ

    //�Զ�װ�ص�����ֵ0-0xffff��72M/36000=2000
    TIM_TimeBaseStructure.TIM_Period = arr;
    //ʱ�ӳ���Ԥ��Ƶֵ0-oxffff��������36000��Ƶ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    ////��ͨ�͸߼�������������
    //ʱ�ӷָ��ʱΪ0���߼�Ӧ�ò���
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //����ģʽ�������£��������123
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

    //�����ж�
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    //��������
    TIM_Cmd(TIM2,ENABLE);
}

