#ifndef _MSG_H__
#define _MSG_H__
#include "type.h"
//��Ϣ����󳤶�
#define MSG_MAX_LEN 10
//һֱ�ȴ�
#define MSG_WAIT_ENDLESS 0xffffffff
typedef struct _TCB_MSG{
	//��Ϣid
	uint32 msg_id;
	//����ָ��
	int32 head_index;
	//��Ϣ,��ŵ�ָ��
	 uint32 msg[MSG_MAX_LEN];
}TCB_MSG;

TCB_MSG* msg_create(void);
void msg_close(TCB_MSG* mTCB_MSG);

uint32 msg_get(TCB_MSG* mTCB_MSG,void *msg,uint32 msg_get_delay);
uint32 msg_put(TCB_MSG* mTCB_MSG,uint32 msg,uint32 msg_get_delay);
int32 msg_len(TCB_MSG* mTCB_MSG);
#endif
