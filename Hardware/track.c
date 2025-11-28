#include "stm32f10x.h"                  // Device header
uint8_t Sensor1=0;
uint8_t Sensor2=0;
uint8_t Sensor3=0;
uint8_t Sensor4=0;
uint8_t Sensor5=0;

void Track_Init(void){
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁用JTAG
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	
}

void Read_Track_Sensor(void){
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)==Bit_RESET){
		Sensor1=1;
	}else{
		Sensor1=0;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==Bit_RESET){
		Sensor2=1;
	}else{
		Sensor2=0;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)==Bit_RESET){
		Sensor3=1;
	}else{
		Sensor3=0;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)==Bit_RESET){
		Sensor4=1;
	}else{
		Sensor4=0;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==Bit_RESET){
		Sensor5=1;
	}else{
		Sensor5=0;
	}
}