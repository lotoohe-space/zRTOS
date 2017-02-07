#ifndef _SEM_H__
#define _SEM_H__
#include "type.h"
//一直等待
#define SEM_WAIT_ENDLESS 0xffffffff
//信号量的任务控制块
typedef struct _SEM{
	uint32 sem_flag;
}TCB_SEM;

TCB_SEM* sem_create(void);
void sem_free(TCB_SEM *mSEM);
uint32 sem_post(TCB_SEM *mSEM);
uint32 sem_pend(TCB_SEM *mSEM,uint32 sem_wait_delay);
#endif
