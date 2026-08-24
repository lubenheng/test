#ifndef _FSM_H_
#define _FSM_H_
											
											
typedef enum
{
	FSM_SWITCH_IMM, //when return, the fsm will run into the next fsm immediately
	FSM_WAITING, // when return,the fsm will jump out the whole fsm
	FSM_FINISH, // fsm finish,the fsm will jump out the whole fsm
}fsm_result_e;
											
#endif//_FSM_H_

