#ifndef CMDLINE_H
#define CMDLINE_H

/* command line process */
#define CMDLINE_BAD_CMD			(-1)
#define CMDLINE_TOO_MANY_ARGS	(-2)
#define CMDLINE_TOO_FEW_ARGS	(-3)
#define CMDLINE_INVALID_ARG		(-4)
#define CMDLINE_CARRIAGE_RET	(-5)

/* Define function error for test routine */
#define ERR_TEST_DONE		0		/* test done */
#define ERR_TEST_FAIL		1		/* test fail */
#define ERR_TEST_ARGC		200		/* argc error */
#define ERR_TEST_PARA		201		/* nvalid parameter */

#ifdef SHE_CMD_TEST
#define CMD_BUF_SIZE		1280
#define CMDLINE_MAX_ARGS    160
#else
#define CMD_BUF_SIZE		128
#define CMDLINE_MAX_ARGS    32
#endif

typedef int (*pfn_cmdline)(int argc, char* argv[]);


typedef struct
{
	const char* cmd;
	pfn_cmdline	pfn_cmd;
	const char* help;
} cmdline_entry;

typedef struct
{
	volatile uint32_t cmdline_buff_cnt;
	volatile bool ready_flag;
} cmdline_ctl;

void cmdline_handle(void);
void cmdline_handle_polling(void);
void cmdline_init(void);
int cmd_help(int argc, char *argv[]);
int cmd_mem_read(int argc, char *argv[]);
int cmd_mem_write(int argc, char *argv[]);
int cmd_mem_write_cd(int argc, char *argv[]);
int cmd_mcu_sleep(int argc, char *argv[]);
int cmd_system_reset(int argc, char *argv[]);
int cmd_global_reset(int argc, char *argv[]);
int cmd_print_random_challenge(int argc, char *argv[]);
int cmd_set_gpioA7(int argc, char *argv[]);
int cmd_query_idn(int argc, char *argv[]);
int cmd_pressure_test(int argc, char *argv[]);
int cmd_fee_sector_erase(int argc, char *argv[]);
int cmd_gpio_input_get(int argc, char *argv[]);
int cmd_gpio_output_set(int argc, char *argv[]);
int cmd_gpio_toggle_times(int argc, char *argv[]);
int	cmd_sleep_now(int argc, char *argv[]);
int	cmd_deep_sleep_now(int argc, char *argv[]);
int cmd_gpio_init(int argc, char *argv[]);

int get_version(int argc, char *argv[]);
int cmd_gpio_toggle(int argc, char *argv[]);
int cmd_pulse_us(int argc, char *argv[]);


#endif /*CMDLINE_H*/

