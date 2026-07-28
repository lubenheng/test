#ifndef QEP_TEST_H
#define QEP_TEST_H

#include "qep_type.h"


/****************GPIO Init********************/
void qep_gpio_init(void);

/* callback */
void g_qep_callback(qep_int_source_e souce);





void qep_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void qep_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);




void qep_mode_test(int argc, char *argv[]); 

#endif

