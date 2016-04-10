#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"	  
#include "timer.h"
#include "InputCmd.h"
#include "stmflash.h"
#include "image2lcd.h"
#include "image.h"
#include "show.h"
#include "w5500.h"

#define lcd_key_1 1  //������̰�����Ӧ��ֵ
#define lcd_key_2 2
#define lcd_key_3 3
#define lcd_key_4 4


#define Auto_Mode 0
#define Hand_Mode 1
#define ScanOK    1
#define ScanError 0
u8 Mode;
u8 k;
/*****************************************************/
/*stm32 �ڲ�FLASH************************************/
 u8 TEXT_Buffer[3]={1,2,3};
#define SIZE 100	 			  	//���鳤��
#define FLASH_SAVE_ADDR  0X08070000 				//����FLASH �����ַ(����Ϊż��)
#define FLASH_SAVE_EDIT_ADDR (FLASH_SAVE_ADDR+0X00000100)
#define FLASH_SAVE_WEIYI_ADDR (FLASH_SAVE_ADDR+0X00000300)
void read_edit_num(void);

 


/****************************************************/
/*LCDģ��������趨*/
void beepms(u16 va);  
void refshow(void);//ˢ����ʾ	
void start_init(void);//���������ʼ��
void pwm_speed(u16 acc_time,u16 speed_max,u16 pulse_num_1);




/************************************************************/
/*�༭����******/
void set_weiyi_init(u8 statement);
void set_weiyi(u8 statement);
void edit_init(void);//��������
void jia_service(void);//�Ӱ���������
void jian_service(void);//��������
void Move_LR_Service(void);//�����ư���������
void Short_Line_flicker(void);//������˸����������ʾ�ı�ĳ����
void input_command(void);
void display_command(void);//��ʾ����
void set_system(void);
void set_system_2(void);
void set_system_3(void);
void save_command(void);//��������
void read_command(void);//��ȡ����
u8 init_lock=1;//����ֻ��ʼ��һ�����
u8 uc_lock=1;//��ֹ�����ظ�����
u8 state=1;
u8 command_num=0;//��ǰָ���ţ���������ǰ�ڼ���ָ��,��0��ʼ
u8 command[100];//ָ����룬0Ϊ�գ�1Ϊλ��,2Ϊʱ�䣬3Ϊ�ٶȣ�4Ϊ���룬5Ϊ�����6Ϊɨ�裬7Ϊ��ת��8Ϊѭ����9Ϊ������20Ϊѭ���յ㣬21Ϊѭ������
u8 save_ok=0;//�����Ϊ1���ٴα༭Ϊ0
u8 edit_statement=0;//�༭״̬��0Ϊ�༭ָ�1Ϊ�༭ָ���������2Ϊ�༭λ��ָ��






Set_Weiyi Weiyi[100];//�ɹ�����100��λ��
typedef struct
{
	u8 ge;
	u8 shi;
	u8 bai;
	u8 qian;
	u8 wan;
	u8 shiwan;

}Num_Wei;
typedef struct
{
	u8 ge;
	u8 shi;
	u8 bai;
	u8 qian;
}Num_4_Wei;//���ڱ༭4λ��ָ�������
typedef struct
{
	Num_Wei Pulse_Num; 
	Num_Wei Pulse_Rate; 
	Num_Wei Pulse_Acc;
}Dis_Weiyi;//������ʾλ��







Dis_Weiyi Weiyi_Dis[100];//��ʾ100��λ��
Num_Wei Set_Time[20];
u8 command_edit_num_wei[100][5];//���ڱ༭4λ��ָ�������,��ֳ�λ����һ����ָ�����ڶ�λΪ1~4��0����
u16 command_edit_num[100];//���Ա༭������������һ�е�4λ�ϲ������ɵõ���ǰ���������
u16 Pinpoint_Num=1;//���ö�λ���֣����ڱ༭λ������,��Ч����Ϊ0~17
u16 Pinpoint_Edit=0;//���ڱ༭�������֣���Ч����Ϊ0,1,2,3,4
u32 flicker_count;


u8 page=1;
u8 page_max;
u8 command_max;//��Ч����ĸ���
u8 ScanCommandNum;
u8 ScanCommand[10];



u16 run_num;
u8 end;//����ָ���������

void Run(void);
void Run_WeiYi(u8 dir,u16 num);
void Run_Speed(u16 num);
void Run_Time(u16 num);
void Run_Input(u16 Xi,u16 Xstate);
void Run_Output(u16 led ,u16 Xstate);
void Run_Scan(void);
void Run_Return(u16 num,u16 *p);
void Run_End(u16 startnum,u16 endnum,u16 num,u16 *p);
int Scan_GPIO(u16 Xi,u16 Xstate);
void Run_Scan_Service(u16 startnum,u16 endnum);
/*************************************************************************/
/***************************************************/


u16 speed;
float speed_now;
u16 Pm;//�м�λ�Ƶ�
float Acc;
u8 speed_lock=1;






/****************************************************/
/*����ģ��������趨*/
u8 speed_current;//�ٶȵĵ�ǰֵ
u16 speed_min;//�ٶ���Сֵ������СƵ��
u16 arr_p[12];//��Ƶ���йص�ϵ��
u8 speed_i;
u16 pulse_count;
float mul_pulse;
float mul_speed;
u16 pulse_min;
u16 pulse_num;
u16 speed_base[8]={500,1450,2300,3050,3700,
										4250,4650,4900};
u16 pulse_current_base[8]={200,361,729,1156,1600,
	                         2025,2304,2500
													     };
u16 pulse_current[8];
u8 pulse_step;
u16 arr_test;

 /********************ϵͳ����*************************************/
 u8 Select_Pro=1;//ѡ�����
Num_Wei Pulse_Dangliang;	//һȦ����
u32     PerPulseNum;
Num_Wei Distance_Per;					//ÿȦ����		
u32     PerDistance;															 
Num_Wei Dot_Pulse;	
Num_Wei Constant_Speed;															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
															 
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ 
}
void xianshi()//��ʾ��Ϣ
{    
	u8 idstr[5];
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;	
	LCD_ShowString(0,2,200,16,16,"ID:");	    
	inttohex(lcddev.id,idstr);	  
	LCD_ShowString(100,2,200,16,16,idstr);
	//��ʾ32*32����
	showhanzi32(0,20,0);	 //��
	showhanzi32(40,20,1);	 //��
	showhanzi32(80,20,2);    //��
	//��ʾ16*16����
	showhanzi16(0,55,0);	  //ר
	showhanzi16(20,55,1);	  //ע
	showhanzi16(40,55,2);	  //��
	showhanzi16(60,55,3);	  //ʾ
	showhanzi16(80,55,4);	  //��
	showhanzi16(100,55,5);	  //��
}
void showqq() //��ʾȫ��QQ
{ 

	u16 x,y; 
	x=0;
	y=100;
	while(y<lcddev.height-39)
	{
		x=0;
		while(x<lcddev.width-39)
		{
			showimage(x,y);	
			x+=40;
		}
		y+=40;
	 }

}
void refshow(void)	 //ˢ����ʾ
{
	switch(state)
	{
		case 0:
		LCD_Clear(WHITE);
	    xianshi();
    	showqq();
		break;
		case 1:
		LCD_Clear(BLACK); 
		break;
		case 2:
		LCD_Clear(RED);
		break;
		case 3:
		LCD_Clear(GREEN);
		break;
		case 4:
		LCD_Clear(BLUE);
		break;
	}	
}
void jiance(u8 key) //��ⰴ��
{

	if(key==1)	//KEY_RIGHT����,��ִ��У׼����
	{
		LCD_Clear(WHITE);//����
	    TP_Adjust();  //��ĻУ׼ 
		TP_Save_Adjdata();	 
		Load_Drow_Dialog();
	}
	if(key==2)
	{
		state++;
		 if(state==5)
		 {
		 	state=0;
			LCD_Init();	   
		 }
		refshow();//ˢ����ʾ
	}

}	 
void beepms(u16 va)
{
	beep=1;
	delay_ms(va);
	beep=0;
}					
int main(void)
 {	
   int key,i;	
u8 datatemp[SIZE];	 
	//u16 i=0;	 
	//u8 key=0;	
  	//����IC��ӿ�
	/*ID����˵��:Ŀǰ�Ѿ��������������£���ֻ�������г���IC��֧�֣�û���г��Ĳ�֧�֣�������᲻���ڸ����Ա�֧�ָ����IC��
	2.4�� S6d1121    lcddev.id=0x1121
	2.4��9325/9328   lcddev.id=0x9325
	2.8��9325/9328   lcddev.id=0x9325
	3.2��1289        lcddev.id=0x8989
	3.2��8347        lcddev.id=0x0047
	3.2��8352A       lcddev.id=0x0052
	3.2��8352B       lcddev.id=0x0065
	3.2��9320        lcddev.id=0x9320
	1963����4.3��    lcddev.id=0x1943
	��ҵ��4.3��      lcddev.id=0x0043
	1963����5��      lcddev.id=0x1905
	��ҵ��5��        lcddev.id=0x0050
	1963����7��      lcddev.id=0x1907
	��ҵ��7��        lcddev.id=0x0070
	��ҵ��9��        lcddev.id=0x0090  
	*/
	lcddev.bus16=1;//���߽ӿڣ�0-8λ���ߣ�1-16λ���ߣ��޸�8λ����֮ǰ��ȷ������������Ƿ���8λ�ӿڣ�Ŀǰֻ��2.4��2.8�����֧��8λ�İ汾��3.2�����Ͼ���֧��8λ���� 
	lcddev.id=0x0043;//ָ��ID,0x1234Ϊ�Զ���ID,�м�ע�ⲻ�����е������ܶ���ID,ֻ���ٲ�������IC�ܶ�ID,����4.3���Լ�4.3�����ϵĶ����ܶ�ID.


	delay_init();	    	 //��ʱ������ʼ��	  
	//TIM3_Int_Init(399,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms 
	//100Hz�ĳ�ʼƵ�ʵ�arrΪ71ʱΪ�������Ƶ��10khz
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();			     //LED�˿ڳ�ʼ��
	GPIO_Input_Init();
   SPI_Flash_Init();//��ʼ��Flash;
	 	System_Initialization();	//STM32ϵͳ��ʼ������(��ʼ��STM32ʱ�Ӽ�����)
	Load_Net_Parameters();		//װ���������	
	W5500_Hardware_Reset();		//Ӳ����λW5500
	W5500_Initialization();		//W5500��ʼ������
	LCD_Init();	
	tp_dev.init();//������ʼ��
	KEY_Init();
	TIM3_PWM_Init(7199,99);
	//TIM_SetCompare3(TIM3,3000);
 	POINT_COLOR=BLACK;//��������Ϊ��ɫ 
	 BACK_COLOR=BACK_COL;
	 LCD_Clear(TOP_0);
	//
	 start_init();
		edit_init();

		

	while(1)
	{  
		W5500_Socket_Set();//W5500�˿ڳ�ʼ������

		W5500_Interrupt_Process();//W5500�жϴ��������

		if((S0_Data & S_RECEIVE) == S_RECEIVE)//���Socket0���յ�����
		{
			S0_Data&=~S_RECEIVE;
			Process_Socket_Data(0);//W5500���ղ����ͽ��յ�������
		}
		
    switch (edit_statement)
		{
			case 0: input_command();
							break;
			case 1:input_command();
							break;
			case 2: set_weiyi(state);
							break;
			case 3: Run();
							break;
			case 4: set_system();
							break;
			case 5: set_system_2();
			        break;
			case 6: set_system_3();
							break;
			case 7: Run_Scan();
							break;
							
		}
		i++; 
	}
	
}

