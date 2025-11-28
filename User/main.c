#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "Track.h"
#include <string.h>

uint8_t mode=0;
uint8_t speed_mode=0;

float Mode1_Actual,Mode1_Out;
float Mode1_Kp=0.5,Mode1_Ki=0.02,Mode1_Kd=0.01;
float Mode1_Error0,Mode1_Error1,Mode1_Error2;

float Mode2_Actual,Mode2_Out; 
float Mode2_Kp=0.5 ,Mode2_Ki=0.02 ,Mode2_Kd=0.01 ;
float Mode2_Error0,Mode2_Error1,Mode2_Error2;
float left_target_speed=0.0f;
float right_target_speed=0.0f;
float base_speed=45.0f;
float turn_speed=45.0f;


int main(void)
{ 
	OLED_Init();
    Key_Init();
	Motor_Init();
	Encoder_Init();
	Serial_Init();
	Timer_Init();

    OLED_ShowString(1,1,"SPEED");
	

	while (1)
	{
		if(Key_Check(KEY_2,KEY_SINGLE)){
			speed_mode=(speed_mode==1)?0:1;
			if(speed_mode==0){
				OLED_ShowString(2,1,"speed:50");
			}else{
				OLED_ShowString(2,1,"speed:100");
		   }
	    }
		if(Key_Check(KEY_1,KEY_SINGLE)){
			mode=(mode==1)?0:1; 
		}
	   	if(mode==1){
			Read_Track_Sensor();
			if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 1 && Sensor4 == 0 && Sensor5 == 0) {
				// 00100 - 中间传感器检测到线，直线行驶
				left_target_speed = base_speed;
				right_target_speed = base_speed;
			}
			else if(Sensor1 == 0 && Sensor2 == 1 && Sensor3 == 1 && Sensor4 == 0 && Sensor5 == 0) {
				// 01100 - 轻微左偏，向右微调
				right_target_speed = base_speed + turn_speed;
				left_target_speed = base_speed - turn_speed;
			}
			else if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 1 && Sensor4 == 1 && Sensor5 == 0) {
				// 00110 - 轻微右偏，向左微调
				right_target_speed = base_speed - turn_speed;
				left_target_speed = base_speed + turn_speed;
			}
			else if(Sensor1 == 0 && Sensor2 == 1 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0) {
				// 01000 - 中等左偏，向右调整
				right_target_speed = base_speed + turn_speed * 1.25;
				left_target_speed = base_speed - turn_speed * 1.25;
			}
			else if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 1 && Sensor5 == 0) {
				// 00010 - 中等右偏，向左调整
				right_target_speed = base_speed - turn_speed * 1.25;
				left_target_speed = base_speed + turn_speed * 1.25;
			}
			else if(Sensor1 == 1 && Sensor2 == 1 && Sensor3 == 1 && Sensor4 == 0 && Sensor5 == 0) {
				// 11100 - 大左偏，急向右转
				right_target_speed = base_speed + turn_speed * 2;
				left_target_speed = base_speed - turn_speed * 2;
			}
			else if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 1 && Sensor4 == 1 && Sensor5 == 1) {
				// 00111 - 大右偏，急向左转
				right_target_speed = base_speed - turn_speed * 3;
				left_target_speed = base_speed + turn_speed * 3;
			}
			else if(Sensor1 == 1 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0) {
				// 10000 - 极左偏，急向右转
				right_target_speed = base_speed + turn_speed * 2.5;
				left_target_speed = base_speed - turn_speed * 2.5;
			}
			else if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 1) {
				// 00001 - 极右偏，急向左转
				right_target_speed = base_speed - turn_speed * 2.5;
				left_target_speed = base_speed + turn_speed * 2.5;
			}
			else if(Sensor1 == 1 && Sensor2 == 1 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0) {
				// 11000 - 左偏，向右转
				right_target_speed = base_speed + turn_speed * 2;
				left_target_speed = base_speed - turn_speed * 2;
			}
			else if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 1 && Sensor5 == 1) {
				// 00011 - 右偏，向左转
				right_target_speed = base_speed - turn_speed * 2;
				left_target_speed = base_speed + turn_speed * 2;
			}
			else if(Sensor1 == 1 && Sensor2 == 1 && Sensor3 == 1 && Sensor4 == 1 && Sensor5 == 1) {
				// 11111 - 检测到十字路口或起跑线，直行
				right_target_speed = base_speed;
				left_target_speed = base_speed;
			}
			else if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0) {
				// 00000 - 丢失路线，根据上一次状态处理或停止
				// 可以选择保持上次速度、减速或停止
				right_target_speed=right_target_speed;
				left_target_speed=left_target_speed;
			}
			else {
				// 其他未预料到的情况，保守处理
				right_target_speed=right_target_speed;
				left_target_speed=left_target_speed;

			}
			
			// 速度限幅，确保不超过最大最小速度
