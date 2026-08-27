#ifndef CSA_H
#define CSA_H

//#include "stdint.h"


void CSA_ENABLE(u8 state);
void CSA_InputCH_SWMode	(uint32_t state);
void CSA_SET_InputCH(uint32_t CHSEL);
void CSA_SET_PGAGAIN(uint32_t coeff);
void CSA_SET_REF(uint32_t ref);



#endif

