#ifndef _TASK_H__
#define _TASK_H__
#include "type.h"
/** 设置最小优先级 */
#define TASK_MIN_LEVEL 10
/** 最多32个任务 */
#define TASK_TCB_NUM 32
/** 当前任务线程模式 */
#define TASK_MODE_THREAD 0
/** 当前任务定时器模式 */
#define TASK_MODE_TIMER 1

typedef struct _TASK_TCB{
	/** 堆栈地址该指针会随程序运行发生变化 */
	uint32 *mem_task;
	/** 堆栈的栈低指针，当任务终结时用于内存释放 */
	uint32 *mem_low_task;
	/** 堆栈的栈顶指针 */
	uint32 *mem_bottom_task;
	/** 运行时间计数 */
	uint32 run_count;
	/** 延时计数 */
	uint32 delay_count;
	/** 进程id */
	uint32 task_id;
	/** 当前状态 */
	uint8 status;
	/** 任务优先级 */
	uint8 level;
	/** 当前任务的模式 */
	uint8 mode;
}TASK_TCB;

void task_end(void);

extern void start_os(void);
extern void open_scheduling(void);
extern void enter_int(void);
extern void exit_int(void);

uint32 os_create_base_manual(
  TASK_TCB *mTASK_TCB,
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 *mem,
	uint32 mem_num,
	uint8 mode);
	
TASK_TCB *os_create_base(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 task_num,
	uint8 mode);
	
TASK_TCB* os_create_task(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 task_num);
	
void os_start(void);
void os_task_delay(uint32 count);
void os_task_del(void );
uint32 os_systick_count(void);
#endif