/***************************************************************************************
������ʱ���Ϊ10�ݣ���Ƶ�ʵ���СֵΪ���ֵ��1/20
��һ�β����ܵ��������Ǽ��ٲ��ֵ�1/100,10������ֱ�Ϊ1��3��5.����������19
******************************************************************************************/
void pwm_speed(u16 acc_time,u16 speed_max,u16 pulse_num_1)//����ʱ�䡢�ٶ����ֵ���������
{
   	 pulse_num=pulse_num_1;
	   Pm= pulse_num/2;//�����е�ֵ
	  speed=speed_max;
	  speed_now=speed_max*0.1;
	  TIM3->ARR=720000/speed_now-1;
	   Acc=speed_max/acc_time;
	  TIM_Cmd(TIM3, ENABLE);
}
void start_init()
{
	u8 i,j;
	LCD_Clear(BACK_COL);
	BACK_COLOR=BACK_COL;
	
	for(i=1;i<=5;i++)//��ʾ������
	{
		LCD_ShowNum_32(10,50*i-30,0);
		LCD_ShowNum_32(26,50*i-30,i-1);
	}
	POINT_COLOR=LIGHTBLUE ;
	for(i=1;i<=5;i++)
	{
		for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
		{
			 LCD_DrawLine(60,50*i-30+32+j, 140,50*i-30+32+j);
			 LCD_DrawLine(150,50*i-30+32+j, 280,50*i-30+32+j);
		}
	}
	BACK_COLOR=WHITE;
}
void set_weiyi(u8 statement)//����λ�ƽ���
{
   u16 i;
	if(init_lock)//�л�״̬ʱ��ʼ��һ��
	{
		set_weiyi_init(statement);
		edit_init();
		init_lock=0;
		flicker_count=0;
	}
	if(Weiyi[statement].Direction==0)//������
	{
		showhanzi32(254,20,93);
	}
	else 
	{
		showhanzi32(254,20,94);
	}
	LCD_ShowNum_32(180,80,Weiyi_Dis[statement].Pulse_Num.shiwan);//���������ֵ
	LCD_ShowNum_32(198,80,Weiyi_Dis[statement].Pulse_Num.wan);
	LCD_ShowNum_32(216,80,Weiyi_Dis[statement].Pulse_Num.qian);
	LCD_ShowNum_32(234,80,Weiyi_Dis[statement].Pulse_Num.bai);
	LCD_ShowNum_32(252,80,Weiyi_Dis[statement].Pulse_Num.shi);
	LCD_ShowNum_32(270,80,Weiyi_Dis[statement].Pulse_Num.ge);
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
	LCD_ShowNum_32(198,140,Weiyi_Dis[statement].Pulse_Rate.wan);//����Ƶ����ֵ
	LCD_ShowNum_32(216,140,Weiyi_Dis[statement].Pulse_Rate.qian);
	LCD_ShowNum_32(234,140,Weiyi_Dis[statement].Pulse_Rate.bai);
	LCD_ShowNum_32(252,140,Weiyi_Dis[statement].Pulse_Rate.shi);
	LCD_ShowNum_32(270,140,Weiyi_Dis[statement].Pulse_Rate.ge);
	
	LCD_ShowNum_32(198,200,Weiyi_Dis[statement].Pulse_Acc.wan);//�����ٶ���ֵ
	LCD_ShowNum_32(216,200,Weiyi_Dis[statement].Pulse_Acc.qian);
	LCD_ShowNum_32(234,200,Weiyi_Dis[statement].Pulse_Acc.bai);
	LCD_ShowNum_32(252,200,Weiyi_Dis[statement].Pulse_Acc.shi);
	LCD_ShowNum_32(270,200,Weiyi_Dis[statement].Pulse_Acc.ge);
	tp_dev.scan(0);
	flicker_count++;

	Short_Line_flicker();

	
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{	
		delay_ms(10);
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			if(uc_lock==1)//��ֹ�ظ�����
			{
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>10&&tp_dev.y<65)//��ҳ��
				{
//					Pinpoint_Num=1;
//				  state++;
//					if(state==17)
//					{
//						state=1;
//					}
//					uc_lock=0;
//					init_lock=1;
    
				}
		
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>10&&tp_dev.y<65)//���ؼ�
				{
					edit_statement=0;
					Pinpoint_Num=1;
					init_lock=1;
					uc_lock=0;
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num--;
					if(Pinpoint_Num==0)
					{
						Pinpoint_Num=17;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num++;
					if(Pinpoint_Num==18)
					{
						Pinpoint_Num=1;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>130&&tp_dev.y<185)//�Ӽ�
				{
					uc_lock=0;
				 jia_service();
					
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>130&&tp_dev.y<185)//����
				{
					uc_lock=0;
				 jian_service();
					
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>190&&tp_dev.y<245)//ֹͣ��
				{
					  end=1;
						pulse_count=0;
						p1=0;
						speed_lock=1;
						TIM_Cmd(TIM3, DISABLE);
					   edit_statement=2;
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>190&&tp_dev.y<245)//����
				{
					run_num=0;
					for(i=0;i<=99;i++)
					{
						if(command[i]==1)
						{
							Weiyi[i].Pulse_Num=Weiyi_Dis[i].Pulse_Num.shiwan*100000
														+Weiyi_Dis[i].Pulse_Num.wan*10000
			                      +Weiyi_Dis[i].Pulse_Num.qian*1000
														+Weiyi_Dis[i].Pulse_Num.bai*100
														+Weiyi_Dis[i].Pulse_Num.shi*10
														+Weiyi_Dis[i].Pulse_Num.ge;//�õ�����λ�ƾ���
							Weiyi[i].Pulse_Num=Weiyi[i].Pulse_Num*PerPulseNum/PerDistance;//�õ��������
				
				
							Weiyi[i].Pulse_Rate=Weiyi_Dis[i].Pulse_Rate.wan*10000
			                      +Weiyi_Dis[i].Pulse_Rate.qian*1000
														+Weiyi_Dis[i].Pulse_Rate.bai*100
														+Weiyi_Dis[i].Pulse_Rate.shi*10
														+Weiyi_Dis[i].Pulse_Rate.ge;
							Weiyi[i].Speed_Acc=Weiyi_Dis[i].Pulse_Acc.wan*10000
			                      +Weiyi_Dis[i].Pulse_Acc.qian*1000
														+Weiyi_Dis[i].Pulse_Acc.bai*100
														+Weiyi_Dis[i].Pulse_Acc.shi*10
														+Weiyi_Dis[i].Pulse_Acc.ge;
						}
					}
					edit_statement=3;
					uc_lock=0;
				}
			}
			
		}
	}
	else
	{
		uc_lock=1;
	}
	
}

void set_weiyi_init(u8 statement)//�����ʼ��
{
	//u8 ge;
	//u8 shi;
	//ge=statement%10;
	//shi=statement/10;
	LCD_Clear(WHITE);
//	showhanzi32(0,20,3);                                //��
//	showhanzi32(36,20,4);                               //��
//	LCD_ShowNum_32(72,20,shi);                            //0
//	LCD_ShowNum_32(90,20,ge);                            //1
//	showhanzi32(108,20,5);                              //��
//	showhanzi32(144,20,6);                              //λ
//	showhanzi32(180,20,7);                              //��
	
	showhanzi32(0,20,6);                                //λ
	showhanzi32(36,20,7);                               //��
	showhanzi32(72,20,91);                              //��
	showhanzi32(108,20,92);                              //��

	
	showhanzi32(0,80,6);                                //λ
	showhanzi32(36,80,7);                               //��
  showhanzi32(72,80,73);                              //��
	showhanzi32(108,80,74);                             //��
	
	showhanzi32(0,140,8);                               //��
	showhanzi32(36,140,9);                              //��
  showhanzi32(72,140,12);                             //Ƶ
	showhanzi32(108,140,13);                            //��
	
	showhanzi32(0,200,14);                              //��
	showhanzi32(36,200,15);                             //��
  showhanzi32(72,200,16);                             //��
	showhanzi32(108,200,32);                            //ʱ
	showhanzi32(144,200,33);                            //��

	LCD_ShowNum_32(180,80,Weiyi_Dis[statement].Pulse_Num.shiwan);//���������ֵ
	LCD_ShowNum_32(198,80,Weiyi_Dis[statement].Pulse_Num.wan);
	LCD_ShowNum_32(216,80,Weiyi_Dis[statement].Pulse_Num.qian);
	LCD_ShowNum_32(234,80,Weiyi_Dis[statement].Pulse_Num.bai);
	LCD_ShowNum_32(252,80,Weiyi_Dis[statement].Pulse_Num.shi);
	LCD_ShowNum_32(270,80,Weiyi_Dis[statement].Pulse_Num.ge);
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
	LCD_ShowNum_32(198,140,Weiyi_Dis[statement].Pulse_Rate.wan);//����Ƶ����ֵ
	LCD_ShowNum_32(216,140,Weiyi_Dis[statement].Pulse_Rate.qian);
	LCD_ShowNum_32(234,140,Weiyi_Dis[statement].Pulse_Rate.bai);
	LCD_ShowNum_32(252,140,Weiyi_Dis[statement].Pulse_Rate.shi);
	LCD_ShowNum_32(270,140,Weiyi_Dis[statement].Pulse_Rate.ge);
	
	LCD_ShowNum_32(198,200,Weiyi_Dis[statement].Pulse_Acc.wan);//�����ٶ���ֵ
	LCD_ShowNum_32(216,200,Weiyi_Dis[statement].Pulse_Acc.qian);
	LCD_ShowNum_32(234,200,Weiyi_Dis[statement].Pulse_Acc.bai);
	LCD_ShowNum_32(252,200,Weiyi_Dis[statement].Pulse_Acc.shi);
	LCD_ShowNum_32(270,200,Weiyi_Dis[statement].Pulse_Acc.ge);
	tp_dev.scan(0);
}


