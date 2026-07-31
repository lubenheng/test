#ifndef I2C_TEST_H
#define I2C_TEST_H


#include "i2c_reg.h"
#include "i2c_type.h"


void i2c_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void i2c_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);


#define I2C_TEST_LOOP			100u

#define I2C_DATALEN_WITH_OP11	4U
#define I2C_DATALEN_WITH_PY02	16U

#define I2C_REG_LEN			I2C_PY02_REG_LEN
#define I2C_DATALEN_TEST	I2C_DATALEN_WITH_PY02

typedef enum{
	I2C_MASTER_MODE_TEST = 0U,
	I2C_MASTER_DMATCH_TEST,
	I2C_MASTER_HREQ_PIN_TEST,
	I2C_MASTER_DMA_TEST,
	I2C_SLAVE_MODE_TEST,
	I2CM_GPT_TRIGGER_TEST,
	I2C_TRGMUX_TRIGGER_TEST,
	I2C_TRGT_TRIGGER_TEST,
	I2C_CMP_TRIGGER_TEST,
	I2C_BURNIN_MODE_TEST
}i2c_test_sel_e;

/** I2C specific port selection. */
typedef enum
{
	I2C0_SEL0 = 0U,		/*!< I2C0 SCL:PTA3, ALT:3, SDA:PTA2, ALT:3 */
	I2C0_SEL1,				/*!< I2C0 SCL:PTC3, ALT:6, SDA:PTC2, ALT:6 */
	I2C1_SEL0,				/*!< I2C1 SCL:PTD5, ALT:5, SDA:PTC1, ALT:5 */
	I2C1_SEL1,				/*!< I2C1 SCL:PTE1, ALT:4, SDA:PTE0, ALT:4 */
	I2C1_SEL2,				/*!< I2C1 SCL:PTD1, ALT:5, SDA:PTD0, ALT:5 */
} i2c_port_sel_e;

extern i2c_master_config_t g_i2cm_config;
extern i2c_master_msg_t g_i2c_master_msg;
extern i2c_slave_config_t g_i2cs_config; 
extern i2c_slave_msg_t g_i2c_slave_msg;

extern bool g_i2c_pressure_test_finish;


void i2c0_gpio_init(void);
void i2c1_gpio_init(void);


void i2c_master_int_handle_test(i2c_event_e master_event);
void i2c_master_int_handle_t(i2c_event_e master_event);
void i2c_slave_int_handle_test(i2c_event_e slave_event);
void i2c_test(i2c_instance_e i2c_instance, i2c_test_sel_e i2c_test_sel);

/*test cases*/
void i2c_master_mode_handle_test(i2c_instance_e i2c_instance);
void i2c_master_hreq_pin_test(i2c_instance_e i2c_instance, uint32_t polarity);
void i2c_master_trgmux_trigger_test(i2c_instance_e i2c_instance, uint8_t trg_src);
void i2c_master_gpt_trigger_test(i2c_instance_e i2c_instance, uint32_t e_flag);
void i2c_master_trgt_trigger_test(i2c_instance_e i2c_instance, uint32_t s_ic);
status_t i2c_master_dmatch_test(i2c_instance_e i2c_instance, i2c_master_dmatch_config_e datamatch_cfg);
void i2c_master_10address_test(i2c_instance_e i2c_instance);
void i2c_master_pinlow_test(i2c_instance_e i2c_instance);
void i2c_master_autostop_test(i2c_instance_e i2c_instance, uint32_t t_case);
void i2c_master_dma_maintest(i2c_instance_e i2c_instance);
void i2c_slave_test(i2c_instance_e i2c_instance);
void i2c_slave_randomread_test(i2c_instance_e i2c_instance);
//void i2c_slave_addrmatch_test(i2c_instance_e i2c_instance,
//                              i2c_slave_addr_config_e addr_cfg,
//                              uint32_t addr0, uint32_t addr1);
void i2c_slave_generalcall_test(i2c_instance_e i2c_instance);
void i2c_slave_smbus_test(i2c_instance_e i2c_instance);

void i2c_trgt_trigger_process(i2c_instance_e i2c_instance);
void i2c_pressure_test(i2c_instance_e i2c_instance);
void i2c_burnin_test(void);

void i2c_master_randomnum_trans_test(i2c_instance_e i2c_instance);
void i2c_slave_randomnum_trans_test(i2c_instance_e i2c_instance);

void i2c_pinmux_test(i2c_instance_e i2c_instance, uint32_t group);
void i2cs_pinmux_test(i2c_instance_e i2c_instance, uint32_t group);
void i2c_clock_test(i2c_instance_e i2c_instance, bool apb_en, uint32_t pre_div, uint32_t speed);

void gpio_test_verify(void);


#endif /* I2C_TEST_H */


