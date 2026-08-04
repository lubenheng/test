#ifndef MCPWM_CMDLINE_H
#define MCPWM_CMDLINE_H


int cmd_mcpwm_function1_test(int argc, char *argv[]);
int cmd_mcpwm_function2_test(int argc, char *argv[]);
int cmd_mcpwm_parameter_set(int argc, char *argv[]);

/*四种发波方式*********************************************/
/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
int cmd_mcpwm_center_align_test(int argc, char *argv[]);
/* 图 14-8 MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐推挽模式 */
int cmd_mcpwm_center_align_push_pull_test(int argc, char *argv[]);
/* 图 14-9MCPWM 时序边沿对齐模式 */
int cmd_mcpwm_edge_align_test(int argc, char *argv[]);
/* 图 14-10MCPWM 时序 TH<n>0 和 TH<n>1 边沿对齐推挽模式 */
int cmd_mcpwm_edge_align_push_pull_test(int argc, char *argv[]);

/*中断扫描*********************************************/
int cmd_mcpwm_interrupt_source_test(int argc, char *argv[]);
int cmd_mcpwm_interrupt_source_test(int argc, char *argv[]);

/*通道逻辑测试*********************************************/
int cmd_mcpwm_channel_n_p_inverse_test(int argc, char *argv[]);
int cmd_mcpwm_channel_swap_test(int argc, char *argv[]);
int cmd_mcpwm_output_default_value_test(int argc, char *argv[]);

/*故障保护测试*********************************************/
int cmd_mcpwm_io_fault_test(int argc, char *argv[]);
/****************************************************/


#define MCPWM_CMDLINE_MENU 		{"cmd_mcpwm_interrupt_source_test",				cmd_mcpwm_interrupt_source_test,			"   test"},\
								{"cmd_mcpwm_channel_n_p_inverse_test",			cmd_mcpwm_channel_n_p_inverse_test,			"   test"},\
								{"cmd_mcpwm_channel_swap_test",					cmd_mcpwm_channel_swap_test,				"   test"},\
								{"cmd_mcpwm_output_default_value_test",			cmd_mcpwm_output_default_value_test,		"   test"},\
								{"cmd_mcpwm_center_align_test",					cmd_mcpwm_center_align_test,				"   test"},\
								{"cmd_mcpwm_center_align_push_pull_test",		cmd_mcpwm_center_align_push_pull_test,		"   test"},\
								{"cmd_mcpwm_edge_align_test",					cmd_mcpwm_edge_align_test,					"   test"},\
								{"cmd_mcpwm_edge_align_push_pull_test",			cmd_mcpwm_edge_align_push_pull_test,		"   test"},\
								{"cmd_mcpwm_io_fault_test",					cmd_mcpwm_io_fault_test,					"   test"},\
								{"cmd_mcpwm_parameter_set",     				cmd_mcpwm_parameter_set, 					"   set"},
											
#endif /* MCPWM_CMDLINE_H */

