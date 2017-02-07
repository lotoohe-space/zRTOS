/** 
* @file         msg.c
* @brief        消息邮箱的实现代码
* @details  		操作系统的消息邮箱的实现的代码
* @author       z_space
* @date     		2016.8.22
* @version  		1.0
* @par Copyright (c):  
*    						无   
* @par History:          
*   version: author, date, desc\n 
*/ 
#include "msg.h"
#include "mem.h"
#include "task.h"
#include <string.h>
/** 
 * 创建一个消息队列. 
 * 创建一个消息队列,内存申请失败则返回错误. 
 * @param[in]  无 
 * @retval 		 succeed 成功
 * @retval		 null 内存申请失败
 */ 
TCB_MSG* msg_create(void){
	//申请内存
	TCB_MSG* mTCB_MSG=m_malloc(sizeof(TCB_MSG));
	if(mTCB_MSG==null){return null;}
	memset(mTCB_MSG,0,sizeof(TCB_MSG));
	mTCB_MSG->msg_id=(uint32)mTCB_MSG;
	mTCB_MSG->head_index=-1;
	
	return mTCB_MSG;
}
/** 
 * 释放一个消息队列. 
 * 释放一个消息队列，释放后消息队列不能够在使用
 * @param[in]  mTCB_MSG 要释放的消息控制块
 * @retval 		 无
 */
void msg_close(TCB_MSG* mTCB_MSG){
	m_free(mTCB_MSG);
}
/** 
 * 获取消息. 
 * 从消息队列中获取消息
 * @param[in]  mTCB_MSG 要获取的消息的消息控制块 
 * @param[in]  msg			要获取的消息存放地址
 * @param[in]  msg_get_delay 获取消息的超时时间,为0xffffffff时一直等待
 * @retval 		 true 成功
 * @retval		 false 内存申请失败
 */ 
uint32 msg_get(TCB_MSG* mTCB_MSG,void *msg,uint32 msg_get_delay){
	int32 i=0;
	if(mTCB_MSG==null){return false;}
	do{
		//没有消息
		if(mTCB_MSG->head_index<0){
			//如果一直等待
			if(msg_get_delay==MSG_WAIT_ENDLESS){
				continue;
			}
			//等待一段时间
			if(msg_get_delay!=0){
				os_task_delay(msg_get_delay);
			}
			//还没有消息则退出
			if(mTCB_MSG->head_index<0){
				return false;
			}
		}
		//有消息
		enter_int();
		//获取消息
		*(uint32*)msg = (uint32)mTCB_MSG->msg[0];
		//消息右移
		for(i=1;i<MSG_MAX_LEN;i++){
			mTCB_MSG->msg[i-1]=mTCB_MSG->msg[i];
		}
		//队列头减1
		mTCB_MSG->head_index--;
		exit_int();
		return true;
	}while(msg_get_delay==MSG_WAIT_ENDLESS);
	return false;
}
/** 
 * 存放消息. 
 * 存放消息到消息控制块
 * @param[in]  mTCB_MSG 要存放的消息的消息控制块 
 * @param[in]  msg			要发送的消息的地址
 * @param[in]  msg_get_delay 存放消息的超时时间,为0xffffffff时一直等待
 * @retval 		 true 成功
 * @retval		 false 内存申请失败
 */ 
uint32 msg_put(TCB_MSG* mTCB_MSG,uint32 msg,uint32 msg_get_delay){
	if(mTCB_MSG==null){return false;}
	//如果等待时间没有限制
	if(msg_get_delay==MSG_WAIT_ENDLESS){
		//一直等待直到邮箱有空
		for(;mTCB_MSG->head_index >= MSG_MAX_LEN-1;);
	}else{
		//如果邮箱满了
		if(mTCB_MSG->head_index >= MSG_MAX_LEN-1){
			//等待一段时间
			if(msg_get_delay!=0){
				os_task_delay(msg_get_delay);
			}
			//如果还满则退出
			if(mTCB_MSG->head_index >= MSG_MAX_LEN-1){
				return false;
			}
		}
	}
	enter_int();
	mTCB_MSG->head_index++;
	//存放消息
	(mTCB_MSG->msg)[mTCB_MSG->head_index] = msg;
	exit_int();
	return true;
}
/** 
 * 获取消息的长度. 
 * 获取消息的长度. 
 * @param[in]  mTCB_MSG 要存放的消息的消息控制块 
 * @retval 		 int 消息的长度
 */ 
int32 msg_len(TCB_MSG* mTCB_MSG){
	return mTCB_MSG->head_index;
}
