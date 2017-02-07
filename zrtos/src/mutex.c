/************************************************************
Copyright (C), 2016-2016, Tech. Co., Ltd.
FileName: mutex.c
Author:		z_space
Version :	1.0
Date:			2016.08.22
Description: ��������ʵ��Դ��
Version: v1.0
Function List: 
1. mutex_create
2. mutex_free
3. mutex_lock
4. mutex_unlock
History: ��
<author> <time> <version > <desc>
David 2016/08/22 1.0 build this moudle
***********************************************************/
#include "mutex.h"
#include "mem.h"
#include "task.h"
/*************************************************
Function: mutex_create
Description: �������Ĵ�������
Input: ��
Return: 
�ڴ�����ʧ�� null
�ɹ� ���ػ������Ŀ��ƿ� 
Others: ��
*************************************************/
MUTEX* mutex_create(void){
	MUTEX *mMUTEX=(MUTEX*)m_malloc(sizeof(MUTEX));
	if(mMUTEX==null){return null;}
	//Ĭ��û�м���
	mMUTEX->mutex_flag=0;
	return mMUTEX;
}
/*************************************************
Function: mutex_free
Description: ���������ͷź���
Input: ��
Return: ��
Others: ��
*************************************************/
void mutex_free(MUTEX *mMUTEX){
	if(mMUTEX==null){return ;}
	m_free(mMUTEX);
}
/*************************************************
Function: mutex_unlock
Description: ����������
Input: 
1. mMUTEX
	�������Ŀ��ƿ�
Return: 
	�ɹ� true
	ʧ�� false
Others: ��
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
Description: ����������
Input: 
1. mMUTEX
	�������Ŀ��ƿ�
2.mutex_wait_delay
	�������ĵȴ�ʱ�䣬Ϊ0xffffffff��ʾһֱ�ȴ�
Return: 
	�ɹ� true
	ʧ�� false
Others: ��
*************************************************/
uint32 mutex_lock(MUTEX* mMUTEX,uint32 mutex_wait_delay){
	if(mMUTEX==null){return false;}
	if(mutex_wait_delay==MUTEX_WAIT_ENDLESS){
		//һֱ�ȴ���������
		for(;mMUTEX->mutex_flag!=0;);
	}else{
		//��ռ��
		if(mMUTEX->mutex_flag!=0){
			//MUTEX_WAIT_ENDLESS
			//�ȴ�һ��ʱ��
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
