/** 
* @file         task.c
* @brief        �����л�����ȵ�ʵ��Դ��
* @details  		�����л�,�����Լ�������ص�ʵ��
* @author       z_space
* @date     		2016.8.22
* @version  		1.0
* @par Copyright (c):  
*    						��   
* @par History:          
*   version: author, date, desc\n 
*/  

#include "task.h"
#include "mem.h"
#include "timer.h"
#include "stm32f10x_conf.h"
#include <stdlib.h>
/**  ϵͳ����ʱ�����  */
volatile uint32 sys_clk_count=0;
/**  ������ƿ��б�  */
TASK_TCB *TASK_TCB_LIST[TASK_TCB_NUM]={0};
/**  ��һ�����еĶ�ջ��ַ  */
uint32 **task_mem_=0x00;
/**  �������е�����  */
TASK_TCB *TCBIng=0x00;
/** 
 * �������õĿ�����Դ. 
 * �������tcb�����б����ҵ��ܹ�ʹ�õ������. 
 * @param[in]  ��  
 * @retval  ��ȡ�����õ��±�
 */  
int32 os_find_tcb_idle(void){
	uint32 i;
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]==null){
			return i;
		}
	}
	return -1;
}
/** 
 * ��һ���������ʱ����ִ�д˺���
 * ��һ���������ʱ����ִ�д˺����������������ɾ����ǰ����
 * @param[in]  ��
 * @retval  �� 
 */
void task_end(void){
	if(TCBIng->mode==TASK_MODE_THREAD){
		os_task_del();
	}else{
		if(((TIMER*)TCBIng)->mode==TIMER_MODE_ONCE){
			os_task_del();
		}else{
			int32 d_timer=((TIMER*)TCBIng)->back_call_timer - TCBIng->run_count;
			//�������е�ʱ��Ȼ�������ʱ
			if(d_timer>0){os_task_delay(d_timer);}
			timer_reset(((TIMER*)TCBIng));
		}
	}
}
/** 
 * �ֶ�����tcb���ƿ飬���д���һ������
 * �ֶ�����tcb���ƿ�,����һ�����񣬲��Ҵ��ھ���״̬,�ú�����ѡ�񴴽�Ϊ��ʱ�������ǣ��߳�����
 * @param[in]  fun_poi ָ�������ָ��   
 * @param[in]  prg �����񴫵ݵĲ���
 * @param[in]	 task_num �������Ķ����ֳ��Ķ�ջ
 * @param[in]  mode ������ģʽ��������task timer
 * @retval  false	ʧ��
 * @retval  true  �ɹ�  
 */
