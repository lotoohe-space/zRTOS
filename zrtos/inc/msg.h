#ifndef _MSG_H__
#define _MSG_H__
#include "type.h"
//消息的最大长度
#define MSG_MAX_LEN 10
//一直等待
#define MSG_WAIT_ENDLESS 0xffffffff
typedef struct _TCB_MSG{
	//消息id
	uint32 msg_id;
	//队首指针
	int32 head_index;
	//消息,存放的指针
	 uint32 msg[MSG_MAX_LEN];
}TCB_MSG;

TCB_MSG* msg_create(void);
void msg_close(TCB_MSG* mTCB_MSG);

uint32 msg_get(TCB_MSG* mTCB_MSG,void *msg,uint32 msg_get_delay);
uint32 msg_put(TCB_MSG* mTCB_MSG,uint32 msg,uint32 msg_get_delay);
int32 msg_len(TCB_MSG* mTCB_MSG);
#endif
