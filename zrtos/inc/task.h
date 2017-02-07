#ifndef _TASK_H__
#define _TASK_H__
#include "type.h"
/** ������С���ȼ� */
#define TASK_MIN_LEVEL 10
/** ���32������ */
#define TASK_TCB_NUM 32
/** ��ǰ�����߳�ģʽ */
#define TASK_MODE_THREAD 0
/** ��ǰ����ʱ��ģʽ */
#define TASK_MODE_TIMER 1

typedef struct _TASK_TCB{
	/** ��ջ��ַ��ָ�����������з����仯 */
	uint32 *mem_task;
	/** ��ջ��ջ��ָ�룬�������ս�ʱ�����ڴ��ͷ� */
	uint32 *mem_low_task;
	/** ��ջ��ջ��ָ�� */
	uint32 *mem_bottom_task;
	/** ����ʱ����� */
	uint32 run_count;
	/** ��ʱ���� */
	uint32 delay_count;
	/** ����id */
	uint32 task_id;
	/** ��ǰ״̬ */
	uint8 status;
	/** �������ȼ� */
	uint8 level;
	/** ��ǰ�����ģʽ */
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