uint32 os_create_base_manual(
  TASK_TCB *mTASK_TCB,
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 *mem,
	uint32 mem_num,
	uint8 mode){
	//ջ��ָ��
	uint32 *mem_low;
	//ջ��ָ��
	uint32 *mem_bottom;
	//
	int32 usable_task;
	enter_int();
	if(mTASK_TCB==null){return false;}
	if(mem==null){return false;}
	//��ȡ���õ���Դ
	usable_task=os_find_tcb_idle();
	if(usable_task<0){return false;}
	/* �����λ�ĵ�ַ */
	mem_low=mem-mem_num;
	//����ջ����ָ��
	mem_bottom=mem;

	/* Registers stacked as if auto-saved on exception */
	*(mem) = (uint32)0x01000000L; /* xPSR */
	*(--mem) = (uint32)fun_poi; /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	//*(--mem) = (uint32)0xFFFFFFFEL;
	*(--mem) = (uint32)task_end;
	*(--mem) = (uint32)0x12121212L; /* R12*/
	*(--mem) = (uint32)0x03030303L; /* R3 */
	*(--mem) = (uint32)0x02020202L; /* R2 */
	*(--mem) = (uint32)0x01010101L; /* R1 */
	*(--mem) = (uint32)prg; /* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem) = (uint32)0x11111111L; /* R11 */
	*(--mem) = (uint32)0x10101010L; /* R10 */
	*(--mem) = (uint32)0x09090909L; /* R9 */
	*(--mem) = (uint32)0x08080808L; /* R8 */
	*(--mem) = (uint32)0x07070707L; /* R7 */
	*(--mem) = (uint32)0x06060606L; /* R6 */
	*(--mem) = (uint32)0x05050505L; /* R5 */
	*(--mem) = (uint32)0x04040404L; /* R4 */	
	//���ö�ջ�ĵ�ַ
	mTASK_TCB->mem_task=mem;
	//��ʼ��������ƿ�
	mTASK_TCB->run_count=0;
	mTASK_TCB->delay_count=0;
	//������ƿ�ĵ�ַ��Ϊ����id
	mTASK_TCB->task_id=(uint32)mTASK_TCB;
	mTASK_TCB->status=true;
	//�����������ȼ�
	if(level>TASK_MIN_LEVEL){level=TASK_MIN_LEVEL;};
	//������������ȼ�
	mTASK_TCB->level=level;
	//����ջ��ָ��
	mTASK_TCB->mem_low_task=mem_low;
	//����ջ����ָ��
	mTASK_TCB->mem_bottom_task=mem_bottom;	
	//��ǰ�����ģʽ
	mTASK_TCB->mode=mode;
	//����������ƿ�
	TASK_TCB_LIST[usable_task]=mTASK_TCB;
	exit_int();	
	return true;
}
/** 
 * ����һ������
 * ����һ�����񣬲��Ҵ��ھ���״̬,�ú�����ѡ�񴴽�Ϊ��ʱ�������ǣ��߳�����
 * @param[in]  fun_poi ָ�������ָ��   
 * @param[in]  prg �����񴫵ݵĲ���
 * @param[in]	 task_num �������Ķ����ֳ��Ķ�ջ
 * @param[in]  mode ������ģʽ��������task timer
 * @retval  false	ʧ��
 * @retval  true  �ɹ�  
 */
TASK_TCB *os_create_base(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 task_num,
	uint8 mode
){
	uint32 *mem;
	TASK_TCB *mTASK_TCB;	
	//������ƿ���ڴ�
	mTASK_TCB=(TASK_TCB *)z_malloc(sizeof(TASK_TCB));
	if(mTASK_TCB==null){return null;}
	//�����ջ���ڴ�
	mem=(uint32 *)z_malloc(sizeof(uint32)*task_num);
	if(mem==null){z_free(mTASK_TCB);return null;}		
	//��������
	if(os_create_base_manual(
		mTASK_TCB,
		fun_poi,
		prg,
		level,
	//��ջ�ڴ�
		&mem[task_num-1],
		task_num,
	//�߳�ģʽ
		TASK_MODE_THREAD
	)==false){
		//��������ˣ�����ʧ����
		//�ͷ��ڴ�
		z_free(mem);
		z_free(mTASK_TCB);
		return null;
	}
	return mTASK_TCB;
}
/** 
 * ����һ������
 * ����һ�����񣬲��Ҵ��ھ���״̬
 * @param[in]  fun_poi ָ�������ָ��   
 * @param[in]  prg �����񴫵ݵĲ���
 * @param[in]	 task_num �������Ķ����ֳ��Ķ�ջ
 * @retval  false	ʧ��
 * @retval  true  �ɹ�  
 */
TASK_TCB* os_create_task(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 task_num){
	return os_create_base(fun_poi,prg,level,task_num,TASK_MODE_THREAD);
}
/** 
 * ����ѭ������
 * ʹ�������ߵ������ʱ���һ
 * @param[in]  ��
 * @retval  ��  
 */
void task_loop(void){
	uint32 i;
	for(i=0;i<TASK_TCB_NUM;i++){
		//�̴߳��ڻ�Ծ״̬
		if(TASK_TCB_LIST[i]!=null&&TASK_TCB_LIST[i]->status==true){
			//�Ƿ�����ʱ
			if(TASK_TCB_LIST[i]->delay_count>0){
				TASK_TCB_LIST[i]->delay_count--;
			}
			//����ʱ��+1
			TASK_TCB_LIST[i]->run_count++;	
		}
	}
}
/** 
 * ɾ��������
 * ɾ���������е�����
 * @param[in]  ��
 * @retval  ��  
 */
