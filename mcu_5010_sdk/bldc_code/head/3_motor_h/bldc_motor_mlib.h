#ifndef __BLDC_MOTOR_MLIB_H_
#define __BLDC_MOTOR_MLIB_H_



extern uint8_t ipdDriTab[6];
extern uint8_t ipdDriTabRev[6];

extern uint8_t gNextSectorTab[2][6];
extern uint8_t gLastSectorTab[2][6];

extern uint8_t g_Hall_NextSectorTab[2][7];
extern uint8_t g_Hall_LastSectorTab[2][7];

void MC_CrossZero_AcmpIRQ(void);
void MC_CrossZero_AcmpScan(void);
void MC_CrossZero_AdcScan(void);



void MC_FindZero_Deal(void);
void MC_CrossZeroTime_Deal(void);
void MC_MaskShiftTime_Deal(void);

void MC_ChangePhase_Deal(void);

void MC_MaskStateStart_Set(void);
void MC_MaskStateEnd_Check(void);
void MC_MaskTime_Write(void);

void MC_MaskStop(void);
void MC_MotorState_Manage(void);

uint8_t MC_IPD_6Pulse(void);
uint8_t MC_IPD_12Pulse(void);

void MC_RePhaseCrossZero_AcmpScan(void);
void MC_RePhaseFindZero_Deal(void);


void MC_PwmFreSet(uint16_t pwmFre);
void MC_PwmDutyWrite(uint16_t   uiDutyTemp);



//int32_t MC_PI_Handler( PidPI_t *pi,int32_t errorBack);
void MC_VoltReal_Get(uint16_t uiDutyTemp);
#endif

