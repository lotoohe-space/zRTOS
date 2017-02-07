#ifndef _TIMER_H__
#define _TIMER_H__
#include "task.h"

/** һ��ģʽ */
#define TIMER_MODE_ONCE 0
/** ����ѭ��ģʽ */
#define TIMER_MODE_INFINITE_LOOP 1

/** ��ʱ����Ĭ�ϵȼ� */
#define TIMER_DEFINE_PRIORITY (TASK_MIN_LEVEL/2)

	
typedef struct _TIMER{
	/** �̳�������ص�tcb */
	TASK_TCB mTASK_TCB;
	/** �ص���ʱ�� */
	uint32 back_call_timer;
	/** �����Ļص���ַ */
	uint32 fun_addr;
	/** ��ʱ����ģʽ */
	uint8  mode;
	/** ��λ��� */
	uint8 reset_flag;
}TIMER;


TIMER * timer_create(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 task_num,
	uint8 timer_mode,
	uint32 timer_sys_count
);

void timer_reset(TIMER *mTIMER);
uint32 timer_start(TIMER *mTIMER);
uint32 timer_stop(TIMER *mTIMER);
uint32 timer_set_cycle_val(TIMER *mTIMER,uint32 val);
#endif
