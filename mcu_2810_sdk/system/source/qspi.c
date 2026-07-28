#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "qspi.h"




//------------------------------------------------
void wait4CtrlerReady (void)
{
	uint8_t tmp;

	tmp = 1;
	while(tmp)
	{
		tmp = (QSPI->stig_basic >> 1) & 1;
		__ISB();
	}
}



//------------------------------------------------
void wait4FlashReady (void)
{
	uint8_t  tmp;
	uint32_t tmp2;
    
	tmp = 1;
	while(tmp)
	{
        QSPI->stig_basic = 0x05800001;
		wait4CtrlerReady();
		
		tmp = (QSPI->stig_rdat_low) & 1;
        
		if (tmp)
        {
            for (tmp2=0; tmp2<1000; tmp2++)
                __ISB();
        }
	}
}



//------------------------------------------------
void xip_4x_init(uint8_t flash_rd_cmd, uint8_t flash_div, uint8_t capture, uint32_t image1_addr)
{
    //确定flash是空闲的
    wait4FlashReady();
    
    //写使能
    QSPI->stig_basic = 0x06000001;
    wait4CtrlerReady();
    wait4FlashReady();

    //写QE+写保护
    QSPI->stig_wdat_low = 0x027f;  
    QSPI->stig_basic    = 0x01009001;
    wait4CtrlerReady();
    wait4FlashReady();
    

    //------------ set XIP ---------------------
    QSPI->basic    = (flash_div<<7) | 1<<4 | 1<<2 | 1<<1;   //stop QSPI ctrler		
    QSPI->read     = 4<<15 | 1<<14 | 2<<12 | 2<<10 | 0xeb; 	//QUAL READ
    QSPI->capture  = capture;
    QSPI->remap    = image1_addr;	//remap 

    if (flash_rd_cmd == 0)
    {
        QSPI->mod_bit  = 0xa0; 	//mode  (快速专用)
        QSPI->basic   |= 1<<5 | 1<<3 | 1; //1<<5 是"快速专用"
    }
    else
    {
        QSPI->mod_bit  = 0x00; 	//mode  (慢速专用)
        QSPI->basic   |= 0<<5 | 1<<3 | 1; //1<<5 是"快速专用"
    }
}
