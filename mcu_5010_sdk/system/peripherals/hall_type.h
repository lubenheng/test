#ifndef HALL_TYPE_H
#define HALL_TYPE_H


typedef enum
{
	HALL_DIV_1,  //不分频
	HALL_DIV_2,  //2分频 
	HALL_DIV_4,  //4分频
	HALL_DIV_8   //8分频
}	HALL_Clkdiv_t;

/** hall interrupt types*/
typedef enum{
    HALL_VALUE_0 = 0U,         /*!< 0 = 1*Hall_U + 2*Hall_V + 4*Hall_W  */
    HALL_VALUE_1,              /*!< 1 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_VALUE_2,              /*!< 2 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_VALUE_3,              /*!< 3 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_VALUE_4,              /*!< 4 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_VALUE_5,              /*!< 5 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_VALUE_6,              /*!< 6 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_VALUE_7,              /*!< 7 = 1*Hall_U + 2*Hall_V + 4*Hall_W   */
    HALL_OVERTIME,             /*!< 8,HALL 超时触发中断 */
	HALL_SOFTWARE_TRG,    /*!< 9,软件触发中断 */
	HALL_FAULT,    			   /*!< 10,错误中断 */
} hall_int_source_e;




/*interrupt handle definition*/
typedef void (*HALL_INT_CALLBACK)(hall_int_source_e);


#endif

