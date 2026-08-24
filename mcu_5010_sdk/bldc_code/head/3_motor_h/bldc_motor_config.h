#ifndef __BLDC_MOTOR_CONFIG_H_
#define __BLDC_MOTOR_CONFIG_H_




/****************************************************************************
*(1)
*堵转保护
*
*****************************************************************************/
#define Block_Protect_En                                        (1)/*堵转保护*/
#define Block_Protect_LowDuty									(0.70)/*堵转保护低占空比*/
#define Block_Protect_LowDuty_FiltMS                            (2000)/*低占空比堵转时间unit:ms*/
#define Block_Protect_HighDuty_FiltMS                           (20)/*高占空比堵转时间unit:ms*/
/****************************************************************************/

  
/****************************************************************************
*(2)
*缺相保护
*
*****************************************************************************/
#define LackPhase_Protect_En                                    (1)/*缺相保护*/
#define LackPhase_Protect_Delta                                 (50)/*缺相保护滤波*/
/****************************************************************************/


/****************************************************************************
*(3)
*失步保护
*
*****************************************************************************/
#define LosStep_Protect_En                                      (0)/*失步保护*/
#define LosStep_Protect_CntSet                                  (30)/*失步保护滤波*/
/****************************************************************************/


/****************************************************************************
*(4)
*强制换相
*
*****************************************************************************/
#define ForceChangePhase_En                               		(1)/*强制换相*/
#define ForceChangePhase_CntSet                            		(120000)/*强制换相时间*/
/****************************************************************************/


/****************************************************************************
*(5)
*调速控制
*
*****************************************************************************/
#define LoopMode_None                            				(0)/*开环*/
#define LoopMode_Speed                      					(1)/*速度环*/
#define LoopMode_Set                                     		(LoopMode_None)/*调速设置*/

#define LoopSpeed_AimSet_Min                           			(1500)/*转速环转速设置uint:rpm*/
#define LoopSpeed_AimSet_Max                             		(15000)/*转速环转速设置uint:rpm*/         
#define LoopSpeed_PID_Kp                             			((0.05)*32768)/*kp Q15*/
#define LoopSpeed_PID_Ki                                 		((0.01)*32768)/*ki Q15*/

#define LoopNone_Duty_Min                           			((0.10)*32768)/*调速占空比范围*/
#define LoopNone_Duty_Max                             			((1.00)*32768)/*调速占空比范围*/      

   
#define DutyBuf_Deal_Fre                         				(1000)/*unit:hz*/                               
                             
#define DutyBuf_Step_Q15                         				(819)/*buf步进值Max32768*/                               

#define DutyRun_StepAdd_Open_Q15           						(82)/*run步进值Max32768*/
#define DutyRun_StepAdd_Closed_Q15         						(82)/*run步进值Max32768*/
#define DutyRun_StepReduce_Q15                     				(82)/*run步进值Max32768*/
                                                                                    
#define DutyLimit_Min_Q15                               		((0.01)*32768)/*占空比设置*/
#define DutyLimit_Max_Q15                            			((1.00)*32768)/*占空比设置*/	

/****************************************************************************/


/****************************************************************************
*(6)
*扇区时间限制
*
*****************************************************************************/
#define MaskTime_Max_US                                         (3000)/*最大屏蔽时间uint:us*/
#define MaskTime_Min_US                                         (20)/*最小屏蔽时间uint:us*/

#define ShiftTime_Max_US                                        (3000)/*最大移相时间uint:us*/
#define ShiftTime_Min_US                                        (10)/*最小移相时间uint:us*/

#define SectorTIme_Max_US                                       (20000)/*最大计算扇区时间uint:us*/
#define SectorTIme_Min_US                                       (50)/*最小计算扇区时间uint:us*/
/****************************************************************************/


/****************************************************************************
*(7)
*屏蔽移相设置
*
*****************************************************************************/
#define Sector_CloseLoop_CntSet                                 (6)/*换相时间闭环计算滤波*/

#define MaskTime_OpenLoop_Dis									(0)/*为1时开环屏蔽时间由角度计算*/
#define MaskTime_OpenLoop_US                                    (800)/*开环屏蔽时间uint:us*/
#define ShiftTime_OpenLoop_US                                   (20)/*开环移相时间uint:us*/

#define MaskTime_LowSpeed_Angle                               (28)/*闭环屏蔽角度:1-50*/
#define ShiftTime_LowSpeed_Angle                              (5)/*闭环移相角度:1-50*/
#define MaskTime_HighSpeed_Angle                               (35)/*闭环屏蔽角度:1-50*/
#define ShiftTime_HighSpeed_Angle                              (15)/*闭环移相角度:1-50*/

