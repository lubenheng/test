#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "mdu.h"
#include "uart.h"

uint32_t MDU_UnsignedMultiplyFun(uint16_t M1, uint16_t M2)
{
    MDU->ctrl = 1 | 0<<4;
    MDU->a  = M1;
    MDU->b  = M2;
    MDU->trig = 1;
    while (MDU->ctrl&0x80000000)
    {
    }
    printk("%d\r\n", MDU->c);
		return MDU->c;
}


uint32_t MDU_SignedMultiplyFun(uint16_t M1, uint16_t M2)
{
    MDU->ctrl = 1 | 1<<4;
    MDU->a  = M1;
    MDU->b  = M2;
    MDU->trig = 1;
    while (MDU->ctrl&0x80000000)
    {
    }
    printk("%d\r\n", MDU->c);
		return MDU->c;
}

uint32_t MDU_LeftSignedMultiplyFun(uint16_t M1, uint16_t M2)
{
    MDU->ctrl = 1 | 2<<4;
    MDU->a  = M1;
    MDU->b  = M2;
    MDU->trig = 1;
    while (MDU->ctrl&0x80000000)
    {
    }
    printk("%d\r\n", MDU->c);
		return MDU->c;
}

int16_t MDU_LpfFun(int16_t a, int16_t b,int16_t k)
{
    uint32_t tmp;
    int16_t c;
		MDU->ctrl = 1 | 3<<4;
    MDU->a = (a << 16);
    MDU->b = b;
		MDU->z = k;
    MDU->trig = 1;
	
    while (MDU->ctrl&0x80000000)
    {
    }
		
    tmp = MDU->c;
    c = (tmp >> 16);
		printk("%d\r\n", (MDU->c>>16));
		printk("%d\r\n", c);
    return c;
}


uint32_t MDU_UnsignedDivFun(uint32_t dividend, uint16_t divisor)
{
    DIGCTRL->clk_en0 |= BIT9;
	
	  MDU->ctrl = 1 | 4<<4;
    MDU->a  =  dividend;
    MDU->b =  divisor;
    MDU->trig = 1;
	  //GPIO->out |= BIT26; 
    while (MDU->ctrl&0x80000000)
    {
    }
   // GPIO->out &= ~BIT26;
    printk("%d\r\n %f\r\n", MDU->c, (float)(MDU->d)/65536.0);
		return MDU->c;
}

void  MDU_ParkFun(int16_t X0, int16_t Y0,uint16_t theta)
{
		DIGCTRL->clk_en0 |= BIT9;
    //------------- 坐标旋转 ------------------------
    MDU->ctrl = 1 | 5<<4;
    MDU->a  = X0;
    MDU->b  = Y0 ;
    MDU->z = theta;
    MDU->trig = 1;
		//GPIO->out |= BIT26;
    while (MDU->ctrl&0x80000000)
    { 
    } 
		//GPIO->out &= ~BIT26;
    printk("%d\r\n %d\r\n", (int16_t)(MDU->c),(int16_t)(MDU->d));
}

void  MDU_ArctanFun(int16_t X0, int16_t Y0)
{
	  DIGCTRL->clk_en0 |= BIT9;
//    //------------- 求幅角 ------------------------
    MDU->ctrl = 1 | 6<<4;
    MDU->a  = X0;
    MDU->b = Y0;
    MDU->trig = 1;
//	  GPIO->out |= BIT26; 
    while (MDU->ctrl&0x80000000)
    { 
    }
//		GPIO->out &= ~BIT26;
    printk("%d, %d\r\n", (uint16_t)(MDU->c), (int16_t)(MDU->d));
}