//			float max_speed = 100.0f;
//			float min_speed = 0.0f;
//			
//			if(left_target_speed > max_speed) left_target_speed = max_speed;
//			if(left_target_speed < min_speed) left_target_speed = min_speed;
//			if(right_target_speed > max_speed) right_target_speed = max_speed;
//			if(right_target_speed < min_speed) right_target_speed = min_speed;
		}else{
			left_target_speed=0.0f;
			right_target_speed=0.0f;
		}
		Serial_Printf("%f,%f,%f\n",left_target_speed,Mode2_Actual,Mode2_Out);
		
	    Delay_ms(1);
	}
}




//void TIM1_UP_IRQHandler(void){
//	
//	static uint16_t Count;
//	
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		Key_Tick();
//		
//		Count++;
//		if(Count>=10)
//		{
//			// 功能一电机控速
//			if(mode==0){
//			Count=0;
//			//增量式PID控制速度
//			Mode1_Actual=Encoder1_Get();
//			
//			Mode1_Error2=Mode1_Error1;
//			Mode1_Error1=Mode1_Error0;
//			Mode1_Error0=Mode1_Target-Mode1_Actual;
//			
//			Mode1_Out += Mode1_Kp * (Mode1_Error0-Mode1_Error1) +
//		    	         Mode1_Ki *  Mode1_Error0 +
//			             Mode1_Kd * (Mode1_Error0-2*Mode1_Error1+Mode1_Error2);
//			
//			if(Mode1_Out>100)Mode1_Out=100;
//			if(Mode1_Out<-100)Mode1_Out=-100;
//			
//			Motor1_SetPWM(Mode1_Out);
//		}
//	}
//		//功能二主从电机
//		if (mode==1){
//			
//			Count=0;
//			//读取电机一位置为目标
//			Mode2_Target+=Encoder1_Get();
//			//增量式PID控制位置
//			Mode2_Actual+=Encoder2_Get();
//			
//			Mode2_Error2=Mode2_Error1;
//			Mode2_Error1=Mode2_Error0;
//			Mode2_Error0=Mode2_Target-Mode2_Actual;
//			Mode2_Out += Mode2_Kp * (Mode2_Error0-Mode2_Error1) +
//					     Mode2_Ki *  Mode2_Error0 +
//					     Mode2_Kd * (Mode2_Error0-2*Mode2_Error1+Mode2_Error2);
//			
//			if(Mode2_Out>100)Mode2_Out=100;
//			if(Mode2_Out<-100)Mode2_Out=-100;
//			
//			Motor2_SetPWM(Mode2_Out);
//		}
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Key_Tick();
		Mode1_Actual=Encoder1_Get()*14;
		
		Mode1_Error2=Mode1_Error1;
		Mode1_Error1=Mode1_Error0;
		Mode1_Error0=right_target_speed-Mode1_Actual;
		
		Mode1_Out += Mode1_Kp * (Mode1_Error0-Mode1_Error1) +
					 Mode1_Ki *  Mode1_Error0 +
					 Mode1_Kd * (Mode1_Error0-2*Mode1_Error1+Mode1_Error2);
		
		if(Mode1_Out>100)Mode1_Out=100;
		if(Mode1_Out<-100)Mode1_Out=-100;
		
		Motor1_SetPWM(Mode1_Out);
		
		Mode2_Actual=Encoder2_Get()*14;
		
		Mode2_Error2=Mode2_Error1;
		Mode2_Error1=Mode2_Error0;
		Mode2_Error0=left_target_speed-Mode2_Actual;
		
		Mode2_Out += Mode2_Kp * (Mode2_Error0-Mode2_Error1) +
					 Mode2_Ki *  Mode2_Error0 +
					 Mode2_Kd * (Mode2_Error0-2*Mode2_Error1+Mode2_Error2);
		
		if(Mode2_Out>100)Mode2_Out=100;
		if(Mode2_Out<-100)Mode2_Out=-100;
		
		Motor2_SetPWM(Mode2_Out);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Motor4_SetPWM(Mode1_Out);
		Motor3_SetPWM(Mode2_Out);
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}





