#ifndef _MUTEX_H__
#define _MUTEX_H__
#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "type.h"
//一直等待
#define MUTEX_WAIT_ENDLESS 0xffffffff

typedef struct _MUTEX{
	//互斥量锁标志
	uint8 mutex_flag;
}MUTEX;
//创建与释放函数
MUTEX* mutex_create(void);
void mutex_free(MUTEX *mMUTEX);

//控制函数
uint32 mutex_unlock(MUTEX* mMUTEX);
uint32 mutex_lock(MUTEX* mMUTEX,uint32 mutex_wait_delay);

#ifdef __cplusplus
}
#endif
#endif
