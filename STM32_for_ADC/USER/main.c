
/******************** 4���ࣩ·ADC��ʱ���ڷ��� ********************
 * �ļ���  ��main.c
 * ����    ��ͨ�����ڵ�������������ӷ������ݣ����ӽ��յ����ݺ������ش������ԡ�         
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103VET6
 * ��汾  ��ST3.0.0  			
 * �޸�ʱ�䣺2019.08.18	 
*********************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "stm32_adc.h"
#include "stm32_dac.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32_tim2.h"
#include "stm32_led.h"

int  ADC_Value[4];//�������澭��ת���õ��ĵ�ѹֵ
int Manu_value[8]={0};
//float adc_average[6] = {0};
//int  ADC_Value[6];//�������澭��ת���õ��ĵ�ѹֵ
float adc_average[4] = {0};
extern u16  AD_Value[20][4];


int force_Value[4] = {0};//�洢��







void ADC_average(void);
void ADC_ConvertSend(int* value);
void LED_GPIO_Config(void);
void TIM2_Config(u16 arr,u16 psc);

int main(void)
{ 	
	
	SystemInit();	//����ϵͳʱ��Ϊ 72M  
	USART1_Config(); //USART1 ���� 	
	ADC1_Init();
	LED_GPIO_Config();/* led �˿����� */ 
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//��ʼ�ɼ�
	
	TIM2_NVIC_Configuration(); /* TIM2 ��ʱ���� */
	TIM2_Config(719,999);//��ʱ���ж�ʱ������ 5ms     (3599+1)*(9999+1)/72000000=1s
  while (1)
  {	 
		//printf("hello the world! \r\n");
			ADC_average();
			ADC_Value[0]=0;//��ƽ��ֵ��ת���ɵ�ѹֵ
			ADC_Value[1]=(float)adc_average[1]*3300/4096;//��ƽ��ֵ��ת���ɵ�ѹֵ
			ADC_Value[2]=(float)adc_average[2]*3300/4096;//��ƽ��ֵ��ת���ɵ�ѹֵ
		  ADC_Value[3]=(float)adc_average[3]*3300/4096;
		
		  force_Value[0]=65534;
		  force_Value[1]=24*(ADC_Value[1]*1000/(3300-ADC_Value[1]));
		  force_Value[2]=24*(ADC_Value[2]*1000/(3300-ADC_Value[2]));
		  force_Value[3]=24*(ADC_Value[3]*1000/(3300-ADC_Value[3]));
		
		
		
		
		
			//ADC_Value[3]=(float)adc_average[3]*3300/4096;//��ƽ��ֵ��ת���ɵ�ѹֵ
//			ADC_Value[4]=(float)adc_average[4]*3300/4096;//��ƽ��ֵ��ת���ɵ�ѹֵ
//			ADC_Value[5]=(float)adc_average[5]*3300/4096;//��ƽ��ֵ��ת���ɵ�ѹֵ
	
		//printf("AD value = %d  %d  %d  %d   %d  %d  mV  \r\n",ADC_Value[0],ADC_Value[1],ADC_Value[2],ADC_Value[3],ADC_Value[4],ADC_Value[5]);
		//printf("AD value = %d  %d  %d  %d  mV  \r\n",ADC_Value[0],ADC_Value[1],ADC_Value[2],ADC_Value[3]);
  }
	
}

void ADC_average(void)
{
		int sum = 0;
		u8 i,j;
		for(i=0;i<4;i++)
	{
		for(j=0;j<10;j++)
		{
			sum+=AD_Value[j][i];
		}			
		adc_average[i] = sum/10;
			sum = 0;
	}
}

 void ADC_ConvertSend(int * value)//�������ݰ�+ 2����ͷ
 {
	  unsigned int temp[8];
	  unsigned short i=0,j=0;  

	 
		for(i=0;i<8;i++)  
		{  
			temp[i] = value[i];//floatתBYTE
		}  
		
		for( j=0;j<8;j++)
	  {
			UART1_Send_byte(USART1,temp[j]);
	  }
 }
void TIM2_IRQHandler(void)
{
    //�ж�TIM3�����ж��Ƿ���
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
    {
        //���������־λ
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
			//״̬ȡ��
    //  GPIO_WriteBit(GPIOB,GPIO_Pin_13,(BitAction)(!GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_13)));
		//	GPIO_WriteBit(GPIOB,GPIO_Pin_12,(BitAction)(!GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_12)));//c8t6��
		//	GPIO_WriteBit(GPIOB,GPIO_Pin_14,(BitAction)(!GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_14)));
			
			Manu_value[0]=force_Value[0]%256;// Yu
			Manu_value[1]=force_Value[0]/256;// zheng
			Manu_value[2]=force_Value[1]%256;
			Manu_value[3]=force_Value[1]/256;
			Manu_value[4]=force_Value[2]%256;
			Manu_value[5]=force_Value[2]/256;
			Manu_value[6]=force_Value[3]%256;
			Manu_value[7]=force_Value[3]/256;
//			adc =(Manu_value[3] * 256 + Manu_value[2]);
//			printf("AD value = %d  %d   %d    %d\r\n",adc,ADC_Value[1],Manu_value[2] ,Manu_value[3] );
			ADC_ConvertSend(Manu_value);//���ڷ���
			//printf("AD value = %d  %d  %d mV  \r\n",ADC_Value[0],ADC_Value[1],ADC_Value[2]);
    }

}


