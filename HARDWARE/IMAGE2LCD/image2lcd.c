#include "sys.h"
#include "lcd.h"
#include "image2lcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//FLASHͼƬ��ʾ	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2011/10/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved		
//********************************************************************************
//V1.1�޸�˵�� 20121027
//1����Ӷ�ILI9341 LCD��֧��
//2����LCD_Set_Window���뱾���루��ΪILI93xx.c���ٰ����ú�����
//3���޸�image_show����֧��ILI9341��     
////////////////////////////////////////////////////////////////////////////////// 	
     	 
//���ô���
//sx,sy,ex,ey��������
//���ڴ�С:(ex-sx+1)*(ey-ex+1)
//ע��,ȷ��ex>=sx;ey>=sy!!!!
//9320/9325/9328/4531/4535/1505/b505/8989��IC�Ѿ�ʵ�ʲ���	   
void LCD_Set_Window(u16 sx,u16 sy,u16 ex,u16 ey)	   
{
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval;  
// #if USE_HORIZONTAL  //ʹ�ú���	
	//����ֵ
	hsaval=sy;				
	heaval=ey;
	vsaval=479-ex;
	veaval=479-sx;	
// #else				//����
// 	//����ֵ
// 	hsaval=sx;				
// 	heaval=ex;
// 	vsaval=sy;
// 	veaval=ey;	
// #endif
	if(lcddev.id==0X9341)
	{
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(hsaval>>8); 
		LCD_WR_DATA(hsaval&0XFF);	 
		LCD_WR_DATA(heaval>>8); 
		LCD_WR_DATA(heaval&0XFF);	 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(vsaval>>8); 
		LCD_WR_DATA(vsaval&0XFF);	 
		LCD_WR_DATA(veaval>>8); 
		LCD_WR_DATA(veaval&0XFF);	 
	}else
	{
	 	if(lcddev.id==0X8989)//8989 IC
		{
			hsareg=0X44;heareg=0X44;//ˮƽ���򴰿ڼĴ��� (1289����һ���Ĵ�������)
			hsaval|=(heaval<<8);	//�õ��Ĵ���ֵ.
			heaval=hsaval;
			vsareg=0X45;veareg=0X46;//��ֱ���򴰿ڼĴ���	  
		}else  //��������IC
		{
			hsareg=0X50;heareg=0X51;//ˮƽ���򴰿ڼĴ���
			vsareg=0X52;veareg=0X53;//��ֱ���򴰿ڼĴ���	  
		}								  
		//���üĴ���ֵ
		LCD_WR_REG_DATA(hsareg,hsaval);
		LCD_WR_REG_DATA(heareg,heaval);
		LCD_WR_REG_DATA(vsareg,vsaval);
		LCD_WR_REG_DATA(veareg,veaval);
	}
} 


//��8λ���ݻ��16λ��ɫ
//mode:0,��λ��ǰ,��λ�ں�.
//     1,��λ��ǰ,��λ�ں�.
//str:����
u16 image_getcolor(u8 mode,u8 *str)
{
	u16 color;
	if(mode)
	{
		color=((u16)*str++)<<8;
		color|=*str;
	}else
	{
		color=*str++;
		color|=((u16)*str)<<8;
	}
	return color;	
}
//��Һ���ϻ�ͼ		   
//xsta,ysta,xend,yend:��ͼ����
//scan:��image2lcd V2.9��˵��.
//*p:ͼ������
void image_show(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 scan,u8 *p)
{  
	u32 i,j,yy;
	u32 len=0;
	if((scan&0x03)==0)//ˮƽɨ��
	{
		switch(scan>>6)//����ɨ�跽ʽ
		{
			case 0:
				LCD_Scan_Dir(L2R_U2D);//������,���ϵ���
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xsta,ysta);//���ù��λ�� 
				break; 
			case 1:
				LCD_Scan_Dir(L2R_D2U);//������,���µ���
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xsta,yend);//���ù��λ�� 
				break; 
			case 2:
				LCD_Scan_Dir(R2L_U2D);//���ҵ���,���ϵ���
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xend,ysta);//���ù��λ�� 
				break; 
			case 3:
				LCD_Scan_Dir(R2L_D2U);//���ҵ���,���µ���
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xend,yend);//���ù��λ�� 
				break; 
		}
	}else  //��ֱɨ��
	{
		switch(scan>>6)//����ɨ�跽ʽ
		{
			case 0:
				LCD_Scan_Dir(U2D_L2R);//���ϵ���,������
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xsta,ysta);//���ù��λ�� 
				break; 
			case 1:
				LCD_Scan_Dir(D2U_L2R);//���µ��ϴ�,����
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xsta,yend);//���ù��λ�� 
				break; 
			case 2:
				LCD_Scan_Dir(U2D_R2L);//���ϵ���,���ҵ��� 
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xend,ysta);//���ù��λ�� 
				break; 
			case 3:
				LCD_Scan_Dir(D2U_R2L);//���µ���,���ҵ���
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xend,yend);//���ù��λ�� 
				break; 
		}
	}
				LCD_Set_Window(xsta,ysta,xend,yend);
				LCD_SetCursor(xsta,ysta);//���ù��λ�� 

	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM
	//len=(xend-xsta+1)*(yend-ysta+1);	//д������ݳ���
	yy=ysta;
	for(j=0;j<(yend-ysta+1);j++){
	for(i=0;i<(xend-xsta+1);i++)
	{
		LCD_WR_DATA(image_getcolor(scan&(1<<4),p));
		p+=2;
	}
	yy++;
	LCD_SetCursor(xsta,yy);
	LCD_WriteRAM_Prepare();
}
	LCD_Set_Window(0,0,269,479);
   					  	    
}  

//��ָ����λ����ʾһ��ͼƬ
//�˺���������ʾimage2lcd������ɵ�����16λ���ɫͼƬ.
//����:1,�ߴ粻�ܳ�����Ļ������.
//     2,��������ʱ���ܹ�ѡ:��λ��ǰ(MSB First)
//     3,�������ͼƬ��Ϣͷ����
//x,y:ָ��λ��
//imgx:ͼƬ����(�������ͼƬ��Ϣͷ,"4096ɫ/16λ���ɫ/18λ���ɫ/24λ���ɫ/32λ���ɫ����ͼ������ͷ)
//ע��:���STM32,����ѡ��image2lcd��"��λ��ǰ(MSB First)"ѡ��,����imginfo�����ݽ�����ȷ!!
void image_display(u16 x,u16 y,u8 * imgx)
{
	HEADCOLOR *imginfo;
 	u8 ifosize=sizeof(HEADCOLOR);//�õ�HEADCOLOR�ṹ��Ĵ�С
	imginfo=(HEADCOLOR*)imgx;
 	image_show(x,y,x+imginfo->w-1,y+imginfo->h-1,imginfo->scan,imgx+ifosize);		
}















