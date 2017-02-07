#ifndef _TIMER_H__
#define _TIMER_H__
#include "task.h"

/** 一次模式 */
#define TIMER_MODE_ONCE 0
/** 无线循环模式 */
#define TIMER_MODE_INFINITE_LOOP 1

/** 定时器的默认等级 */
#define TIMER_DEFINE_PRIORITY (TASK_MIN_LEVEL/2)

	
typedef struct _TIMER{
	/** 继承任务相关的tcb */
	TASK_TCB mTASK_TCB;
	/** 回调的时间 */
	uint32 back_call_timer;
	/** 函数的回调地址 */
	uint32 fun_addr;
	/** 定时器的模式 */
	uint8  mode;
	/** 复位标记 */
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
