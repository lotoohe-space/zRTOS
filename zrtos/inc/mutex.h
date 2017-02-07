#ifndef _MUTEX_H__
#define _MUTEX_H__
#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "type.h"
//һֱ�ȴ�
#define MUTEX_WAIT_ENDLESS 0xffffffff

typedef struct _MUTEX{
	//����������־
	uint8 mutex_flag;
}MUTEX;
//�������ͷź���
MUTEX* mutex_create(void);
void mutex_free(MUTEX *mMUTEX);

//���ƺ���
uint32 mutex_unlock(MUTEX* mMUTEX);
uint32 mutex_lock(MUTEX* mMUTEX,uint32 mutex_wait_delay);

#ifdef __cplusplus
}
#endif
#endif
