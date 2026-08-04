#ifndef CMDLINE_H
#define CMDLINE_H


/** the symbol of bad command */
#define CMDLINE_BAD_CMD			(-1)
#define CMDLINE_TOO_MANY_ARGS	(-2)
#define CMDLINE_TOO_FEW_ARGS	(-3)
#define CMDLINE_INVALID_ARG		(-4)
#define CMDLINE_CARRIAGE_RET	(-5)

/** the operation result of command callback function */
#define ERR_TEST_DONE		0		
#define ERR_TEST_FAIL		1		
#define ERR_TEST_ARGC		200		
#define ERR_TEST_PARA		201		

/** the size of command buffer */
#define CMD_BUF_SIZE		128
/** the maximum number of cmdline parameters */
#define CMDLINE_MAX_ARGS    16
/** determine whether a character is a number */
#define isdigit(c)      ('0' <= (c) && (c) <= '9')

/** definition of command callback function type */
typedef int (*pfn_cmdline)(int argc, char* argv[]);

/** cmdline entry structure */
typedef struct
{
	const char* cmd;     /*!< the buffre used to save name of cmdline */
	pfn_cmdline	pfn_cmd; /*!< the address to callback function of certain commmand */
	const char* help;    /*!< the tips about certain commmand */
} cmdline_entry;

/** cmdline control structure */
typedef struct
{
	volatile uint32_t cmdline_buff_cnt; /*!< the count of characters saved in cmdline buffer */
	volatile bool ready_flag; 			/*!< true if a complete command was received */
} cmdline_ctl;

/*******************************************************************************
* Public cmdline Control Functions
******************************************************************************/
void cmdline_handle(void);
void cmdline_init(void);
int cmd_help(int argc, char *argv[]);
int mem_read(int argc, char *argv[]);
int mem_write(int argc, char *argv[]);
#endif /*CMDLINE_H*/

