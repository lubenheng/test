#ifndef __MOTOR_HALL_H_
#define __MOTOR_HALL_H_



//#include "cms32m5xxx.h"

#include "stdint.h"
#include "types.h"

#define SENSOR_MODE     0




//有感模式**********************************************************************//
	

#define	HALL_A			0x04
#define	HALL_B			0x02
#define	HALL_C			0x01


#define HALL_A_IO		GPIO1->DI_f.P3
#define HALL_B_IO		GPIO1->DI_f.P4
#define HALL_C_IO		GPIO1->DI_f.P5



extern uint8_t hall_index[6];
extern uint8_t sectorTableOfHall[7];



typedef struct 
{
	uint8_t ModeEn;

	uint8_t Hall_A;
	uint8_t Hall_B;
	uint8_t Hall_C;

	uint8_t hallNow;
	uint8_t hallLast;
	uint8_t HallSector;

	
}Motor_Sensor_t;



extern Motor_Sensor_t strSensor;

void MC_SensorMode_Init(void);

void MC_SensorMode_Start(void);

void MC_SensorMode_HallChange(void);
void UC_SensorMode_IOInit(void);



#endif

