#include "timer.h"
#include "mem.h"
#include "task.h"

/** 
 * ����һ����ʱ��
 * ����һ����ʱ�������̿�ʼ����
 * @param[in]  fun_poi ������
 * @param[in]  prg	����
 * @param[in]  task_num	����ջ���ֳ�
 * @param[in]  timer_mode ��ʱ����ģʽ
 * @param[in]  timer_sys_count ��ʱ���Ķ�ʱֵ
 * @retval  ��ʱ����tcbָ��
 */
TIMER * timer_create(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 task_num,
	uint8 timer_mode,
	uint32 timer_sys_count
){
	TIMER *mTIMER;
	uint32 *mem;
	//������ƿ���ڴ�
	mTIMER=(TIMER *)z_malloc(sizeof(TIMER));
	if(mTIMER==null){return null;}
	//�����ջ���ڴ�
	mem=(uint32 *)z_malloc(sizeof(uint32)*task_num);
	if(mem==null){z_free(mTIMER);return null;}
	//��������
	if(os_create_base_manual(
		(TASK_TCB *)mTIMER,
		fun_poi,
		prg,
		TIMER_DEFINE_PRIORITY,
	//��ջ�ڴ�
		&mem[task_num-1],
		task_num,
	//��ʱ��ģʽ
		TASK_MODE_TIMER
	)==false){
		//��������ˣ�����ʧ����
		//�ͷ��ڴ�
		z_free(mem);
		z_free(mTIMER);
		return null;
	}
	enter_int();
	//����ʱ��ص�һ��,һ����������ʱ
	mTIMER->back_call_timer=timer_sys_count;
	//��ʱ��ģʽ
	mTIMER->mode=timer_mode;
	//����δ��λģʽ
	mTIMER->reset_flag=0;	
	//��ʱ����ʼ����ֹͣ״̬
	mTIMER->mTASK_TCB.status=false;
	//�����ĵ�ַ
	mTIMER->fun_addr=(uint32)fun_poi;
	exit_int();
	return mTIMER;
}
/** 
 * ���ö�ʱ���Ķ�ʱֵ
 * ���ö�ʱ���Ķ�ʱֵ
 * @param[in] mTIMER ��ʱ����ָ��
 * @param[in] val ��ʱ���µ�ֵ
 * @retval  true �ɹ�
 * @retval  false ʧ��
 */
uint32 timer_set_cycle_val(TIMER *mTIMER,uint32 val){
	if(mTIMER==null){return false;}
	enter_int();
	mTIMER->back_call_timer=val;
	exit_int();
	return true;		
}
/** 
 * ֹͣ�����ʱ��
 * ֹͣ�����ʱ��
 * @param[in] mTIMER ��ʱ����ָ��
 * @retval  true �ɹ�
 * @retval  false ʧ��
 */
uint32 timer_stop(TIMER *mTIMER){
	if(mTIMER==null){return false;}
	enter_int();
	mTIMER->mTASK_TCB.status=false;
	exit_int();
	return true;	
}
/** 
 * ���������ʱ��
 * ���������ʱ��
 * @param[in] mTIMER ��ʱ����ָ��
 * @retval  true �ɹ�
 * @retval  false ʧ��
 */
uint32 timer_start(TIMER *mTIMER){
	if(mTIMER==null){return false;}
	enter_int();
	mTIMER->mTASK_TCB.status=true;
	exit_int();
	return true;
}
/** 
 * ɾ�������ʱ��
 * ɾ�������ʱ��
 * @param[in]  ��
 * @retval  ��
 */
void timer_del(void){
	/* ����ֱ�ӵ���task��ɾ����������Ϊ�̳���task */
	os_task_del();
}
/** 
 * ��λ��ʱ�����ʼ��״̬
 * @param[in] mTIMER ��ʱ����ָ��
 * @retval  ��
 */
void timer_reset(TIMER *mTIMER){
	uint32 *mem;
	enter_int();
	//��λջ����ָ��
	mTIMER->mTASK_TCB.mem_task=mTIMER->mTASK_TCB.mem_bottom_task;
	//��λ��ʱֵ
	mTIMER->mTASK_TCB.delay_count=mTIMER->back_call_timer;
	//��λ���е�ʱ��
	mTIMER->mTASK_TCB.run_count=0;
	//����Ϊ��λģʽ
	mTIMER->reset_flag=1;	
	mem=mTIMER->mTASK_TCB.mem_task;
	//��λ�Ĵ�����ֵ
	/* Registers stacked as if auto-saved on exception */
	*(mem) = (uint32)0x01000000L; /* xPSR */
	*(--mem) = (uint32)(mTIMER->fun_addr); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	//*(--mem) = (uint32)0xFFFFFFFEL;
	*(--mem) = (uint32)task_end;
	*(--mem) = (uint32)0x12121212L; /* R12*/
	*(--mem) = (uint32)0x03030303L; /* R3 */
	*(--mem) = (uint32)0x02020202L; /* R2 */
	*(--mem) = (uint32)0x01010101L; /* R1 */
	*(--mem) = (uint32)0x00000000L; /* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem) = (uint32)0x11111111L; /* R11 */
	*(--mem) = (uint32)0x10101010L; /* R10 */
	*(--mem) = (uint32)0x09090909L; /* R9 */
	*(--mem) = (uint32)0x08080808L; /* R8 */
	*(--mem) = (uint32)0x07070707L; /* R7 */
	*(--mem) = (uint32)0x06060606L; /* R6 */
	*(--mem) = (uint32)0x05050505L; /* R5 */
	*(--mem) = (uint32)0x04040404L; /* R4 */
	mTIMER->mTASK_TCB.mem_task=mem;
	exit_int();
	open_scheduling();
}
