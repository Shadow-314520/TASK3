#ifndef __TRACK_H
#define __TRACK_H
#include "stm32f10x.h"                  // Device header


extern uint8_t Sensor1;
extern uint8_t Sensor2;
extern uint8_t Sensor3;
extern uint8_t Sensor4;
extern uint8_t Sensor5;


void Track_Init(void);
uint8_t Read_Track_Sensor(void);



#endif