void edit_init(void)//�༭���漴��������
{
	if(edit_statement==0||edit_statement==1)
	{
		show_buttom(290,15,80,32);
		showhanzi32(305,20,18);//��ҳ
		showhanzi32(340,20,19);
		
		//LCD_DrawRectangle(390,15 ,470 ,61 );
		show_buttom(380,15,80,32);
		showhanzi32(395,20,3);//����
		showhanzi32(430,20,62);
		
		//LCD_DrawRectangle(300,65 ,380 ,111 );
		show_buttom(290,65,80,32);
		showhanzi32(305,70,22);//����
		showhanzi32(340,70,23);
		
		//LCD_DrawRectangle(390,65 ,470 ,111 );
		show_buttom(380,65,80,32);
		showhanzi32(395,70,24);//����
		showhanzi32(430,70,25);
		
		//LCD_DrawRectangle(300,115 ,380 ,161 );
		show_buttom(290,115,80,32);
		showhanzi32(322,120,26);//��
	 
		
		//LCD_DrawRectangle(390,115 ,470 ,161 );
		show_buttom(380,115,80,32);
		showhanzi32(412,120,27);//��
		
		
		//LCD_DrawRectangle(300,165 ,380 ,211 );
		show_buttom(290,165,80,32);
		showhanzi32(305,170,47);//ɾ��
		showhanzi32(340,170,48);
		
		//LCD_DrawRectangle(390,165 ,470 ,211 );
		show_buttom(380,165,80,32);
		showhanzi32(395,170,53);//���
		showhanzi32(430,170,26);
		
	// LCD_DrawRectangle(300,215 ,380 ,261 );
	show_buttom(290,215,80,32);
		showhanzi32(305,220,77);//����
		showhanzi32(340,220,78);
	//	
	//	LCD_DrawRectangle(390,215 ,470 ,261 );
	show_buttom(380,215,80,32);
		showhanzi32(395,220,28);//����
		showhanzi32(430,220,29);
	}
		if(edit_statement==2)
	{
		LCD_DrawRectangle(300,10 ,380 ,65 );
		showhanzi32(305,20,18);//��ҳ
		showhanzi32(340,20,19);
		
		LCD_DrawRectangle(390,10 ,470 ,65 );
		showhanzi32(395,20,49);//����
		showhanzi32(430,20,50);
		
		LCD_DrawRectangle(300,70 ,380 ,125 );
		showhanzi32(305,80,22);//����
		showhanzi32(340,80,23);
		
		LCD_DrawRectangle(390,70 ,470 ,125 );
		showhanzi32(395,80,24);//����
		showhanzi32(430,80,25);
		
		LCD_DrawRectangle(300,130 ,380 ,185 );
		showhanzi32(322,140,26);//��
	 
		
		LCD_DrawRectangle(390,130 ,470 ,185 );
		showhanzi32(412,140,27);//��
		
		
		LCD_DrawRectangle(300,190 ,380 ,245 );
		showhanzi32(305,200,47);//ɾ��
		showhanzi32(340,200,48);
		
		LCD_DrawRectangle(390,190 ,470 ,245 );
		showhanzi32(395,200,28);//����
		showhanzi32(430,200,29);
	}
		if(edit_statement==4||edit_statement==5||edit_statement==6)
	{
		LCD_DrawRectangle(300,10 ,380 ,65 );
		showhanzi32(305,20,18);//��ҳ
		showhanzi32(340,20,19);
		
		LCD_DrawRectangle(390,10 ,470 ,65 );
		showhanzi32(395,20,49);//����
		showhanzi32(430,20,50);
		
		LCD_DrawRectangle(300,70 ,380 ,125 );
		showhanzi32(305,80,22);//����
		showhanzi32(340,80,23);
		
		LCD_DrawRectangle(390,70 ,470 ,125 );
		showhanzi32(395,80,24);//����
		showhanzi32(430,80,25);
		
		LCD_DrawRectangle(300,130 ,380 ,185 );
		showhanzi32(322,140,26);//��
	 
		
		LCD_DrawRectangle(390,130 ,470 ,185 );
		showhanzi32(412,140,27);//��
		
		
		LCD_DrawRectangle(300,190 ,380 ,245 );
		showhanzi32(305,200,75);//��ȡ
		showhanzi32(340,200,76);
		
		LCD_DrawRectangle(390,190 ,470 ,245 );
		showhanzi32(395,200,77);//����
		showhanzi32(430,200,78);
	}
}
void jia_service(void)//�ӷ�����
{
	   u8 i;
		if(edit_statement==0)//�༭ָ��
		{
			for(i=1;i<=4;i++)
			{
			command_edit_num_wei[command_num][i]=0;
			}
			
			if(command[command_num]>=0&&command[command_num]<=15)
			{
				command[command_num]++;
			}
			if(command[command_num]==10)//ָ���ѭ��
			{
				command[command_num]=0;
			}
			if(command[command_num]==2)//�����λ��������һ��ָ����λ�ƵĲ���
			{
					Weiyi_Dis[command_num].Pulse_Num.shiwan=0;//���������ֵ
					Weiyi_Dis[command_num].Pulse_Num.wan=0;
			    Weiyi_Dis[command_num].Pulse_Num.qian=0;
					Weiyi_Dis[command_num].Pulse_Num.bai=0;
					Weiyi_Dis[command_num].Pulse_Num.shi=0;
					Weiyi_Dis[command_num].Pulse_Num.ge=0;
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
					Weiyi_Dis[command_num].Pulse_Rate.wan=0;//����Ƶ����ֵ
					Weiyi_Dis[command_num].Pulse_Rate.qian=0;
					Weiyi_Dis[command_num].Pulse_Rate.bai=0;
					Weiyi_Dis[command_num].Pulse_Rate.shi=0;
					Weiyi_Dis[command_num].Pulse_Rate.ge=0;
	
					Weiyi_Dis[command_num].Pulse_Acc.wan=0;//�����ٶ���ֵ
					Weiyi_Dis[command_num].Pulse_Acc.qian=0;
					Weiyi_Dis[command_num].Pulse_Acc.bai=0;
					Weiyi_Dis[command_num].Pulse_Acc.shi=0;
					Weiyi_Dis[command_num].Pulse_Acc.ge=0;
				
				Weiyi[command_num].Direction=0;
				Weiyi[command_num].Pulse_Num=0;
				Weiyi[command_num].Pulse_Rate=0;
				Weiyi[command_num].Speed_Acc=0;
			}
			if(command[command_num]==8)//�����ѭ��ָ���Ҫ��������2��ָ��
			{
					for(i=99;i>command_num+2;i--)
					{
						command[i]=command[i-2];
						command_edit_num_wei[i][1]=command_edit_num_wei[i-2][1];
						command_edit_num_wei[i][2]=command_edit_num_wei[i-2][2];
						command_edit_num_wei[i][3]=command_edit_num_wei[i-2][3];
						command_edit_num_wei[i][4]=command_edit_num_wei[i-2][4];
					}
		      command[command_num+1]=20;
					command[command_num+2]=21;
					command_edit_num_wei[command_num+1][1]=0;
					command_edit_num_wei[command_num+1][2]=0;
					command_edit_num_wei[command_num+1][3]=0;
					command_edit_num_wei[command_num+1][4]=0;
					command_edit_num_wei[command_num+2][1]=0;
					command_edit_num_wei[command_num+2][2]=0;
					command_edit_num_wei[command_num+2][3]=0;
					command_edit_num_wei[command_num+2][4]=0;
			}
			if(command[command_num]==9)//����ѭ��ָ������ָ��ȥ��
			{
					for(i=1;(i+command_num)<=100;i++)
					{
						command[command_num+i]=command[command_num+i+2];
						command_edit_num_wei[command_num+i][1]=command_edit_num_wei[command_num+i+2][1];
						command_edit_num_wei[command_num+i][2]=command_edit_num_wei[command_num+i+2][2];
						command_edit_num_wei[command_num+i][3]=command_edit_num_wei[command_num+i+2][3];
						command_edit_num_wei[command_num+i][4]=command_edit_num_wei[command_num+i+2][4];
					}
			}
			display_command();
		}
		
		
		
		
		if(edit_statement==1)//�༭ָ�������
		{
				  if(command[command_num]==1||command[command_num]==2||command[command_num]==3||command[command_num]==7||command[command_num]==8||command[command_num]==20||command[command_num]==21)
				{
					command_edit_num_wei[command_num][Pinpoint_Edit]++;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
					if(command_edit_num_wei[command_num][Pinpoint_Edit]==10)
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]=0;
					}
					display_command();
				}
			  if(command[command_num]==4||command[command_num]==6)//���� ɨ��
				{
					if(Pinpoint_Edit==3)
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]++;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
						if(command_edit_num_wei[command_num][Pinpoint_Edit]==7)
						{
							command_edit_num_wei[command_num][Pinpoint_Edit]=1;
						}
						display_command();
					}
						if(Pinpoint_Edit==4)//0,1����ָʾ�ߵ�
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]++;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
						if(command_edit_num_wei[command_num][Pinpoint_Edit]==2)
						{
							command_edit_num_wei[command_num][Pinpoint_Edit]=0;
						}
						display_command();
					}
				}
						if(command[command_num]==5)//���
				{
					if(Pinpoint_Edit==3)
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]++;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
						if(command_edit_num_wei[command_num][Pinpoint_Edit]==5)
						{
							command_edit_num_wei[command_num][Pinpoint_Edit]=1;
						}
						display_command();
					}
						if(Pinpoint_Edit==4)//0,1����ָʾ�ߵ�
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]++;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
						if(command_edit_num_wei[command_num][Pinpoint_Edit]==2)
						{
							command_edit_num_wei[command_num][Pinpoint_Edit]=0;
						}
						display_command();
					}
				}
			
		}
		
		
		
		
		
	  if(edit_statement==2)//�༭λ��
		{
			switch(Pinpoint_Num)
			{
				case 1:	  if(Weiyi[state].Direction==0)
									{
										Weiyi[state].Direction=1;
									}
									else
									{
										Weiyi[state].Direction=0;
									}
									break;
				case 2:	Weiyi_Dis[state].Pulse_Num.shiwan++;
									if(Weiyi_Dis[state].Pulse_Num.shiwan==10)
									{
										Weiyi_Dis[state].Pulse_Num.shiwan=0;
									}
									break;
				case 3:	Weiyi_Dis[state].Pulse_Num.wan++;
									if(Weiyi_Dis[state].Pulse_Num.wan==10)
									{
										Weiyi_Dis[state].Pulse_Num.wan=0;
									}
									break;
				case 4:	Weiyi_Dis[state].Pulse_Num.qian++;
									if(Weiyi_Dis[state].Pulse_Num.qian==10)
									{
										Weiyi_Dis[state].Pulse_Num.qian=0;
									}
									break;
				case 5:	Weiyi_Dis[state].Pulse_Num.bai++;
									if(Weiyi_Dis[state].Pulse_Num.bai==10)
									{
										Weiyi_Dis[state].Pulse_Num.bai=0;
									}
									break;
				case 6:	Weiyi_Dis[state].Pulse_Num.shi++;
									if(Weiyi_Dis[state].Pulse_Num.shi==10)
									{
									Weiyi_Dis[state].Pulse_Num.shi=0;
									}
									break;
				case 7:	Weiyi_Dis[state].Pulse_Num.ge++;
									if(Weiyi_Dis[state].Pulse_Num.ge==10)
									{
										Weiyi_Dis[state].Pulse_Num.ge=0;
									}
									break;
				case 8:	Weiyi_Dis[state].Pulse_Rate.wan++;
									if(Weiyi_Dis[state].Pulse_Rate.wan==10)
									{
										Weiyi_Dis[state].Pulse_Rate.wan=0;
									}
									break;
				case 9:	Weiyi_Dis[state].Pulse_Rate.qian++;
									if(Weiyi_Dis[state].Pulse_Rate.qian==10)
									{
										Weiyi_Dis[state].Pulse_Rate.qian=0;
									}
									break;
				case 10:	Weiyi_Dis[state].Pulse_Rate.bai++;
									if(Weiyi_Dis[state].Pulse_Rate.bai==10)
									{
										Weiyi_Dis[state].Pulse_Rate.bai=0;
									}
									break;
				case 11:	Weiyi_Dis[state].Pulse_Rate.shi++;
									if(Weiyi_Dis[state].Pulse_Rate.shi==10)
									{
										Weiyi_Dis[state].Pulse_Rate.shi=0;
									}
									break;
				case 12:	Weiyi_Dis[state].Pulse_Rate.ge++;
									if(Weiyi_Dis[state].Pulse_Rate.ge==10)
									{
										Weiyi_Dis[state].Pulse_Rate.ge=0;
									}
									break;
				case 13:	Weiyi_Dis[state].Pulse_Acc.wan++;
									if(Weiyi_Dis[state].Pulse_Acc.wan==10)
									{
										Weiyi_Dis[state].Pulse_Acc.wan=0;
									}
									break;
				case 14:	Weiyi_Dis[state].Pulse_Acc.qian++;
									if(Weiyi_Dis[state].Pulse_Acc.qian==10)
									{
										Weiyi_Dis[state].Pulse_Acc.qian=0;
									}
									break;
					case 15:	Weiyi_Dis[state].Pulse_Acc.bai++;
									if(Weiyi_Dis[state].Pulse_Acc.bai==10)
									{
										Weiyi_Dis[state].Pulse_Acc.bai=0;
									}
									break;
					case 16:	Weiyi_Dis[state].Pulse_Acc.shi++;
									if(Weiyi_Dis[state].Pulse_Acc.shi==10)
									{
										Weiyi_Dis[state].Pulse_Acc.shi=0;
									}
									break;
					case 17:	Weiyi_Dis[state].Pulse_Acc.ge++;
									if(Weiyi_Dis[state].Pulse_Acc.ge==10)
									{
										Weiyi_Dis[state].Pulse_Acc.ge=0;
									}
									break;							
				}
			/**********************************************************/
			/*����λ�Ƶ��������
				          ����Ƶ��
									����ʱ��*/
				Weiyi[state].Pulse_Num=Weiyi_Dis[state].Pulse_Num.shiwan*100000
														+Weiyi_Dis[state].Pulse_Num.wan*10000
			                      +Weiyi_Dis[state].Pulse_Num.qian*1000
														+Weiyi_Dis[state].Pulse_Num.bai*100
														+Weiyi_Dis[state].Pulse_Num.shi*10
														+Weiyi_Dis[state].Pulse_Num.ge;//�õ�����λ�ƾ���
			  Weiyi[state].Pulse_Num=Weiyi[state].Pulse_Num*PerPulseNum/PerDistance;//�õ��������
				
				
				Weiyi[state].Pulse_Rate=Weiyi_Dis[state].Pulse_Rate.wan*10000
			                      +Weiyi_Dis[state].Pulse_Rate.qian*1000
														+Weiyi_Dis[state].Pulse_Rate.bai*100
														+Weiyi_Dis[state].Pulse_Rate.shi*10
														+Weiyi_Dis[state].Pulse_Rate.ge;
				Weiyi[state].Speed_Acc=Weiyi_Dis[state].Pulse_Acc.wan*10000
			                      +Weiyi_Dis[state].Pulse_Acc.qian*1000
														+Weiyi_Dis[state].Pulse_Acc.bai*100
														+Weiyi_Dis[state].Pulse_Acc.shi*10
														+Weiyi_Dis[state].Pulse_Acc.ge;
			}
		if(edit_statement==4)//ϵͳ����
		{
			switch(Pinpoint_Num)
			{
				case 1: Select_Pro++;
								if(Select_Pro==4)
								{
									Select_Pro=1;
								}
								break;	
        case 2:if(Mode==Auto_Mode)
				        {
									Mode=Hand_Mode;
								}
								else
								{
									Mode=Auto_Mode;
								}
			}
		}
		if(edit_statement==5)//ϵͳ����2
		{
			switch(Pinpoint_Num)
			{
				case 1:Pulse_Dangliang.wan++;
								if(Pulse_Dangliang.wan==10)
								{
									Pulse_Dangliang.wan=0;
								}
								break;
				case 2:Pulse_Dangliang.qian++;
								if(Pulse_Dangliang.qian==10)
								{
									Pulse_Dangliang.qian=0;
								}
								break;
				case 3:Pulse_Dangliang.bai++;
								if(Pulse_Dangliang.bai==10)
								{
									Pulse_Dangliang.bai=0;
								}
								break;
				case 4:Pulse_Dangliang.shi++;
								if(Pulse_Dangliang.shi==10)
								{
									Pulse_Dangliang.shi=0;
								}
								break;
				case 5:Pulse_Dangliang.ge++;
								if(Pulse_Dangliang.ge==10)
								{
									Pulse_Dangliang.ge=0;
								}
								break;
				case 6:Distance_Per.wan++;
								if(Distance_Per.wan==10)
								{
									Distance_Per.wan=0;
								}
								break;
				case 7: Distance_Per.qian++;
								if(Distance_Per.qian==10)
								{
									Distance_Per.qian=0;
								}
								break;
				case 8:Distance_Per.bai++;
								if(Distance_Per.bai==10)
								{
									Distance_Per.bai=0;
								}
								break;		
				case 9:Distance_Per.shi++;
								if(Distance_Per.shi==10)
								{
									Distance_Per.shi=0;
								}
								break;
				case 10:Distance_Per.ge++;
								if(Distance_Per.ge==10)
								{
									Distance_Per.ge=0;
								}
								break;								
			}
			
				PerPulseNum=Pulse_Dangliang.wan*10000
			              +Pulse_Dangliang.qian*1000
										+Pulse_Dangliang.bai*100
										+Pulse_Dangliang.shi*10
										+Pulse_Dangliang.ge;
				PerDistance=Distance_Per.wan*10000
			                      +Distance_Per.qian*1000
														+Distance_Per.bai*100
														+Distance_Per.shi*10
														+Distance_Per.ge;
		}
		if(edit_statement==6)//ϵͳ����2
		{
			switch(Pinpoint_Num)
			{
				case 1:Dot_Pulse.wan++;
								if(Dot_Pulse.wan==10)
								{
									Dot_Pulse.wan=0;
								}
								break;
				case 2:Dot_Pulse.qian++;
								if(Dot_Pulse.qian==10)
								{
									Dot_Pulse.qian=0;
								}
								break;
				case 3:Dot_Pulse.bai++;
								if(Dot_Pulse.bai==10)
								{
									Dot_Pulse.bai=0;
								}
								break;
				case 4:Dot_Pulse.shi++;
								if(Dot_Pulse.shi==10)
								{
									Dot_Pulse.shi=0;
								}
								break;
				case 5:Dot_Pulse.ge++;
								if(Dot_Pulse.ge==10)
								{
									Dot_Pulse.ge=0;
								}
								break;
				case 6:Constant_Speed.wan++;
								if(Constant_Speed.wan==10)
								{
									Constant_Speed.wan=0;
								}
								break;
				case 7: Constant_Speed.qian++;
								if(Constant_Speed.qian==10)
								{
									Constant_Speed.qian=0;
								}
								break;
				case 8:Constant_Speed.bai++;
								if(Constant_Speed.bai==10)
								{
									Constant_Speed.bai=0;
								}
								break;		
				case 9:Constant_Speed.shi++;
								if(Constant_Speed.shi==10)
								{
									Constant_Speed.shi=0;
								}
								break;
				case 10:Constant_Speed.ge++;
								if(Constant_Speed.ge==10)
								{
									Constant_Speed.ge=0;
								}
								break;								
			}
		}
    		
}
void jian_service(void)//��������
{
		  u8 i;
		if(edit_statement==0)//�༭ָ��
		{
			for(i=1;i<=4;i++)
			{
			command_edit_num_wei[command_num][i]=0;
			}
			command[command_num]--;
			if(command[command_num]==255)
			{
				command[command_num]=9;
			}
			if(command[command_num]==0)
			{
				Weiyi_Dis[command_num].Pulse_Num.shiwan=0;//���������ֵ
					Weiyi_Dis[command_num].Pulse_Num.wan=0;
			    Weiyi_Dis[command_num].Pulse_Num.qian=0;
					Weiyi_Dis[command_num].Pulse_Num.bai=0;
					Weiyi_Dis[command_num].Pulse_Num.shi=0;
					Weiyi_Dis[command_num].Pulse_Num.ge=0;
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
					Weiyi_Dis[command_num].Pulse_Rate.wan=0;//����Ƶ����ֵ
					Weiyi_Dis[command_num].Pulse_Rate.qian=0;
					Weiyi_Dis[command_num].Pulse_Rate.bai=0;
					Weiyi_Dis[command_num].Pulse_Rate.shi=0;
					Weiyi_Dis[command_num].Pulse_Rate.ge=0;
	
					Weiyi_Dis[command_num].Pulse_Acc.wan=0;//�����ٶ���ֵ
					Weiyi_Dis[command_num].Pulse_Acc.qian=0;
					Weiyi_Dis[command_num].Pulse_Acc.bai=0;
					Weiyi_Dis[command_num].Pulse_Acc.shi=0;
					Weiyi_Dis[command_num].Pulse_Acc.ge=0;
				
				Weiyi[command_num].Direction=0;
				Weiyi[command_num].Pulse_Num=0;
				Weiyi[command_num].Pulse_Rate=0;
				Weiyi[command_num].Speed_Acc=0;
			}
			if(command[command_num]==8)//�����ѭ��ָ���Ҫ��������2��ָ��
			{
					for(i=99;i>command_num+2;i--)
					{
						command[i]=command[i-2];
						command_edit_num_wei[i][1]=command_edit_num_wei[i-2][1];
						command_edit_num_wei[i][2]=command_edit_num_wei[i-2][2];
						command_edit_num_wei[i][3]=command_edit_num_wei[i-2][3];
						command_edit_num_wei[i][4]=command_edit_num_wei[i-2][4];
					}
		      command[command_num+1]=20;
					command[command_num+2]=21;
					command_edit_num_wei[command_num+1][1]=0;
					command_edit_num_wei[command_num+1][2]=0;
					command_edit_num_wei[command_num+1][3]=0;
					command_edit_num_wei[command_num+1][4]=0;
					command_edit_num_wei[command_num+2][1]=0;
					command_edit_num_wei[command_num+2][2]=0;
					command_edit_num_wei[command_num+2][3]=0;
					command_edit_num_wei[command_num+2][4]=0;
			}
			if(command[command_num]==7)//����ѭ��ָ������ָ��ȥ��
			{
					for(i=1;(i+command_num)<=100;i++)
					{
						command[command_num+i]=command[command_num+i+2];
						command_edit_num_wei[command_num+i][1]=command_edit_num_wei[command_num+i+2][1];
						command_edit_num_wei[command_num+i][2]=command_edit_num_wei[command_num+i+2][2];
						command_edit_num_wei[command_num+i][3]=command_edit_num_wei[command_num+i+2][3];
						command_edit_num_wei[command_num+i][4]=command_edit_num_wei[command_num+i+2][4];
					}
			}
			display_command();
		}
		///////////////////////////////////////////////////////////////////
		
		
		
		
		if(edit_statement==1)//�༭ָ�������
		{
		  	if(command[command_num]==1||command[command_num]==2||command[command_num]==3||command[command_num]==7||command[command_num]==8||command[command_num]==20||command[command_num]==21)
				{
					command_edit_num_wei[command_num][Pinpoint_Edit]--;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
					if(command_edit_num_wei[command_num][Pinpoint_Edit]==255)
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]=9;
					}
					display_command();
				}
			  if(command[command_num]==4||command[command_num]==5||command[command_num]==6)
				{
					if(Pinpoint_Edit==3)
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]--;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
						if(command_edit_num_wei[command_num][Pinpoint_Edit]==255)
						{
							command_edit_num_wei[command_num][Pinpoint_Edit]=9;
						}
						display_command();
					}
						if(Pinpoint_Edit==4)//0,1����ָʾ�ߵ�
					{
						command_edit_num_wei[command_num][Pinpoint_Edit]--;//��Ӧ�ò�����++��command_num����ָʾ��ǰ�༭�ڼ���ָ�Pinpoint_Edit����ָʾָ��4λ��������ĳһλ
						if(command_edit_num_wei[command_num][Pinpoint_Edit]==255)
						{
							command_edit_num_wei[command_num][Pinpoint_Edit]=1;
						}
						display_command();
					}
				}
		}
		//////////////////////////////////////////////////////////////////////
		
		
		
		if(edit_statement==2)//�༭λ��
		{
			switch(Pinpoint_Num)
			{
				case 1:		if(Weiyi[state].Direction==0)
									{
										Weiyi[state].Direction=1;
									}
									else
									{
										Weiyi[state].Direction=0;
									}
									break;
				case 2:	Weiyi_Dis[state].Pulse_Num.shiwan--;
									if(Weiyi_Dis[state].Pulse_Num.shiwan==255)
									{
										Weiyi_Dis[state].Pulse_Num.shiwan=9;
									}
									break;
				case 3:	Weiyi_Dis[state].Pulse_Num.wan--;
									if(Weiyi_Dis[state].Pulse_Num.wan==255)
									{
										Weiyi_Dis[state].Pulse_Num.wan=9;
									}
									break;
				case 4:	Weiyi_Dis[state].Pulse_Num.qian--;
									if(Weiyi_Dis[state].Pulse_Num.qian==255)
									{
										Weiyi_Dis[state].Pulse_Num.qian=9;
									}
									break;
				case 5:	Weiyi_Dis[state].Pulse_Num.bai--;
									if(Weiyi_Dis[state].Pulse_Num.bai==255)
									{
										Weiyi_Dis[state].Pulse_Num.bai=9;
									}
									break;
				case 6:	Weiyi_Dis[state].Pulse_Num.shi--;
									if(Weiyi_Dis[state].Pulse_Num.shi==255)
									{
									Weiyi_Dis[state].Pulse_Num.shi=9;
									}
									break;
				case 7:	Weiyi_Dis[state].Pulse_Num.ge--;
									if(Weiyi_Dis[state].Pulse_Num.ge==255)
									{
										Weiyi_Dis[state].Pulse_Num.ge=9;
									}
									break;
				case 8:	Weiyi_Dis[state].Pulse_Rate.wan--;
									if(Weiyi_Dis[state].Pulse_Rate.wan==255)
									{
										Weiyi_Dis[state].Pulse_Rate.wan=9;
									}
									break;
				case 9:	Weiyi_Dis[state].Pulse_Rate.qian--;
									if(Weiyi_Dis[state].Pulse_Rate.qian==255)
									{
										Weiyi_Dis[state].Pulse_Rate.qian=9;
									}
									break;
				case 10:	Weiyi_Dis[state].Pulse_Rate.bai--;
									if(Weiyi_Dis[state].Pulse_Rate.bai==255)
									{
										Weiyi_Dis[state].Pulse_Rate.bai=9;
									}
									break;
				case 11:	Weiyi_Dis[state].Pulse_Rate.shi--;
									if(Weiyi_Dis[state].Pulse_Rate.shi==255)
									{
										Weiyi_Dis[state].Pulse_Rate.shi=9;
									}
									break;
				case 12:	Weiyi_Dis[state].Pulse_Rate.ge--;
									if(Weiyi_Dis[state].Pulse_Rate.ge==255)
									{
										Weiyi_Dis[state].Pulse_Rate.ge=9;
									}
									break;
				case 13:	Weiyi_Dis[state].Pulse_Acc.wan--;
									if(Weiyi_Dis[state].Pulse_Acc.wan==255)
									{
										Weiyi_Dis[state].Pulse_Acc.wan=9;
									}
									break;
				case 14:	Weiyi_Dis[state].Pulse_Acc.qian--;
									if(Weiyi_Dis[state].Pulse_Acc.qian==255)
									{
										Weiyi_Dis[state].Pulse_Acc.qian=9;
									}
									break;
					case 15:	Weiyi_Dis[state].Pulse_Acc.bai--;
									if(Weiyi_Dis[state].Pulse_Acc.bai==255)
									{
										Weiyi_Dis[state].Pulse_Acc.bai=9;
									}
									break;
					case 16:	Weiyi_Dis[state].Pulse_Acc.shi--;
									if(Weiyi_Dis[state].Pulse_Acc.shi==255)
									{
										Weiyi_Dis[state].Pulse_Acc.shi=9;
									}
									break;
					case 17:	Weiyi_Dis[state].Pulse_Acc.ge--;
									if(Weiyi_Dis[state].Pulse_Acc.ge==255)
									{
										Weiyi_Dis[state].Pulse_Acc.ge=9;
									}
									break;							
			}
				Weiyi[state].Pulse_Num=Weiyi_Dis[state].Pulse_Num.shiwan*100000
														+Weiyi_Dis[state].Pulse_Num.wan*10000
			                      +Weiyi_Dis[state].Pulse_Num.qian*1000
														+Weiyi_Dis[state].Pulse_Num.bai*100
														+Weiyi_Dis[state].Pulse_Num.shi*10
														+Weiyi_Dis[state].Pulse_Num.ge;//�õ�����ʵ��λ�ƾ���
			 Weiyi[state].Pulse_Num=Weiyi[state].Pulse_Num*PerPulseNum/PerDistance;//�õ��������
			
				Weiyi[state].Pulse_Rate=Weiyi_Dis[state].Pulse_Rate.wan*10000
			                      +Weiyi_Dis[state].Pulse_Rate.qian*1000
														+Weiyi_Dis[state].Pulse_Rate.bai*100
														+Weiyi_Dis[state].Pulse_Rate.shi*10
														+Weiyi_Dis[state].Pulse_Rate.ge;
				Weiyi[state].Speed_Acc=Weiyi_Dis[state].Pulse_Acc.wan*10000
			                      +Weiyi_Dis[state].Pulse_Acc.qian*1000
														+Weiyi_Dis[state].Pulse_Acc.bai*100
														+Weiyi_Dis[state].Pulse_Acc.shi*10
														+Weiyi_Dis[state].Pulse_Acc.ge;
		}
		if(edit_statement==4)//ϵͳ����
		{
			switch(Pinpoint_Num)
			{
				case 1: Select_Pro--;
								if(Select_Pro==0)
								{
									Select_Pro=3;
								}
								break;	
        case 2:if(Mode==Auto_Mode)
				        {
									Mode=Hand_Mode;
								}
								else
								{
									Mode=Auto_Mode;
								}
			
		  }
			
		}
		if(edit_statement==5)//ϵͳ����
		{
			switch(Pinpoint_Num)
			{
		
				case 1:Pulse_Dangliang.wan--;
								if(Pulse_Dangliang.wan==255)
								{
									Pulse_Dangliang.wan=9;
								}
								break;
				case 2:Pulse_Dangliang.qian--;
								if(Pulse_Dangliang.qian==255)
								{
									Pulse_Dangliang.qian=9;
								}
								break;
				case 3:Pulse_Dangliang.bai--;
								if(Pulse_Dangliang.bai==255)
								{
									Pulse_Dangliang.bai=9;
								}
								break;
				case 4:Pulse_Dangliang.shi--;
								if(Pulse_Dangliang.shi==255)
								{
									Pulse_Dangliang.shi=9;
								}
								break;
				case 5:Pulse_Dangliang.ge--;
								if(Pulse_Dangliang.ge==255)
								{
									Pulse_Dangliang.ge=9;
								}
								break;
				case 6:Distance_Per.wan--;
								if(Distance_Per.wan==255)
								{
									Distance_Per.wan=9;
								}
								break;
				case 7: Distance_Per.qian--;
								if(Distance_Per.qian==255)
								{
									Distance_Per.qian=9;
								}
								break;
				case 8:Distance_Per.bai--;
								if(Distance_Per.bai==255)
								{
									Distance_Per.bai=9;
								}
								break;		
				case 9:Distance_Per.shi--;
								if(Distance_Per.shi==255)
								{
									Distance_Per.shi=9;
								}
								break;
				case 10:Distance_Per.ge--;
								if(Distance_Per.ge==255)
								{
									Distance_Per.ge=9;
								}
								break;								
			}
				PerPulseNum=Pulse_Dangliang.wan*10000
			              +Pulse_Dangliang.qian*1000
										+Pulse_Dangliang.bai*100
										+Pulse_Dangliang.shi*10
										+Pulse_Dangliang.ge;
				PerDistance=Distance_Per.wan*10000
			              +Distance_Per.qian*1000
									  +Distance_Per.bai*100
										+Distance_Per.shi*10
										+Distance_Per.ge;
		}
		
		if(edit_statement==6)//ϵͳ����
		{
			switch(Pinpoint_Num)
			{
		
				case 1:Dot_Pulse.wan--;
								if(Dot_Pulse.wan==255)
								{
									Dot_Pulse.wan=9;
								}
								break;
				case 2:Dot_Pulse.qian--;
								if(Dot_Pulse.qian==255)
								{
									Dot_Pulse.qian=9;
								}
								break;
				case 3:Dot_Pulse.bai--;
								if(Dot_Pulse.bai==255)
								{
									Dot_Pulse.bai=9;
								}
								break;
				case 4:Dot_Pulse.shi--;
								if(Dot_Pulse.shi==255)
								{
									Dot_Pulse.shi=9;
								}
								break;
				case 5:Dot_Pulse.ge--;
								if(Dot_Pulse.ge==255)
								{
									Dot_Pulse.ge=9;
								}
								break;
				case 6:Constant_Speed.wan--;
								if(Constant_Speed.wan==255)
								{
									Constant_Speed.wan=9;
								}
								break;
				case 7: Constant_Speed.qian--;
								if(Constant_Speed.qian==255)
								{
									Constant_Speed.qian=9;
								}
								break;
				case 8:Constant_Speed.bai--;
								if(Constant_Speed.bai==255)
								{
									Constant_Speed.bai=9;
								}
								break;		
				case 9:Constant_Speed.shi--;
								if(Constant_Speed.shi==255)
								{
									Constant_Speed.shi=9;
								}
								break;
				case 10:Constant_Speed.ge--;
								if(Constant_Speed.ge==255)
								{
									Constant_Speed.ge=9;
								}
								break;								
			}
			
		}


}
void Move_LR_Service(void)//�����Ʒ�����
{
	int i;
	if(edit_statement==0)
	{
		
	}
	if(edit_statement==2)//�༭λ��״̬
	{
			LCD_Fill(180,57, 288, 60,WHITE);
			LCD_Fill(180,117, 288, 120,WHITE);
			LCD_Fill(198,177, 288, 180,WHITE);
			LCD_Fill(198,237, 288, 240,WHITE);
		  if(Pinpoint_Num<=1)
			{
				for(i=0;i<=2;i++)
				{
					//showhanzi32(254,20,93);
					LCD_DrawLine(254, 57+i, 286, 57+i);//���߳���
				}
			}
			else if(Pinpoint_Num<=7)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(144+Pinpoint_Num*18, 117+i, 162+18*Pinpoint_Num, 117+i);
				}
			}
			else if(Pinpoint_Num<=12)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(162+(Pinpoint_Num-6)*18, 177+i, 180+18*(Pinpoint_Num-6), 177+i);
				}
			}	
			else if(Pinpoint_Num<=17)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(162+(Pinpoint_Num-11)*18, 237+i, 180+18*(Pinpoint_Num-11),237+i);
				}
			}
 }
	if(edit_statement==4)//�༭ϵͳ
	{
			LCD_Fill(180,117, 288, 120,WHITE);
			LCD_Fill(198,177, 288, 180,WHITE);
			LCD_Fill(198,237, 288, 240,WHITE);
				if(Pinpoint_Num==1)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(270, 117+i, 288, 117+i);
				}
			}
			else if(Pinpoint_Num==2)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(222, 177+i, 286, 177+i);
				}
			}			
  }
		if(edit_statement==5)//�༭ϵͳ2
	{
			LCD_Fill(198,177, 288, 180,WHITE);
			LCD_Fill(198,237, 288, 240,WHITE);
		
			if(Pinpoint_Num<=5)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-1)*18, 177+i, 216+18*(Pinpoint_Num-1), 177+i);
				}
			}	
			else if(Pinpoint_Num<=10)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-6)*18, 237+i, 216+18*(Pinpoint_Num-6),237+i);
				}
			}
   }
			if(edit_statement==6)//�༭ϵͳ3
	{
			LCD_Fill(198,177, 288, 180,WHITE);
			LCD_Fill(198,237, 288, 240,WHITE);
		
			if(Pinpoint_Num<=5)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-1)*18, 177+i, 216+18*(Pinpoint_Num-1), 177+i);
				}
			}	
			else if(Pinpoint_Num<=10)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-6)*18, 237+i, 216+18*(Pinpoint_Num-6),237+i);
				}
			}
   }
}
void Short_Line_flicker(void)//������˸
{
	  int i,j;
	 	if(edit_statement==0)//�༭ָ��
	{
		if(flicker_count==2000)//�ڳ����м���
		{
			for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
		  {
					 LCD_DrawLine(60,50*(command_num%5+1)-30+32+j, 140,50*(command_num%5+1)-30+32+j);
			}
		}
		if(flicker_count>4000)
		{
			LCD_Fill(60, 50*(command_num%5+1)-30+32+1, 140,50*(command_num%5+1)-30+32+3,WHITE);//������0
			flicker_count=0;
		//	LED1=~LED1;
	  }
	}
		if(edit_statement==1)//�༭������
	{
		if(command[command_num]==7||command[command_num]==1||command[command_num]==8||command[command_num]==20||command[command_num]==21)//1��7ָ�����˸
		{
			if(flicker_count==2000)//�ڳ����м���
			{
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
				{
						 LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
				}
			}
			if(flicker_count==4000)
			{
				for(j=1;j<=3;j++)
				{
					LCD_Fill(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j,WHITE);//������0
					flicker_count=0;
				//	LED1=~LED1;
				}
			}
		}
			if(command[command_num]==2||command[command_num]==3)//2,3����ָ�����˸
		{
			if(flicker_count==2000)//�ڳ����м���
			{
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
				{
						 LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
				}
			}
			if(flicker_count==4000)
			{
				for(j=1;j<=3;j++)
				{
					LCD_Fill(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j,WHITE);//������0
					flicker_count=0;
				//	LED1=~LED1;
				}
			}
		}
			if(command[command_num]==4||command[command_num]==5||command[command_num]==6)//4,5,6����ָ�����˸
		{
			if(flicker_count==2000)//�ڳ����м���
			{
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
				{
						 LCD_DrawLine(150+35*(2*Pinpoint_Edit-5),50*(command_num%5+1)-30+32+j, 150+35*(2*Pinpoint_Edit-5)+25,50*(command_num%5+1)-30+32+j);
				}
			}
			if(flicker_count==4000)
			{
				for(j=1;j<=3;j++)
				{
					LCD_Fill(150+35*(2*Pinpoint_Edit-5),50*(command_num%5+1)-30+32+j, 150+35*(2*Pinpoint_Edit-5)+25,50*(command_num%5+1)-30+32+j,WHITE);//������0
					flicker_count=0;
					//LED1=~LED1;
				}
			}
		}
		
	}
		if(edit_statement==2)//�༭λ�ƶ�
	{
		if(Pinpoint_Num<=1)
		{
			if(flicker_count==50)//�ڳ����м���
			{
				for(i=0;i<=2;i++)
				{
					//showhanzi32(254,20,93);
					LCD_DrawLine(254, 57+i, 286, 57+i);//���߳���
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(254, 57, 286,60,WHITE);//������0
				flicker_count=0;
			//	LED0=~LED0;
			}
			
		}
		else if(Pinpoint_Num<=7)//����λ�Ʒֳ�3��
		{
			if(flicker_count==50)//�ڳ����м���
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(144+Pinpoint_Num*18, 117+i, 162+18*Pinpoint_Num, 117+i);//���߳���
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(144+Pinpoint_Num*18, 117, 162+18*Pinpoint_Num, 120,WHITE);//������0
				flicker_count=0;
			//	LED0=~LED0;
			}

		}
		else if(Pinpoint_Num<=12)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(162+(Pinpoint_Num-6)*18, 177+i, 180+18*(Pinpoint_Num-6), 177+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(162+(Pinpoint_Num-6)*18, 177, 180+18*(Pinpoint_Num-6), 180,WHITE);
				flicker_count=0;
				//LED0=~LED0;
			}

		}
		else if(Pinpoint_Num<=17)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(162+(Pinpoint_Num-11)*18, 237+i, 180+18*(Pinpoint_Num-11),237+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(162+(Pinpoint_Num-11)*18, 237, 180+18*(Pinpoint_Num-11), 240,WHITE);
				flicker_count=0;
				//LED0=~LED0;
			}
		}
	}
			if(edit_statement==4)//�༭ϵͳ
	{
		if(Pinpoint_Num==1)//����λ�Ʒֳ�3��
		{
			if(flicker_count==50)//�ڳ����м���
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(270, 117+i, 288, 117+i);//���߳���
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(270, 117, 288, 120,WHITE);//������0
				flicker_count=0;
		//		LED0=~LED0;
			}

		}
		else if(Pinpoint_Num==2)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(222, 177+i,286 , 177+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(222, 177, 286, 180,WHITE);
				flicker_count=0;
			//	LED0=~LED0;
			}

		}

	}
			if(edit_statement==5)//�༭ϵͳ
	{

		 if(Pinpoint_Num<=5)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-1)*18, 177+i, 216+18*(Pinpoint_Num-1), 177+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(198+(Pinpoint_Num-1)*18, 177, 216+18*(Pinpoint_Num-1), 180,WHITE);
				flicker_count=0;
			//	LED0=~LED0;
			}

		}
		else if(Pinpoint_Num<=10)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-6)*18, 237+i, 216+18*(Pinpoint_Num-6),237+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(198+(Pinpoint_Num-6)*18, 237, 216+18*(Pinpoint_Num-6), 240,WHITE);
				flicker_count=0;
			//	LED0=~LED0;
			}
		}
	}
			if(edit_statement==6)//�༭ϵͳ
	{

		 if(Pinpoint_Num<=5)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-1)*18, 177+i, 216+18*(Pinpoint_Num-1), 177+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(198+(Pinpoint_Num-1)*18, 177, 216+18*(Pinpoint_Num-1), 180,WHITE);
				flicker_count=0;
			//	LED0=~LED0;
			}

		}
		else if(Pinpoint_Num<=10)
		{
			if(flicker_count==50)
			{
				for(i=0;i<=2;i++)
				{
					LCD_DrawLine(198+(Pinpoint_Num-6)*18, 237+i, 216+18*(Pinpoint_Num-6),237+i);
				}
			}
			if(flicker_count==100)
			{
				LCD_Fill(198+(Pinpoint_Num-6)*18, 237, 216+18*(Pinpoint_Num-6), 240,WHITE);
				flicker_count=0;
			//	LED0=~LED0;
			}
		}
	}
	
	

}
void input_command(void)//ɨ��ĳ����������
{
	u8 i,j,k;
	if(init_lock)//�л�״̬ʱ��ʼ��һ��
	{
		start_init();
		edit_init();
		init_lock=0;
		flicker_count=0;
		display_command();
	}
	
	//display_command();
	if(save_ok==0)
	{
	 flicker_count++;//��˸�����Լ�
	Short_Line_flicker();
	}
	
	tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{	
		delay_ms(10);
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			if(uc_lock==1)//��ֹ�ظ�����
			{
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>15&&tp_dev.y<61)//��ҳ��
				{
					for(k=0;k<=100;k++)
					{
						if(command[k]==0)
						{
							command_max=k-1;
							if(command_max==255)
							{
								command_max=0;
							}
							break;
						}
					}
					page_max=command_max/5+1;
				  if(((command_max%5)==4||page<page_max)&&(page<=((command_max+1)/5)))
					{
						page++;
						if(page>page_max)
						{
							page_max=page;
						}
					}
					else
					{
						page=1;
					}
					command_num=page*5-5;//��ҳ��������ʼ�ڵ�һ��
					edit_statement=0;//��ҳ��༭״̬Ϊ�༭ָ��
					display_command();
//					Pinpoint_Num=1;
//				  state++;
//					if(state==17)
//					{
//						state=1;
//					}
					uc_lock=0;
//					init_lock=1;
    
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>15&&tp_dev.y<61)//λ�Ƽ�
				{
					edit_statement=4;
					init_lock=1;
					uc_lock=0;
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>65&&tp_dev.y<111)//���Ƽ�    �����ڱ༭��������λ��
				{				
					if(edit_statement==1)
					{
						if(command[command_num]==2||command[command_num]==3)
						{
							 for(j=1;j<=3;j++)//����ʱ��ʾǰһ�ε�С�̺�
							{
								LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
							}
						}
						if(command[command_num]==4||command[command_num]==5||command[command_num]==6)
						{
							 for(j=1;j<=3;j++)//����ʱ��ʾǰһ�ε�С�̺�
							{
								LCD_DrawLine(150+35*(2*Pinpoint_Edit-5),50*(command_num%5+1)-30+32+j, 150+35*(2*Pinpoint_Edit-5)+25,50*(command_num%5+1)-30+32+j);
							}
						}
							if(command[command_num]==7)
						{
							 for(j=1;j<=3;j++)//����ʱ��ʾǰһ�ε�С�̺�
							{
								LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
							}
						}
						
//							if(command[command_num]==1)
//						{
//							Pinpoint_Edit--;
//							if(Pinpoint_Edit==2)
//							{
//								Pinpoint_Edit=4;
//							}
//						}
						
						
						if(command[command_num]==2)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==0)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==3)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==0)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==4)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==5)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==6)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==7)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==8)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
							if(command[command_num]==20)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
						if(command[command_num]==21)
						{
							Pinpoint_Edit--;
							if(Pinpoint_Edit==2)
							{
								Pinpoint_Edit=4;
							}
						}
						uc_lock=0;
					}
					
					
					
					
					if(edit_statement==2)
					{
						Pinpoint_Num--;
						if(Pinpoint_Num==0)
						{
							Pinpoint_Num=16;
						}
						uc_lock=0;
						Move_LR_Service();
					}
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>65&&tp_dev.y<111)//���Ƽ�
				{
					if(edit_statement==0)
					{
						if(command[command_num]>1)//ֻ�в��ǿ�ָ���λ��ʱ������
						{
							for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
							{
							LCD_DrawLine(60,50*(command_num%5+1)-30+32+j, 140,50*(command_num%5+1)-30+32+j);
							}
						  edit_statement=1;
						  Pinpoint_Edit=0;
							if(command[command_num]==4||command[command_num]==5||command[command_num]==6||command[command_num]==7||command[command_num]==8||command[command_num]==20||command[command_num]==21)
							{
								Pinpoint_Edit=2;
							}
							uc_lock=0;
						}
					}
					
					
					
					if(edit_statement==1)
					{
						if(command[command_num]==2||command[command_num]==3)
						{
							 for(j=1;j<=3;j++)//����ʱ��ʾǰһ�ε�С�̺�
							{
								LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
							}
						}
						if(command[command_num]==4||command[command_num]==5||command[command_num]==6)
						{
							 for(j=1;j<=3;j++)//����ʱ��ʾǰһ�ε�С�̺�
							{
								LCD_DrawLine(150+35*(2*Pinpoint_Edit-5),50*(command_num%5+1)-30+32+j, 150+35*(2*Pinpoint_Edit-5)+25,50*(command_num%5+1)-30+32+j);
							}
						}
							if(command[command_num]==7)
						{
							 for(j=1;j<=3;j++)//����ʱ��ʾǰһ�ε�С�̺�
							{
								LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
							}
						}
					
//						if(command[command_num]==1)
//						{
//							Pinpoint_Edit++;
//							if(Pinpoint_Edit==5)
//							{
//								Pinpoint_Edit=3;
//							}
//						}
					
						if(command[command_num]==2)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=1;
							}
						}
						if(command[command_num]==3)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=1;
							}
						}
						if(command[command_num]==4)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						if(command[command_num]==5)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						if(command[command_num]==6)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						if(command[command_num]==7)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						if(command[command_num]==8)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						if(command[command_num]==20)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						if(command[command_num]==21)
						{
							Pinpoint_Edit++;
							if(Pinpoint_Edit==5)
							{
								Pinpoint_Edit=3;
							}
						}
						uc_lock=0;
					}
					
					
					
					
					if(edit_statement==2)
					{
						Pinpoint_Num++;
						if(Pinpoint_Num==17)
						{
							Pinpoint_Num=1;
						}
						uc_lock=0;
						Move_LR_Service();
					}
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>115&&tp_dev.y<161)//�Ӽ�
				{
					uc_lock=0;
				 jia_service();
					
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>115&&tp_dev.y<161)//����
				{
					uc_lock=0;
				 jian_service();
					
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>165&&tp_dev.y<211)//ɾ��
				{
					for(i=0;(i+command_num)<=100;i++)
					{
						command[command_num+i]=command[command_num+i+1];
						command_edit_num_wei[command_num+i][1]=command_edit_num_wei[command_num+i+1][1];
						command_edit_num_wei[command_num+i][2]=command_edit_num_wei[command_num+i+1][2];
						command_edit_num_wei[command_num+i][3]=command_edit_num_wei[command_num+i+1][3];
						command_edit_num_wei[command_num+i][4]=command_edit_num_wei[command_num+i+1][4];
					}
					uc_lock=0;
					display_command();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>165&&tp_dev.y<211)//���
				{
					for(i=99;i>command_num;i--)
					{
						command[i]=command[i-1];
						command_edit_num_wei[i][1]=command_edit_num_wei[i-1][1];
						command_edit_num_wei[i][2]=command_edit_num_wei[i-1][2];
						command_edit_num_wei[i][3]=command_edit_num_wei[i-1][3];
						command_edit_num_wei[i][4]=command_edit_num_wei[i-1][4];
					}
					command[command_num]=0;
					command_edit_num_wei[command_num][1]=0;
					command_edit_num_wei[command_num][2]=0;
					command_edit_num_wei[command_num][3]=0;
					command_edit_num_wei[command_num][4]=0;
					uc_lock=0;
					display_command();
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>215&&tp_dev.y<261)//����
				{
					save_command();
				}
					if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>215&&tp_dev.y<261)//����
				{
					if(save_ok==1)//
					{
								run_num=0;
								ScanCommandNum=0;//ÿ������ǰ��ʼ��ɨ��ָ�����Ϊ0��
								for(i=0;i<=9;i++)
								{
									ScanCommand[i]=0;
								}
								for(i=0;i<=99;i++)//ÿ������ǰ����λ��
								{
									if(command[i]==1)
									{
										Weiyi[i].Pulse_Num=Weiyi_Dis[i].Pulse_Num.shiwan*100000
																	+Weiyi_Dis[i].Pulse_Num.wan*10000
																	+Weiyi_Dis[i].Pulse_Num.qian*1000
																	+Weiyi_Dis[i].Pulse_Num.bai*100
																	+Weiyi_Dis[i].Pulse_Num.shi*10
																	+Weiyi_Dis[i].Pulse_Num.ge;//�õ�����λ�ƾ���
										Weiyi[i].Pulse_Num=Weiyi[i].Pulse_Num*PerPulseNum/PerDistance;//�õ��������
							
							
										Weiyi[i].Pulse_Rate=Weiyi_Dis[i].Pulse_Rate.wan*10000
																	+Weiyi_Dis[i].Pulse_Rate.qian*1000
																	+Weiyi_Dis[i].Pulse_Rate.bai*100
																	+Weiyi_Dis[i].Pulse_Rate.shi*10
																	+Weiyi_Dis[i].Pulse_Rate.ge;
										Weiyi[i].Speed_Acc=Weiyi_Dis[i].Pulse_Acc.wan*10000
																	+Weiyi_Dis[i].Pulse_Acc.qian*1000
																	+Weiyi_Dis[i].Pulse_Acc.bai*100
																	+Weiyi_Dis[i].Pulse_Acc.shi*10
																	+Weiyi_Dis[i].Pulse_Acc.ge;
									}
									
								}
								for(i=0;i<=99;i++)//�ж��Ƿ���ɨ��ָ��
								{
									if(command[i]==6)
									{
										ScanCommand[ScanCommandNum]=i;
										ScanCommandNum++;
									}
								}
								if(ScanCommandNum>0)//��ɨ��ָ��
								{
									edit_statement=7;
								}
								else
								{
									edit_statement=3;
									uc_lock=0;
								}
								 showhanzi32(395,220,30);//ֹͣ
								 showhanzi32(430,220,31);
							//	delay_ms(1000);
								//delay_ms(1000);
					}
					else
					{
						  LCD_Fill(80, 80, 250,150,WHITE);
	            LCD_DrawRectangle(80,80 ,250,150);
							showhanzi32(100,100,95);//����
							showhanzi32(132,100,96);
							showhanzi32(164,100,77);//����
							showhanzi32(196,100,78);
						  delay_ms(1000);
            	delay_ms(1000);
	            LCD_Fill(0, 0, 290,280,BACK_COL);
	            display_command();
					}
				}
				for(i=1;i<=5;i++)//�ж��Ƿ���ָ��ΰ���
				{
					if(tp_dev.x>80&&tp_dev.x<160&&tp_dev.y>(50*i-44)&&tp_dev.y<(50*i+3))
					{
						save_ok=0;//�ɱ༭״̬
						
						
						if(edit_statement==0)//�л�״̬ʱ��ԭ���ĺ��߲���
						{
							for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
							{
							LCD_DrawLine(60,50*(command_num%5+1)-30+32+j, 140,50*(command_num%5+1)-30+32+j);
							}
						}
							if(edit_statement==1)//�л�״̬ʱ��ԭ���ĺ��߲���
						{
							for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
							{
							 LCD_DrawLine(150+35*(Pinpoint_Edit-1),50*(command_num%5+1)-30+32+j, 150+35*(Pinpoint_Edit-1)+25,50*(command_num%5+1)-30+32+j);
							}
						}
						command_num=i+(page-1)*5-1;
						edit_statement=0;//�л�״̬
						display_command();
					}
				}
					for(i=1;i<=5;i++)//�ж��Ƿ��ǲ���������
				{
					if(tp_dev.x>170&&tp_dev.x<280&&tp_dev.y>(50*i-44)&&tp_dev.y<(50*i+3))
					{
						save_ok=0;//�ɱ༭״̬
						command_num=i+(page-1)*5-1;//�õ��ڼ��а���
						
						if(command[command_num]==1)
						{
							edit_statement=2;
							init_lock=1;
						  state=command_num;//����ǰ�ڼ���λ�Ƹ�ֵ��state
						}
					
					}
				}
				
				
				
				
				
				
				
				
				

			}
			
		}
	}

	else
	{
		uc_lock=1;
	}
	
}
void display_command(void)//������ʾָ����м���ָ������������갴��֮�����
{
	u8 i,j,m;
	for(i=0;i<=99;i++)
	{
		if(command[i]==0)
		{
			  command_edit_num[i]=0;
		}
		else
		{
		  command_edit_num[i]=command_edit_num_wei[i][1]*1000
												 +command_edit_num_wei[i][2]*100
												 +command_edit_num_wei[i][3]*10
												 +command_edit_num_wei[i][4];			
		}
	}
	for(i=1;i<=5;i++)//��ʾ������
	{
		LCD_ShowNum_32(10,50*i-30,(page-1)*5/10);
		LCD_ShowNum_32(26,50*i-30,(page-1)*5%10+i-1);
	}
	for(i=1;i<=5;i++)//��ʾָ��
	{
		LCD_Fill(60,50*i-30,140,50*i+2,WHITE);//�������
		LCD_Fill(150,50*i-30,280,50*i+2,WHITE);
		if(command[i+(page-1)*5-1]==0)
		{
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
			{
			 LCD_DrawLine(60,50*i-30+32+j, 140,50*i-30+32+j);
			 LCD_DrawLine(150,50*i-30+32+j, 280,50*i-30+32+j);
			}	
		}
		if(command[i+(page-1)*5-1]==1)//λ��
		{
			LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
			{
			 LCD_DrawLine(60,50*i-30+32+j, 140,50*i-30+32+j);
			 LCD_DrawLine(150,50*i-30+32+j, 280,50*i-30+32+j);
			}
		
			
			showhanzi32(60,50*i-30,6);//λ��                            
	    showhanzi32(100,50*i-30,7);
			
			
			showhanzi32(150,50*i-30,43);//���
			showhanzi32(182,50*i-30,44);
			showhanzi32(214,50*i-30,45);//����
			showhanzi32(246,50*i-30,46);
			
			//showhanzi32(150,50*i-30,5);//�κ�
			//showhanzi32(182,50*i-30,52);
			
			//LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
		//	LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
			
		}
		if(command[i+(page-1)*5-1]==2)//ʱ��
		{
			showhanzi32(60,50*i-30,32);//ʱ��                           
	    showhanzi32(100,50*i-30,33);
			LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			for(m=0;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}

					LCD_ShowNum_32(155,50*i-30,command_edit_num_wei[i+(page-1)*5-1][1]);
					LCD_ShowNum_32(190,50*i-30,command_edit_num_wei[i+(page-1)*5-1][2]);
					LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
					LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
		if(command[i+(page-1)*5-1]==3)//
		{
			showhanzi32(60,50*i-30,16);//�ٶ�                          
	    showhanzi32(100,50*i-30,17);
			LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			for(m=0;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
					LCD_ShowNum_32(155,50*i-30,command_edit_num_wei[i+(page-1)*5-1][1]);
					LCD_ShowNum_32(190,50*i-30,command_edit_num_wei[i+(page-1)*5-1][2]);
					LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
					LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
		if(command[i+(page-1)*5-1]==4)//����
		{
			LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			for(m=0;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			showhanzi32(60,50*i-30,34);//����                          
	    showhanzi32(100,50*i-30,35);
			
			LCD_ShowNum_32(155,50*i-30,10);//X
			showhanzi32(215,50*i-30,51);
			
			LCD_ShowNum_32(190,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
		if(command[i+(page-1)*5-1]==5)//���
		{
			LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			for(m=0;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			showhanzi32(60,50*i-30,34);//���                          
	    showhanzi32(100,50*i-30,36);
			
			LCD_ShowNum_32(155,50*i-30,11);//Y
			showhanzi32(215,50*i-30,51);//Ϊ
			LCD_ShowNum_32(190,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
		if(command[i+(page-1)*5-1]==6)//ɨ��
		{
			LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			for(m=0;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			showhanzi32(60,50*i-30,37);//ɨ��                          
	    showhanzi32(100,50*i-30,38);
			
			LCD_ShowNum_32(155,50*i-30,10);//X
			showhanzi32(215,50*i-30,51);//Ϊ
			LCD_ShowNum_32(190,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
		if(command[i+(page-1)*5-1]==7)//��ת
		{
				LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			for(m=0;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			showhanzi32(60,50*i-30,39);//��ת                       
	    showhanzi32(100,50*i-30,40);
			
			LCD_ShowNum_32(155,50*i-30,command_edit_num_wei[i+(page-1)*5-1][1]);
			LCD_ShowNum_32(190,50*i-30,command_edit_num_wei[i+(page-1)*5-1][2]);
			LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
			
		}
			if(command[i+(page-1)*5-1]==8)//ѭ��
		{
				LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
			{
			 LCD_DrawLine(60,50*i-30+32+j, 140,50*i-30+32+j);
			 LCD_DrawLine(150,50*i-30+32+j, 210,50*i-30+32+j);
			}
				for(m=2;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			
			showhanzi32(60,50*i-30,60);//ѭ��                            
	    showhanzi32(100,50*i-30,61);
			
			showhanzi32(150,50*i-30,54);//���
			showhanzi32(182,50*i-30,55);
			
			LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
			if(command[i+(page-1)*5-1]==9)//����
		{
			for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
			{
			 LCD_DrawLine(60,50*i-30+32+j, 140,50*i-30+32+j);
			 LCD_DrawLine(150,50*i-30+32+j, 280,50*i-30+32+j);
			}
		//	LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			showhanzi32(60,50*i-30,41);//����                         
	    showhanzi32(100,50*i-30,42);
		}
			if(command[i+(page-1)*5-1]==20)
		{
				LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			LCD_Fill(60, 50*i-30+32+1, 140,50*i-30+32+3,WHITE);
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
			{
			 LCD_DrawLine(150,50*i-30+32+j, 210,50*i-30+32+j);
			}
				for(m=2;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			
			
			showhanzi32(150,50*i-30,56);//�յ�
			showhanzi32(182,50*i-30,57);
			
			LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
			if(command[i+(page-1)*5-1]==21)
		{
				LCD_Fill(150, 50*i-30+32+1, 280,50*i-30+32+3,WHITE);//������0
			LCD_Fill(60, 50*i-30+32+1, 140,50*i-30+32+3,WHITE);
				for(j=1;j<=3;j++)//��ʾ��������ĺ��߶�
			{
			 LCD_DrawLine(150,50*i-30+32+j, 210,50*i-30+32+j);
			}
				for(m=2;m<=3;m++)
			{
				for(j=1;j<=3;j++)
				{
					LCD_DrawLine(150+35*m,50*i-30+32+j, 150+35*m+25,50*i-30+32+j);
				}
			}
			
			
			showhanzi32(150,50*i-30,58);//�յ�
			showhanzi32(182,50*i-30,59);
			
			LCD_ShowNum_32(225,50*i-30,command_edit_num_wei[i+(page-1)*5-1][3]);
			LCD_ShowNum_32(260,50*i-30,command_edit_num_wei[i+(page-1)*5-1][4]);
		}
	}
}
void save_command(void)
{
	u8 i;
	save_ok=1;
	
		for(i=0;i<=99;i++)//����洢��λ��
  {
		if(command[i]==1)
		{
		Weiyi[i].Pulse_Num=Weiyi_Dis[i].Pulse_Num.shiwan*100000
											+Weiyi_Dis[i].Pulse_Num.wan*10000
											+Weiyi_Dis[i].Pulse_Num.qian*1000
											+Weiyi_Dis[i].Pulse_Num.bai*100
											+Weiyi_Dis[i].Pulse_Num.shi*10
											+Weiyi_Dis[i].Pulse_Num.ge;//�õ�����λ�ƾ���
									
		Weiyi[i].Pulse_Rate=Weiyi_Dis[i].Pulse_Rate.wan*10000
												+Weiyi_Dis[i].Pulse_Rate.qian*1000
												+Weiyi_Dis[i].Pulse_Rate.bai*100
												+Weiyi_Dis[i].Pulse_Rate.shi*10
												+Weiyi_Dis[i].Pulse_Rate.ge;
		Weiyi[i].Speed_Acc=Weiyi_Dis[i].Pulse_Acc.wan*10000
												+Weiyi_Dis[i].Pulse_Acc.qian*1000
												+Weiyi_Dis[i].Pulse_Acc.bai*100
												+Weiyi_Dis[i].Pulse_Acc.shi*10
												+Weiyi_Dis[i].Pulse_Acc.ge;
		}
									
	}
	if(Select_Pro==1)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)command,SIZE);
		STMFLASH_Write(FLASH_SAVE_EDIT_ADDR,(u16*)command_edit_num,SIZE);
		STMFLASH_Write(FLASH_SAVE_WEIYI_ADDR,(u16*)Weiyi,SIZE);
	}
	if(Select_Pro==2)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR+1600,(u16*)command,SIZE);
		STMFLASH_Write(FLASH_SAVE_EDIT_ADDR+1600,(u16*)command_edit_num,SIZE);
		STMFLASH_Write(FLASH_SAVE_WEIYI_ADDR+1600,(u16*)Weiyi,SIZE);
	}
	if(Select_Pro==3)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR+3200,(u16*)command,SIZE);
		STMFLASH_Write(FLASH_SAVE_EDIT_ADDR+3200,(u16*)command_edit_num,SIZE);
		STMFLASH_Write(FLASH_SAVE_WEIYI_ADDR+3200,(u16*)Weiyi,SIZE);
	}

	LCD_Fill(80, 80, 250,150,WHITE);
	LCD_DrawRectangle(80,80 ,250,150);
	
	showhanzi32(100,100,77);//����
	showhanzi32(132,100,78);
	showhanzi32(164,100,81);//�ɹ�
	showhanzi32(196,100,82);
  delay_ms(1000);
	delay_ms(1000);
	//LCD_Fill(0, 0, 290,280,BACK_COL);
	init_lock=1;
}
void read_command()
{
	u8 i;
		if(Select_Pro==1)
							{
								STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)command,SIZE);
								STMFLASH_Read(FLASH_SAVE_EDIT_ADDR,(u16*)command_edit_num,SIZE);
								STMFLASH_Read(FLASH_SAVE_WEIYI_ADDR,(u16*)Weiyi,SIZE);
							}
							if(Select_Pro==2)
							{
								STMFLASH_Read(FLASH_SAVE_ADDR+1600,(u16*)command,SIZE);
								STMFLASH_Read(FLASH_SAVE_EDIT_ADDR+1600,(u16*)command_edit_num,SIZE);
								STMFLASH_Read(FLASH_SAVE_WEIYI_ADDR+1600,(u16*)Weiyi,SIZE);
							}
							if(Select_Pro==3)
							{
								STMFLASH_Read(FLASH_SAVE_ADDR+3200,(u16*)command,SIZE);
								STMFLASH_Read(FLASH_SAVE_EDIT_ADDR+3200,(u16*)command_edit_num,SIZE);
								STMFLASH_Read(FLASH_SAVE_WEIYI_ADDR+3200,(u16*)Weiyi,SIZE);
							}
							for(i=0;i<100;i++)
							{
								command_edit_num_wei[i][1]=command_edit_num[i]/1000;
								command_edit_num_wei[i][2]=(command_edit_num[i]-command_edit_num_wei[i][1]*1000)/100;
								command_edit_num_wei[i][3]=(command_edit_num[i]-command_edit_num_wei[i][1]*1000-command_edit_num_wei[i][2]*100)/10;
								command_edit_num_wei[i][4]=command_edit_num[i]-command_edit_num_wei[i][1]*1000-command_edit_num_wei[i][2]*100-command_edit_num_wei[i][3]*10;
							 
								Weiyi_Dis[i].Pulse_Num.shiwan=Weiyi[i].Pulse_Num/100000;
								Weiyi_Dis[i].Pulse_Num.wan=(Weiyi[i].Pulse_Num-Weiyi_Dis[i].Pulse_Num.shiwan*100000)/10000;
								Weiyi_Dis[i].Pulse_Num.qian=(Weiyi[i].Pulse_Num-Weiyi_Dis[i].Pulse_Num.shiwan*100000-Weiyi_Dis[i].Pulse_Num.wan*10000)/1000;
								Weiyi_Dis[i].Pulse_Num.bai=(Weiyi[i].Pulse_Num-Weiyi_Dis[i].Pulse_Num.shiwan*100000-Weiyi_Dis[i].Pulse_Num.wan*10000-Weiyi_Dis[i].Pulse_Num.qian*1000)/100;
								Weiyi_Dis[i].Pulse_Num.shi=(Weiyi[i].Pulse_Num-Weiyi_Dis[i].Pulse_Num.shiwan*100000-Weiyi_Dis[i].Pulse_Num.wan*10000-Weiyi_Dis[i].Pulse_Num.qian*1000-Weiyi_Dis[i].Pulse_Num.bai*100)/10;
								Weiyi_Dis[i].Pulse_Num.ge=(Weiyi[i].Pulse_Num-Weiyi_Dis[i].Pulse_Num.shiwan*100000-Weiyi_Dis[i].Pulse_Num.wan*10000-Weiyi_Dis[i].Pulse_Num.qian*1000-Weiyi_Dis[i].Pulse_Num.bai*100-Weiyi_Dis[i].Pulse_Num.shi*10)/1;

							  Weiyi_Dis[i].Pulse_Rate.shiwan=Weiyi[i].Pulse_Rate/100000;
								Weiyi_Dis[i].Pulse_Rate.wan=(Weiyi[i].Pulse_Rate-Weiyi_Dis[i].Pulse_Rate.shiwan*100000)/10000;
								Weiyi_Dis[i].Pulse_Rate.qian=(Weiyi[i].Pulse_Rate-Weiyi_Dis[i].Pulse_Rate.shiwan*100000-Weiyi_Dis[i].Pulse_Rate.wan*10000)/1000;
								Weiyi_Dis[i].Pulse_Rate.bai=(Weiyi[i].Pulse_Rate-Weiyi_Dis[i].Pulse_Rate.shiwan*100000-Weiyi_Dis[i].Pulse_Rate.wan*10000-Weiyi_Dis[i].Pulse_Rate.qian*1000)/100;
								Weiyi_Dis[i].Pulse_Rate.shi=(Weiyi[i].Pulse_Rate-Weiyi_Dis[i].Pulse_Rate.shiwan*100000-Weiyi_Dis[i].Pulse_Rate.wan*10000-Weiyi_Dis[i].Pulse_Rate.qian*1000-Weiyi_Dis[i].Pulse_Rate.bai*100)/10;
								Weiyi_Dis[i].Pulse_Rate.ge=(Weiyi[i].Pulse_Rate-Weiyi_Dis[i].Pulse_Rate.shiwan*100000-Weiyi_Dis[i].Pulse_Rate.wan*10000-Weiyi_Dis[i].Pulse_Rate.qian*1000-Weiyi_Dis[i].Pulse_Rate.bai*100-Weiyi_Dis[i].Pulse_Rate.shi*10)/1;
							
							  Weiyi_Dis[i].Pulse_Acc.shiwan=Weiyi[i].Speed_Acc/100000;
								Weiyi_Dis[i].Pulse_Acc.wan=(Weiyi[i].Speed_Acc-Weiyi_Dis[i].Pulse_Acc.shiwan*100000)/10000;
								Weiyi_Dis[i].Pulse_Acc.qian=(Weiyi[i].Speed_Acc-Weiyi_Dis[i].Pulse_Acc.shiwan*100000-Weiyi_Dis[i].Pulse_Acc.wan*10000)/1000;
								Weiyi_Dis[i].Pulse_Acc.bai=(Weiyi[i].Speed_Acc-Weiyi_Dis[i].Pulse_Acc.shiwan*100000-Weiyi_Dis[i].Pulse_Acc.wan*10000-Weiyi_Dis[i].Pulse_Acc.qian*1000)/100;
								Weiyi_Dis[i].Pulse_Acc.shi=(Weiyi[i].Speed_Acc-Weiyi_Dis[i].Pulse_Acc.shiwan*100000-Weiyi_Dis[i].Pulse_Acc.wan*10000-Weiyi_Dis[i].Pulse_Acc.qian*1000-Weiyi_Dis[i].Pulse_Acc.bai*100)/10;
								Weiyi_Dis[i].Pulse_Acc.ge=(Weiyi[i].Speed_Acc-Weiyi_Dis[i].Pulse_Acc.shiwan*100000-Weiyi_Dis[i].Pulse_Acc.wan*10000-Weiyi_Dis[i].Pulse_Acc.qian*1000-Weiyi_Dis[i].Pulse_Acc.bai*100-Weiyi_Dis[i].Pulse_Acc.shi*10)/1;
							
							
							}
							if(edit_statement==0||edit_statement==1)
							{
								display_command();
							}
							for(k=0;k<=100;k++)
							{
								if(command[k]==0)
								{
										command_max=k-1;
									if(command_max==255)
									{
										command_max=0;
									}
										break;
								}
							}
							page_max=command_max/5+1;
}
void Run(void)//����ָ��
{
	switch(command[run_num])                                                                                                                                                                        
	{
		case 1:Run_WeiYi(Weiyi[run_num].Direction,run_num);break;
		case 2:Run_Time(command_edit_num[run_num]);break;
		case 3:Run_Speed(command_edit_num[run_num]);break;
		case 4:Run_Input(command_edit_num_wei[run_num][3],command_edit_num_wei[run_num][4]);break;
		case 5:Run_Output(command_edit_num_wei[run_num][3],command_edit_num_wei[run_num][4]);break;
		case 7:Run_Return(command_edit_num[run_num],&run_num);break;
		case 8:Run_End(command_edit_num[run_num],command_edit_num[run_num+1],command_edit_num[run_num+2],&run_num);break;
	}
	delay_ms(5);
	run_num++;
	if(command[run_num]==0)
	{
								 end=1;
						pulse_count=0;
						p1=0;
						speed_lock=1;
						init_lock=1;
					  edit_statement=0;
	}
	
}
void Run_WeiYi(u8 dir,u16 num)
{
	end=0;
	time_state=1;
	if(dir==0)
	{
		RunDirection=1;
	}
	else
	{
		RunDirection=0;
	}
	pwm_speed(Weiyi[num].Speed_Acc,Weiyi[num].Pulse_Rate,Weiyi[num].Pulse_Num);
	while(end==0)
	{
		tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
			
			delay_ms(10);
			if(tp_dev.sta&TP_PRES_DOWN)
			{
					if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>190&&tp_dev.y<245)//ֹͣ��
					{
						 end=1;
						pulse_count=0;
						p1=0;
						speed_lock=1;
						init_lock=1;
						TIM_Cmd(TIM3, DISABLE);
					   edit_statement=0;
						 break;
					}
			}
		}
	};//����û��������ѭ��
	
}
void Run_Time(u16 num)
{
	int i;
	for(i=0;i<num;i++)
	{
		delay_ms(100);
    tp_dev.scan(0);
	 	if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
			delay_ms(10);
			if(tp_dev.sta&TP_PRES_DOWN)
			{
					if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>190&&tp_dev.y<245)//ֹͣ��
					{
						 
					   edit_statement=2;
						 break;
					}
				
			}
		}
	}
}
void Run_Speed(u16 num)
{
	time_state=3;//���ٶ�ָ��
	if(num==0)
	{
			end=1;
			pulse_count=0;
			p1=0;
			speed_lock=1;
			TIM_Cmd(TIM3, DISABLE);
	}else
	{
	pwm_speed(1,num,10000);
	}
}
void Run_Input(u16 Xi,u16 Xstate)
{
	int GO_ON=0;
		switch (Xi)
		{
			case 1:while(GO_ON==0)
						{
							if(X1==Xstate)
							{
								delay_ms(10);
								if(X1==Xstate)
								{
									GO_ON=1;
								}
							}
						}
							break;
			case 2:while(GO_ON==0)
						{
							if(X2==Xstate)
							{
								delay_ms(10);
								if(X2==Xstate)
								{
									GO_ON=1;
								}
							}
						}
							break;
			case 3:while(GO_ON==0)
						{
							if(X3==Xstate)
							{
								delay_ms(10);
								if(X3==Xstate)
								{
									GO_ON=1;
								}
							}
						}
							break;
			case 4:while(GO_ON==0)
						{
							if(X4==Xstate)
							{
								delay_ms(10);
								if(X4==Xstate)
								{
									GO_ON=1;
								}
							}
						}
							break;	
			case 5:while(GO_ON==0)
						{
							if(X5==Xstate)
							{
								delay_ms(10);
								if(X5==Xstate)
								{
									GO_ON=1;
								}
							}
						}
							break;		
			case 6:while(GO_ON==0)
						{
							if(X6==Xstate)
							{
								delay_ms(10);
								if(X6==Xstate)
								{
									GO_ON=1;
								}
							}
						}
							break;
		}
	

	
}
void Run_Output(u16 led ,u16 Xstate)
{
	if(Xstate==1)
	{
			switch(led)
		{
			case 1:LED0=1;break;
			case 2:LED1=1;break;
		}
	}
		if(Xstate==0)
	{
			switch(led)
		{
			case 1:LED0=0;break;
			case 2:LED1=0;break;
		}
	}
}
void Run_Scan(void)
{
	int i;
	int scan;
			tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
			
			delay_ms(10);
			if(tp_dev.sta&TP_PRES_DOWN)
			{
					if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>215&&tp_dev.y<261)//ֹͣ��
					{
						 end=1;
						pulse_count=0;
						p1=0;
						speed_lock=1;
						TIM_Cmd(TIM3, DISABLE);
						init_lock=1;
					   edit_statement=0;
					}
			}
		}
		
			
	for(i=0;i<ScanCommandNum;i++)//ɨ��˿ڣ�ǰ��Ϊ�˿ںţ�����Ϊ��ƽ״̬
	{
		scan=Scan_GPIO(command_edit_num_wei[ScanCommand[i]][3],command_edit_num_wei[ScanCommand[i]][4]);
		if(scan==ScanOK)
		{
			Run_Scan_Service(ScanCommand[i]+1,99);
		}
	}
}
void Run_Return(u16 num,u16 *p)
{
	*p=num-1;
}
void Run_End(u16 startnum,u16 endnum,u16 num,u16 *p)
{
	u16 i,j;
	for(i=1;i<=num;i++)
	{
		for(j=startnum;j<=endnum;j++)
		{
				switch(command[j])                                                                                                                                                                        
			{
			case 1:Run_WeiYi(Weiyi[j].Direction,j);break;
			case 2:Run_Time(command_edit_num[j]);break;
			case 3:Run_Speed(command_edit_num[j]);break;
			case 4:Run_Input(command_edit_num_wei[run_num][3],command_edit_num_wei[run_num][4]);break;
			case 5:Run_Output(command_edit_num_wei[run_num][3],command_edit_num_wei[run_num][4]);break;
			case 7:Run_Return(command_edit_num[j],&j);break;
			case 8:Run_End(command_edit_num[j],command_edit_num[j+1],command_edit_num[j+2],&j);break;
			}
		}
	}
	*p=*p+2;
	
}
void set_system(void)
{
	int i;
		if(init_lock)//�л�״̬ʱ��ʼ��һ��
	{
		LCD_Clear(WHITE);
		init_lock=0;
		flicker_count=0;
		edit_init();
	}
	LCD_ShowNum_32(270,80,Select_Pro);
	
	showhanzi32(0,70,63);                                //ѡ�����
	showhanzi32(36,70,64);                               
  showhanzi32(72,70,65);                              
	showhanzi32(108,70,66);                             

	showhanzi32(0,140,63);                           //ѡ��ģʽ  
	showhanzi32(36,140,64);                              
  showhanzi32(72,140,83);                             
	showhanzi32(108,140,84);        
	if(Mode==Auto_Mode)//�Զ�
	{
		showhanzi32(222,140,85);                             
	  showhanzi32(254,140,86);    
	}
	if(Mode==Hand_Mode)//�ֶ�
	{
		showhanzi32(222,140,87);                             
	  showhanzi32(254,140,88);    
	}

	
	tp_dev.scan(0);
	flicker_count++;

	Short_Line_flicker();

	
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{	
		delay_ms(10);
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			if(uc_lock==1)//��ֹ�ظ�����
			{
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>10&&tp_dev.y<65)//��ҳ��
				{
					edit_statement=5;
					init_lock=1;
					Pinpoint_Num=1;
					uc_lock=0;
				}
		
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>10&&tp_dev.y<65)//���ؼ�
				{
					edit_statement=0;
					Pinpoint_Num=1;
					init_lock=1;
					uc_lock=0;
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num--;
					if(Pinpoint_Num==0)
					{
						Pinpoint_Num=2;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num++;
					if(Pinpoint_Num==3)
					{
						Pinpoint_Num=1;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>130&&tp_dev.y<185)//�Ӽ�
				{
					uc_lock=0;
				 jia_service();
					
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>130&&tp_dev.y<185)//����
				{
					uc_lock=0;
				 jian_service();
					
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>190&&tp_dev.y<245)//��ȡ
				{
          read_command();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>190&&tp_dev.y<245)//����
				{
					save_command();
				}
			
			}
			
		}
	}
	else
	{
		uc_lock=1;
	}
	
	
}
void set_system_2(void)
{
	  int i;
		if(init_lock)//�л�״̬ʱ��ʼ��һ��
	{
		
		init_lock=0;
		flicker_count=0;
	LCD_Clear(WHITE);
	showhanzi32(0,20,85);                             //�Զ�ģʽ����
	showhanzi32(36,20,86);
	showhanzi32(72,20,83);                              
	showhanzi32(108,20,84);
  showhanzi32(140,20,3);                             
	showhanzi32(172,20,62);	
		                          

	showhanzi32(0,140,71);                           //һȦ����
	showhanzi32(36,140,72);                              
  showhanzi32(72,140,67);                             
	showhanzi32(108,140,68);                            
	
	showhanzi32(0,200,71);                             //һȦ����
	showhanzi32(36,200,72);                            
  showhanzi32(72,200,73);                             
	showhanzi32(108,200,74); 
	
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
	LCD_ShowNum_32(198,140,Pulse_Dangliang.wan);//���������ֵ
	LCD_ShowNum_32(216,140,Pulse_Dangliang.qian);
	LCD_ShowNum_32(234,140,Pulse_Dangliang.bai);
	LCD_ShowNum_32(252,140,Pulse_Dangliang.shi);
	LCD_ShowNum_32(270,140,Pulse_Dangliang.ge);
	
	LCD_ShowNum_32(198,200,Distance_Per.wan);//ÿȦ������ֵ
	LCD_ShowNum_32(216,200,Distance_Per.qian);
	LCD_ShowNum_32(234,200,Distance_Per.bai);
	LCD_ShowNum_32(252,200,Distance_Per.shi);
	LCD_ShowNum_32(270,200,Distance_Per.ge);
		edit_init();
	}
	
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
	LCD_ShowNum_32(198,140,Pulse_Dangliang.wan);//�������
	LCD_ShowNum_32(216,140,Pulse_Dangliang.qian);
	LCD_ShowNum_32(234,140,Pulse_Dangliang.bai);
	LCD_ShowNum_32(252,140,Pulse_Dangliang.shi);
	LCD_ShowNum_32(270,140,Pulse_Dangliang.ge);
	
	LCD_ShowNum_32(198,200,Distance_Per.wan);//ÿȦ����
	LCD_ShowNum_32(216,200,Distance_Per.qian);
	LCD_ShowNum_32(234,200,Distance_Per.bai);
	LCD_ShowNum_32(252,200,Distance_Per.shi);
	LCD_ShowNum_32(270,200,Distance_Per.ge);
	
	tp_dev.scan(0);
	flicker_count++;

	Short_Line_flicker();

	
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{	
		delay_ms(10);
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			if(uc_lock==1)//��ֹ�ظ�����
			{
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>10&&tp_dev.y<65)//��ҳ��
				{
					
					edit_statement=6;
						Pinpoint_Num=1;
					init_lock=1;
					uc_lock=0;
				}
		
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>10&&tp_dev.y<65)//���ؼ�
				{
					edit_statement=0;
					Pinpoint_Num=1;
					init_lock=1;
					uc_lock=0;
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num--;
					if(Pinpoint_Num==0)
					{
						Pinpoint_Num=10;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num++;
					if(Pinpoint_Num==11)
					{
						Pinpoint_Num=1;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>130&&tp_dev.y<185)//�Ӽ�
				{
					uc_lock=0;
				 jia_service();
					
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>130&&tp_dev.y<185)//����
				{
					uc_lock=0;
				 jian_service();
					
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>190&&tp_dev.y<245)//��ȡ
				{
            read_command();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>190&&tp_dev.y<245)//����
				{
						save_command();
				}

			}
			
		}
	}
	else
	{
		uc_lock=1;
	}
	
	
}

