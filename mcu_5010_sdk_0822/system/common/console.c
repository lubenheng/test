/*!
 * @file console.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.6, typedefs that indicate size and signedness should be used in place of
 * the basic numerical types.
 * For structure convenience, basic type names are used.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.5, A conversion should not be performed from pointer to void into
 * pointer to object.
 * This is needed for the conversion of void pointer tu an objext pointer
 * 
 */


//#include "platform_common.h"


#ifdef UART
void print_array(const char *log_header, void *data, uint32_t len)
{
	uint8_t *tmp = (uint8_t *)data;
        UNUSED(log_header);
        UNUSED(tmp);
/* OCS_Reserved_Start */	
        //INFO("%s:\n",log_header);
/* OCS_Reserved_End */
	
	for (uint32_t i=0;i<len;i++)
	{
          /* OCS_Reserved_Start */	
		//INFO("0x%02x,",tmp[i]);
          /* OCS_Reserved_End */
	}
        /* OCS_Reserved_Start */
	//INFO("\n");
        /* OCS_Reserved_End */
}
#endif