#define ShiftTime_Dis_CntSet                                   	(6)/*移相跳过滤波*/
#define ShiftTime_En_SectorTime_US                              (250)/*移相恢复扇区条件uint:us*/
#define ShiftTime_Dis_SectorTime_US                             (150)/*移相跳过扇区条件uint:us*/
/****************************************************************************/


/****************************************************************************
*(8)
*动态启动
*
*****************************************************************************/
#define Motion_En                                               (1)/*热启动使能*/
#define Motion_Pass_CntSet                                      (9)/*滑行判断滤波*/          
#define Motion_Pass_LoopRun_Set                                 (1)/*滑行切入后换相开环或是闭环*/

#define Motion_MaskTime_US                                      (30)/*滑行屏蔽时间uint:us*/
#define Motion_ShiftTime_US                                     (30)/*滑行移相时间uint:us*/
#define Motion_SectorTime_Lim_US                                (10000)/*滑行限制扇区条件uint:us*/

#define Motion_CheckTime_Min_MS                                 (30)/*滑行开窗最低时间uint:ms*/
#define Motion_CheckTime_OnceIRQ_Add_MS                         (2)/*滑行开窗递增时间uint:ms*/

#define Motion_BrakeTime_Min_MS                                 (30)/*滑行刹车最低时间uint:ms*/
#define Motion_BrakeTime_OnceIRQ_Add_MS                         (1)/*滑行刹车递增时间uint:ms*/
/****************************************************************************/


/****************************************************************************
*(9)
*静止启动
*
*****************************************************************************/
#define MotorStart_IPD                                          (1)
#define MotorStart_Drag                                         (2)
#define MotorStart_LowVolt                                      (3)
#define MotorStart_Mode                                         (MotorStart_IPD)

#define MotorStart_IPD_PulseTime                                (40)/*IPD脉冲时间uint:us*/
#define MotorStart_IPD_ChargeTime                               (500)/*IPD自举时间uint:us*/
#define MotorStart_IPD_IdleTime                                 (100)/*IPD空闲时间uint:us*/

#define MotorStart_IPD_Drive_Phase2                             (0)
#define MotorStart_IPD_Drive_Phase3                             (1)
#define MotorStart_IPD_Drive_Set                     		    (MotorStart_IPD_Drive_Phase2)/*选择1组脉冲可选择2相或3相*/

#define MotorStart_IPD_PulseRephaseEn                           (0)/*IPD反向脉冲使能*/
/****************************************************************************/


/****************************************************************************
*(10)
*停机
*
*****************************************************************************/
#define SBrake_En                                               (0)/*软刹车使能*/
#define Sbrake_Duty_Add_Q15                                     (2)/*0-32768刹车占空比*/
#define Sbrake_Duty_Max_Q15                                     (32768)/*0-32768刹车占空比*/
#define Sbrake_Duty_Min_Q15                                     (327)/*0-32768刹车占空比*/
#define Sbrake_Time_MS                                          (100)/*软刹车时间 uint:ms*/   
 
#define HBrake_En                                               (0)/*硬刹车使能*/
#define HBrake_Time_MS                                          (100)/*硬刹车时间 uint:ms*/
    
#define NoBrake_Time_MS                                         (1000)/*自然停机时间 uint:ms*/   
/****************************************************************************/


/****************************************************************************
*(11)
*过零
*
*****************************************************************************/
#define ScanMode_3AD                                            (0)
#define ScanMode_ACMP                                           (1)
#define ScanMode_Set                                            (ScanMode_ACMP)/*扫描模式选择*/

#define AcmpIRQ_En_CntSet                                      	(4)/*使能计数滤波*/
#define AcmpIRQ_En_Period_Cnt                                   (10)/*中断过零使能条件，扇区周期数小于10个*/
#define AcmpIRQ_Dis_Period_Cnt                                  (13)/*中断过零禁止条件，扇区周期数大于13个*/
/****************************************************************************/



/****************************************************************************
*(12)
*其他
*
*****************************************************************************/
#define Block_ShiftLim_En                                       (0)/*堵转移相限制使能*//*用于解决在长时间堵停电机不换相到换相时，移相时间过长*/
#define Block_ShiftLim_Clr_CntSet                               (3)/*堵转移相限制清除滤波*/

/****************************************************************************/



#endif