void set_system_3(void)
{
	  int i;
		if(init_lock)//�л�״̬ʱ��ʼ��һ��
	{
		
		init_lock=0;
		flicker_count=0;
	LCD_Clear(WHITE);
	showhanzi32(0,20,87);                             //�ֶ�ģʽ����
	showhanzi32(36,20,88);
	showhanzi32(72,20,83);                              
	showhanzi32(108,20,84);
  showhanzi32(140,20,3);                             
	showhanzi32(172,20,62);	
		                          

	showhanzi32(0,140,57);                           //�㶯����
	showhanzi32(36,140,88);                              
  showhanzi32(72,140,67);                             
	showhanzi32(108,140,68);                            
	
	showhanzi32(0,200,89);                             //����Ƶ��
	showhanzi32(36,200,90);                            
  showhanzi32(72,200,12);                             
	showhanzi32(108,200,13); 
	
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
	LCD_ShowNum_32(198,140,Dot_Pulse.wan);//�㶯����
	LCD_ShowNum_32(216,140,Dot_Pulse.qian);
	LCD_ShowNum_32(234,140,Dot_Pulse.bai);
	LCD_ShowNum_32(252,140,Dot_Pulse.shi);
	LCD_ShowNum_32(270,140,Dot_Pulse.ge);
	
	LCD_ShowNum_32(198,200,Constant_Speed.wan);//����Ƶ��
	LCD_ShowNum_32(216,200,Constant_Speed.qian);
	LCD_ShowNum_32(234,200,Constant_Speed.bai);
	LCD_ShowNum_32(252,200,Constant_Speed.shi);
	LCD_ShowNum_32(270,200,Constant_Speed.ge);
		edit_init();
	}
	
	
	//LCD_ShowNum_32(180,80,Pulse_Num_dis_01.shiwan);//
	LCD_ShowNum_32(198,140,Dot_Pulse.wan);//�㶯����
	LCD_ShowNum_32(216,140,Dot_Pulse.qian);
	LCD_ShowNum_32(234,140,Dot_Pulse.bai);
	LCD_ShowNum_32(252,140,Dot_Pulse.shi);
	LCD_ShowNum_32(270,140,Dot_Pulse.ge);
	
	LCD_ShowNum_32(198,200,Constant_Speed.wan);//����Ƶ��
	LCD_ShowNum_32(216,200,Constant_Speed.qian);
	LCD_ShowNum_32(234,200,Constant_Speed.bai);
	LCD_ShowNum_32(252,200,Constant_Speed.shi);
	LCD_ShowNum_32(270,200,Constant_Speed.ge);
	
	tp_dev.scan(0);
	flicker_count++;

	Short_Line_flicker();

	
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{	
		delay_ms(10);
		if(tp_dev.sta&TP_PRES_DOWN)
		{
			if(uc_lock==1)//��ֹ�ظ�����
			{
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>10&&tp_dev.y<65)//��ҳ��
				{
					
					edit_statement=4;
						Pinpoint_Num=1;
					init_lock=1;
					uc_lock=0;
				}
		
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>10&&tp_dev.y<65)//���ؼ�
				{
					edit_statement=0;
					Pinpoint_Num=1;
					init_lock=1;
					uc_lock=0;
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num--;
					if(Pinpoint_Num==0)
					{
						Pinpoint_Num=10;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>70&&tp_dev.y<125)//���Ƽ�
				{
					Pinpoint_Num++;
					if(Pinpoint_Num==11)
					{
						Pinpoint_Num=1;
					}
					uc_lock=0;
					Move_LR_Service();
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>130&&tp_dev.y<185)//�Ӽ�
				{
					uc_lock=0;
				 jia_service();
					
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>130&&tp_dev.y<185)//����
				{
					uc_lock=0;
				 jian_service();
					
				}
				if(tp_dev.x>300&&tp_dev.x<380&&tp_dev.y>190&&tp_dev.y<245)//��ȡ
				{
						read_command();
				}
				if(tp_dev.x>390&&tp_dev.x<470&&tp_dev.y>190&&tp_dev.y<245)//����
				{
						save_command();
				}
			}
			
		}
	}
	else
	{
		uc_lock=1;
	}
	
	
}
void save_edit_num(void)
{
	
}
int Scan_GPIO(u16 Xi,u16 Xstate)
{
	  
			switch (Xi)
		{
			case 1:if(X1==Xstate)
							{
								delay_ms(10);
								if(X1==Xstate)
								{
									return ScanOK;
								}
								else return ScanError;
							}
							else return ScanError;
							break;
			case 2:if(X2==Xstate)
							{
								delay_ms(10);
								if(X2==Xstate)
								{
									return ScanOK;
								}
								else return ScanError;
							}
							else return ScanError;
							break;
			case 3:if(X3==Xstate)
							{
								delay_ms(10);
								if(X3==Xstate)
								{
									return ScanOK;
								}
								else return ScanError;
							}
							else return ScanError;
							break;
			case 4:if(X4==Xstate)
							{
								delay_ms(10);
								if(X4==Xstate)
								{
									return ScanOK;
								}
								else return ScanError;
							}
							else return ScanError;
							break;	
			case 5:if(X5==Xstate)
							{
								delay_ms(10);
								if(X5==Xstate)
								{
									return ScanOK;
								}
								else return ScanError;
							}
							else return ScanError;
							break;		
			case 6:if(X6==Xstate)
							{
								delay_ms(10);
								if(X6==Xstate)
								{
									return ScanOK;
								}
								else return ScanError;
							}
							else return ScanError;
							break;
		}
}
void Run_Scan_Service(u16 startnum,u16 endnum)
{
	u16 i,j;
		for(j=startnum;j<=endnum;j++)
		{
				switch(command[j])                                                                                                                                                                        
			{
					case 1:Run_WeiYi(Weiyi[j].Direction,j);break;
					case 2:Run_Time(command_edit_num[j]);break;
					case 3:Run_Speed(command_edit_num[j]);break;
					case 4:Run_Input(command_edit_num_wei[run_num][3],command_edit_num_wei[run_num][4]);break;
					case 5:Run_Output(command_edit_num_wei[run_num][3],command_edit_num_wei[run_num][4]);break;
				  case 6:j=endnum+1;break;
					case 7:Run_Return(command_edit_num[j],&j);break;
					case 8:Run_End(command_edit_num[j],command_edit_num[j+1],command_edit_num[j+2],&j);break;
					case 9:j=end+1;break;
			}
		}
	
}
