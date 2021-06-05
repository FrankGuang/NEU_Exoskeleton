#include "stm32f10x.h"
#include "stm32_dac.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dac.h"
/*DAC��� = Vref x (DOR/4095)*/ 
//DAC������ͨ����������ʹ��
//��ͬ����Դ/��ͬ����Դ
//ͬʱ����/��������    DAC_DualSoftwareTriggerCmd�����������ͬʱ����
//ʹ�ò��η�����/��ʹ�ò��η�����
//ʹ�����ǲ�������/ʹ������������/��ʹ�ò��η�����
//������ͬDAC_LFSRUnmask_TriangleAmplitude��ֵ/���ò���ͬDAC_LFSRUnmask_TriangleAmplitude��ֵ
//�ȵ����ϸ����������������ϣ�����Ӱ�졣
void DAC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	DAC_InitTypeDef DAC_InitStruct;
	//��һ��  ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	//�ڶ���  ���ò���
	/*һ��ʹ��DACxͨ������Ӧ��GPIO���žͻ��Զ���DAC��ģ�����������Ϊ�˱�������ĸ��źͶ���Ĺ��ģ�����PA4/PA5��֮ǰӦ�����óɡ�ģ�����롱
		ע���ǡ�ģ�����롰����ΪSTM32��û��ģ�������������ȻPA4 PA5�����ģ���źţ�Ҳֻ�����ó�GPIO_Mode_AIN*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_4 | GPIO_Pin_5)	;//PA.4  PA.5�����	�����������𿹸��ŵ�����
	
//	/*DAC ͨ��1  PA4 ��������*/
//	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
//	DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;//DAC_Trigger_T6_TRGO;
//	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//����������������������迹�������ⲿ�˷ż���ֱ�������ⲿ����
//	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits10_0;//ÿ�δ�������һ��LSFR�㷨�������õ���ֵ�ټ���DAC_DHRx����ֵ��ȥ�����λ��д��DAC_DORx�Ĵ���������ض��ĵ�ѹ
//	DAC_Init(DAC_Channel_1,&DAC_InitStruct);//����LSFR�㷨��λ����DAC_LFSRUnmask_TriangleAmplitude��ȷ����DAC_LFSRUnmask_Bits10_0��ֵ��ʾ��10λ����LSFR����
	
	/*DAC ͨ��1  PA4 ��ͨ��ģת��*/
	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;//�رղ��η�����
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;//DAC_Trigger_Software/DAC_Trigger_Ext_IT9
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//����������������������迹�������ⲿ�˷ż���ֱ�������ⲿ����
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//�ò���������/���ǲ���������أ���ͨDACת��������Ϊ0����
	DAC_Init(DAC_Channel_1,&DAC_InitStruct);																 
 
	/*DAC ͨ��2  PA5 ��ͨ��ģת��*/
	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;//�رղ��η�����
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;//DAC_Trigger_Software/DAC_Trigger_Ext_IT9
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//����������������������迹�������ⲿ�˷ż���ֱ�������ⲿ����
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//�ò���������/���ǲ���������أ���ͨDACת��������Ϊ0����
	DAC_Init(DAC_Channel_2,&DAC_InitStruct);	

	/*DAC ͨ��2  PA5 �������ǲ�*/
//	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
//	DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;
//	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
//	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;//�ڲ������ǲ�������ÿ�δ���ʱ��֮���ۼ�1���ü�������ֵ��DAC_DHRx����ֵ��ӣ�ȥ�����λ��д��DAC_DORx�Ĵ����������ѹ
//	DAC_Init(DAC_Channel_2,&DAC_InitStruct);//���ǲ������������ֵ��DAC_LFSRUnmask_TriangleAmplitude��ȷ�������������ﵽ������ֵ��Ȼ�����ǲ���������ʼ�ݼ�

	
	//������  ʹ������
	//DAC_SetDualChannelData(DAC_Align_12b_R,4095,0);�ȼ���DAC_SetChannel1Data(DAC_Align_12b_R, 4095); DAC_SetChannel2Data(DAC_Align_12b_R, 0);  
	/*DAC ͨ��1  PA4 ʹ��*/
	//DAC_SetChannel1Data(DAC_Align_12b_R, 2048);  //12λ�Ҷ������ݸ�ʽ����DACֵ  ����ֵ���Ϊ4095�����ó�4096�������DORx��Ϊ0
	DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DAC1
	
	/*DAC ͨ��2  PA5 ʹ��*/
	DAC_Cmd(DAC_Channel_2, ENABLE);  //ʹ��DAC2
//	DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
}
 
 
//������ʱ��
void TIM6_Configuration()
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		//��һ��  ʹ��ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
 
		//�ڶ��� ���ò���  ��10-1+1��*��72-1+1)/72000000=0.00001 �� 10e-5
		TIM_TimeBaseInitStruct.TIM_Period = 10 -1;
		TIM_TimeBaseInitStruct.TIM_Prescaler = 71;
		TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);//  TIMx->EGR.UG   
 
	
		/*TIM6,7�������3�����͵�TRGO�ź�
			#define TIM_TRGOSource_Reset               ((uint16_t)0x0000) //��λ UG
			#define TIM_TRGOSource_Enable              ((uint16_t)0x0010) //ʹ�� CEN
			#define TIM_TRGOSource_Update              ((uint16_t)0x0020) //�����¼�
		*/
		
		TIM_SelectOutputTrigger(TIM6,TIM_TRGOSource_Update);//�������TRGO�ź�  ����TRGO�źž��Ƕ�ʱ����������ĸ����ź�
		
		//������  ʹ������
		TIM_Cmd(TIM6,ENABLE);//CEN  λ
}

//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	float temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}


