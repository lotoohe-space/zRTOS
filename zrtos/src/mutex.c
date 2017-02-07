/************************************************************
Copyright (C), 2016-2016, Tech. Co., Ltd.
FileName: mutex.c
Author:		z_space
Version :	1.0
Date:			2016.08.22
Description: 互斥量的实现源码
Version: v1.0
Function List: 
1. mutex_create
2. mutex_free
3. mutex_lock
4. mutex_unlock
History: 无
<author> <time> <version > <desc>
David 2016/08/22 1.0 build this moudle
***********************************************************/
#include "mutex.h"
#include "mem.h"
#include "task.h"
/*************************************************
Function: mutex_create
Description: 互斥量的创建函数
Input: 无
Return: 
内存申请失败 null
成功 返回互斥量的控制块 
Others: 无
*************************************************/
MUTEX* mutex_create(void){
	MUTEX *mMUTEX=(MUTEX*)m_malloc(sizeof(MUTEX));
	if(mMUTEX==null){return null;}
	//默认没有加锁
	mMUTEX->mutex_flag=0;
	return mMUTEX;
}
/*************************************************
Function: mutex_free
Description: 互斥量的释放函数
Input: 无
Return: 无
Others: 无
*************************************************/
void mutex_free(MUTEX *mMUTEX){
	if(mMUTEX==null){return ;}
	m_free(mMUTEX);
}
/*************************************************
Function: mutex_unlock
Description: 互斥量解锁
Input: 
1. mMUTEX
	互斥量的控制块
Return: 
	成功 true
	失败 false
Others: 无
*************************************************/
uint32 mutex_unlock(MUTEX* mMUTEX){
	if(mMUTEX==null){return false;}
	enter_int();
	mMUTEX->mutex_flag=0;
	exit_int();
	return true;
}
/*************************************************
Function: mutex_lock
Description: 互斥量加锁
Input: 
1. mMUTEX
	互斥量的控制块
2.mutex_wait_delay
	互斥量的等待时间，为0xffffffff表示一直等待
Return: 
	成功 true
	失败 false
Others: 无
*************************************************/
uint32 mutex_lock(MUTEX* mMUTEX,uint32 mutex_wait_delay){
	if(mMUTEX==null){return false;}
	if(mutex_wait_delay==MUTEX_WAIT_ENDLESS){
		//一直等待到被解锁
		for(;mMUTEX->mutex_flag!=0;);
	}else{
		//被占用
		if(mMUTEX->mutex_flag!=0){
			//MUTEX_WAIT_ENDLESS
			//等待一段时间
			if(mutex_wait_delay!=0){
				os_task_delay(mutex_wait_delay);
			}
			if(mMUTEX->mutex_flag!=0){
				return false;
			}
		}
	}
	enter_int();
	mMUTEX->mutex_flag=1;
	exit_int();
	return true;
}
