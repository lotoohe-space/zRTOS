#include "sem.h"
#include "task.h"
#include "mem.h"
TCB_SEM* sem_create(void){
	TCB_SEM *mSEM=(TCB_SEM *)m_malloc(sizeof(TCB_SEM));
	if(mSEM==null){return null;}
	//默认阻塞
	mSEM->sem_flag=0;
	return mSEM;
}
void sem_free(TCB_SEM *mSEM){
	m_free(mSEM);
}
//发送一个信号量
uint32 sem_post(TCB_SEM *mSEM){
	if(mSEM==null){return false;}
	enter_int();
	mSEM->sem_flag=1;
	exit_int();
	return true;
}
//接收一个信号量
uint32 sem_pend(TCB_SEM *mSEM,uint32 sem_wait_delay){
	if(mSEM==null){return false;}
	do{
		if(sem_wait_delay!=0&&sem_wait_delay!=SEM_WAIT_ENDLESS){
			os_task_delay(sem_wait_delay);
		}
		if(mSEM->sem_flag==1){
			enter_int();
			mSEM->sem_flag=0;
			exit_int();
			return true;
		}
	}while(sem_wait_delay==SEM_WAIT_ENDLESS);
	return false;
}