void os_task_del(void ){
	uint32 i;
	enter_int();
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]==TCBIng){
			m_free(TCBIng->mem_low_task);
			m_free(TCBIng);
			TASK_TCB_LIST[i]=null;
			break;
		}
	}	
	exit_int();
	open_scheduling();
}
/** 
 * ������ʱ����
 * �������е�������ʱ����
 * @param[in]  count ��ʱ���ٸ�ϵͳ����
 * @retval  �� 
 */
void os_task_delay(uint32 count){
	enter_int();
	TCBIng->delay_count=count;
	exit_int();
	open_scheduling();
}
/** 
 * ������Ⱥ���
 * ����������Ⱥ���
 * @param[in]  ��
 * @retval  �� 
 */
void *task_sw(void){
	uint32 i=0;
	TASK_TCB *max_TASK_TCB=TASK_TCB_LIST[0];
	static TASK_TCB *back_task_tcb=null;
	//��ʱ��ģʽ
	if(TCBIng->mode==1){
		//����Ƕ�ʱ��ģʽ
		if(((TIMER*)TCBIng)->reset_flag==1){
			//�˵�ѹջʱ���ֵ
			*task_mem_=TCBIng->mem_bottom_task-15;
			((TIMER*)TCBIng)->reset_flag=0;
		}
	}
	//����û��ͨ���ȼ���
	if(back_task_tcb!=null){
		uint32 spotted=0;
		for(i=0;i<TASK_TCB_NUM;i++){
			if(back_task_tcb==TASK_TCB_LIST[i]){
				spotted=1;
				continue; 
			}
			//ȷ����û�б����ȹ�������
			if(spotted==1){
				if(TASK_TCB_LIST[i]!=null&&
					TASK_TCB_LIST[i]->status==true&&
				//����û�б���ʱ
					TASK_TCB_LIST[i]->delay_count==0&&
					back_task_tcb->level==TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
					goto step;
				}
			}
		}
	}
	back_task_tcb=null;
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]!=null){
			if(TASK_TCB_LIST[i]->status==true&&
				//����û�б���ʱ
					TASK_TCB_LIST[i]->delay_count==0){
				if(max_TASK_TCB==null||(
					max_TASK_TCB->status==false||
				//����û�б���ʱ
					max_TASK_TCB->delay_count>0)){
					max_TASK_TCB=TASK_TCB_LIST[i];
					continue;
				}
				//��ȡ���ȼ���ߵ�
				if(max_TASK_TCB->level > TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
				}
			}
		}
	}
	step:

	//��ǰ�Ķ�ջ
	task_mem_=&max_TASK_TCB->mem_task;
	//���浱ǰ���е�tcb
	TCBIng=max_TASK_TCB;
	//�����ϴλ�õ�������ȼ�
	back_task_tcb=max_TASK_TCB;
	//���ض�ջ�ĵ�ַ
	return max_TASK_TCB->mem_task;
}
//����ϵͳ�Ŀ�������
void os_idle(void *prg){
	//����systick����
	for(;;){
		prg=prg;
	}
}
/** 
 * ����ϵͳ����
 * ����ϵͳ����
 * @param[in]  ��
 * @retval  �� 
 */
void os_start(void){
	//����һ����������,������������ȼ�Ϊ������ȼ�
	if(os_create_task(os_idle,null,TASK_MIN_LEVEL,50)==false){return ;}
	SysTick_Config(SystemCoreClock / 1000);
	//SysTick_Config(8000000 / 1000);
	//��ת�������������ϵͳ
	start_os();
}
/** 
 * ��ȡ����ϵͳ������ʱ��
 * ��ȡ����ϵͳ������ʱ��
 * @param[in]  ��
 * @retval  sys_clk_count ����ϵͳ������ʱ��
 */
uint32 os_systick_count(void){
	return sys_clk_count;
}
/** 
 * systick���жϺ���
 * systick���жϺ���
 * @param[in]  ��
 * @retval  �� 
 */
void SysTick_Handler(void){
	//ϵͳ������ʱ��+1
	sys_clk_count++;
	//�߳�����ʱ�䴦��
	task_loop();
	//����pendsv�ж�
	open_scheduling();
}



