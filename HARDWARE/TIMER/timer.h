#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
extern u16 pulse_min;
extern u16 pulse_count;
extern u16 pulse_num;
extern u8 speed_i;
extern u8 pulse_step;
extern u16 pulse_current[8];
extern u16 speed_min;
extern u16 arr_p[12];//��Ƶ���йص�ϵ��
extern float speed_now;
extern float Acc;
extern u16 speed;
extern u8 speed_lock;
extern u16 Pm;
extern u16 run_num;
extern u8 command[100];
extern u16 command_edit_num[100];
extern u16 p1;
extern u8 end;
extern u8 time_state;//���ö�ʱ��״ֵ̬
typedef struct
{
	u8  Direction;
	u32 Pulse_Num;//�������
	u16 Pulse_Rate;//����Ƶ��
	u16 Speed_Acc;//����Ӽ��ٶ�
}Set_Weiyi;
extern Set_Weiyi Weiyi[100];
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif
