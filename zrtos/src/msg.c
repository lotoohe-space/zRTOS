/** 
* @file         msg.c
* @brief        ��Ϣ�����ʵ�ִ���
* @details  		����ϵͳ����Ϣ�����ʵ�ֵĴ���
* @author       z_space
* @date     		2016.8.22
* @version  		1.0
* @par Copyright (c):  
*    						��   
* @par History:          
*   version: author, date, desc\n 
*/ 
#include "msg.h"
#include "mem.h"
#include "task.h"
#include <string.h>
/** 
 * ����һ����Ϣ����. 
 * ����һ����Ϣ����,�ڴ�����ʧ���򷵻ش���. 
 * @param[in]  �� 
 * @retval 		 succeed �ɹ�
 * @retval		 null �ڴ�����ʧ��
 */ 
TCB_MSG* msg_create(void){
	//�����ڴ�
	TCB_MSG* mTCB_MSG=m_malloc(sizeof(TCB_MSG));
	if(mTCB_MSG==null){return null;}
	memset(mTCB_MSG,0,sizeof(TCB_MSG));
	mTCB_MSG->msg_id=(uint32)mTCB_MSG;
	mTCB_MSG->head_index=-1;
	
	return mTCB_MSG;
}
/** 
 * �ͷ�һ����Ϣ����. 
 * �ͷ�һ����Ϣ���У��ͷź���Ϣ���в��ܹ���ʹ��
 * @param[in]  mTCB_MSG Ҫ�ͷŵ���Ϣ���ƿ�
 * @retval 		 ��
 */
void msg_close(TCB_MSG* mTCB_MSG){
	m_free(mTCB_MSG);
}
/** 
 * ��ȡ��Ϣ. 
 * ����Ϣ�����л�ȡ��Ϣ
 * @param[in]  mTCB_MSG Ҫ��ȡ����Ϣ����Ϣ���ƿ� 
 * @param[in]  msg			Ҫ��ȡ����Ϣ��ŵ�ַ
 * @param[in]  msg_get_delay ��ȡ��Ϣ�ĳ�ʱʱ��,Ϊ0xffffffffʱһֱ�ȴ�
 * @retval 		 true �ɹ�
 * @retval		 false �ڴ�����ʧ��
 */ 
uint32 msg_get(TCB_MSG* mTCB_MSG,void *msg,uint32 msg_get_delay){
	int32 i=0;
	if(mTCB_MSG==null){return false;}
	do{
		//û����Ϣ
		if(mTCB_MSG->head_index<0){
			//���һֱ�ȴ�
			if(msg_get_delay==MSG_WAIT_ENDLESS){
				continue;
			}
			//�ȴ�һ��ʱ��
			if(msg_get_delay!=0){
				os_task_delay(msg_get_delay);
			}
			//��û����Ϣ���˳�
			if(mTCB_MSG->head_index<0){
				return false;
			}
		}
		//����Ϣ
		enter_int();
		//��ȡ��Ϣ
		*(uint32*)msg = (uint32)mTCB_MSG->msg[0];
		//��Ϣ����
		for(i=1;i<MSG_MAX_LEN;i++){
			mTCB_MSG->msg[i-1]=mTCB_MSG->msg[i];
		}
		//����ͷ��1
		mTCB_MSG->head_index--;
		exit_int();
		return true;
	}while(msg_get_delay==MSG_WAIT_ENDLESS);
	return false;
}
/** 
 * �����Ϣ. 
 * �����Ϣ����Ϣ���ƿ�
 * @param[in]  mTCB_MSG Ҫ��ŵ���Ϣ����Ϣ���ƿ� 
 * @param[in]  msg			Ҫ���͵���Ϣ�ĵ�ַ
 * @param[in]  msg_get_delay �����Ϣ�ĳ�ʱʱ��,Ϊ0xffffffffʱһֱ�ȴ�
 * @retval 		 true �ɹ�
 * @retval		 false �ڴ�����ʧ��
 */ 
uint32 msg_put(TCB_MSG* mTCB_MSG,uint32 msg,uint32 msg_get_delay){
	if(mTCB_MSG==null){return false;}
	//����ȴ�ʱ��û������
	if(msg_get_delay==MSG_WAIT_ENDLESS){
		//һֱ�ȴ�ֱ�������п�
		for(;mTCB_MSG->head_index >= MSG_MAX_LEN-1;);
	}else{
		//�����������
		if(mTCB_MSG->head_index >= MSG_MAX_LEN-1){
			//�ȴ�һ��ʱ��
			if(msg_get_delay!=0){
				os_task_delay(msg_get_delay);
			}
			//����������˳�
			if(mTCB_MSG->head_index >= MSG_MAX_LEN-1){
				return false;
			}
		}
	}
	enter_int();
	mTCB_MSG->head_index++;
	//�����Ϣ
	(mTCB_MSG->msg)[mTCB_MSG->head_index] = msg;
	exit_int();
	return true;
}
/** 
 * ��ȡ��Ϣ�ĳ���. 
 * ��ȡ��Ϣ�ĳ���. 
 * @param[in]  mTCB_MSG Ҫ��ŵ���Ϣ����Ϣ���ƿ� 
 * @retval 		 int ��Ϣ�ĳ���
 */ 
int32 msg_len(TCB_MSG* mTCB_MSG){
	return mTCB_MSG->head_index;
}
