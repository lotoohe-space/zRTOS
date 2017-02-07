/** 
* @file         task.c
* @brief        任务切换与调度的实现源码
* @details  		任务切换,调度以及任务相关的实现
* @author       z_space
* @date     		2016.8.22
* @version  		1.0
* @par Copyright (c):  
*    						无   
* @par History:          
*   version: author, date, desc\n 
*/  

#include "task.h"
#include "mem.h"
#include "timer.h"
#include "stm32f10x_conf.h"
#include <stdlib.h>
/**  系统运行时间计数  */
volatile uint32 sys_clk_count=0;
/**  任务控制块列表  */
TASK_TCB *TASK_TCB_LIST[TASK_TCB_NUM]={0};
/**  上一次运行的堆栈地址  */
uint32 **task_mem_=0x00;
/**  正在运行的任务  */
TASK_TCB *TCBIng=0x00;
/** 
 * 查找能用的控制资源. 
 * 从任务的tcb控制列表中找到能够使用的任务块. 
 * @param[in]  无  
 * @retval  获取到能用的下标
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
 * 当一个任务结束时，会执行此函数
 * 当一个任务结束时，会执行此函数，在这个函数中删除当前任务
 * @param[in]  无
 * @retval  无 
 */
void task_end(void){
	if(TCBIng->mode==TASK_MODE_THREAD){
		os_task_del();
	}else{
		if(((TIMER*)TCBIng)->mode==TIMER_MODE_ONCE){
			os_task_del();
		}else{
			int32 d_timer=((TIMER*)TCBIng)->back_call_timer - TCBIng->run_count;
			//计算运行的时间差，然后进行延时
			if(d_timer>0){os_task_delay(d_timer);}
			timer_reset(((TIMER*)TCBIng));
		}
	}
}
/** 
 * 手动传入tcb控制块，进行创建一个任务
 * 手动传入tcb控制块,创建一个任务，并且处于就绪状态,该函数可选择创建为定时器任务还是，线程任务
 * @param[in]  fun_poi 指向任务的指针   
 * @param[in]  prg 给任务传递的参数
 * @param[in]	 task_num 任务分配的多少字长的堆栈
 * @param[in]  mode 创建的模式，有两种task timer
 * @retval  false	失败
 * @retval  true  成功  
 */
uint32 os_create_base_manual(
  TASK_TCB *mTASK_TCB,
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 *mem,
	uint32 mem_num,
	uint8 mode){
	//栈低指针
	uint32 *mem_low;
	//栈顶指针
	uint32 *mem_bottom;
	//
	int32 usable_task;
	enter_int();
	if(mTASK_TCB==null){return false;}
	if(mem==null){return false;}
	//获取可用的资源
	usable_task=os_find_tcb_idle();
	if(usable_task<0){return false;}
	/* 计算低位的地址 */
	mem_low=mem-mem_num;
	//保存栈顶的指针
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
	//设置堆栈的地址
	mTASK_TCB->mem_task=mem;
	//初始化任务控制块
	mTASK_TCB->run_count=0;
	mTASK_TCB->delay_count=0;
	//任务控制块的地址作为进程id
	mTASK_TCB->task_id=(uint32)mTASK_TCB;
	mTASK_TCB->status=true;
	//限制最大的优先级
	if(level>TASK_MIN_LEVEL){level=TASK_MIN_LEVEL;};
	//设置任务的优先级
	mTASK_TCB->level=level;
	//保存栈低指针
	mTASK_TCB->mem_low_task=mem_low;
	//保存栈顶的指针
	mTASK_TCB->mem_bottom_task=mem_bottom;	
	//当前任务的模式
	mTASK_TCB->mode=mode;
	//保存任务控制块
	TASK_TCB_LIST[usable_task]=mTASK_TCB;
	exit_int();	
	return true;
}
/** 
 * 创建一个任务
 * 创建一个任务，并且处于就绪状态,该函数可选择创建为定时器任务还是，线程任务
 * @param[in]  fun_poi 指向任务的指针   
 * @param[in]  prg 给任务传递的参数
 * @param[in]	 task_num 任务分配的多少字长的堆栈
 * @param[in]  mode 创建的模式，有两种task timer
 * @retval  false	失败
 * @retval  true  成功  
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
	//申请控制块的内存
	mTASK_TCB=(TASK_TCB *)z_malloc(sizeof(TASK_TCB));
	if(mTASK_TCB==null){return null;}
	//申请堆栈的内存
	mem=(uint32 *)z_malloc(sizeof(uint32)*task_num);
	if(mem==null){z_free(mTASK_TCB);return null;}		
	//创建任务
	if(os_create_base_manual(
		mTASK_TCB,
		fun_poi,
		prg,
		level,
	//堆栈内存
		&mem[task_num-1],
		task_num,
	//线程模式
		TASK_MODE_THREAD
	)==false){
		//任务表满了，所以失败了
		//释放内存
		z_free(mem);
		z_free(mTASK_TCB);
		return null;
	}
	return mTASK_TCB;
}
/** 
 * 创建一个任务
 * 创建一个任务，并且处于就绪状态
 * @param[in]  fun_poi 指向任务的指针   
 * @param[in]  prg 给任务传递的参数
 * @param[in]	 task_num 任务分配的多少字长的堆栈
 * @retval  false	失败
 * @retval  true  成功  
 */
TASK_TCB* os_create_task(
	void(*fun_poi)(void* prg),
	void *prg,
	uint32 level,
	uint32 task_num){
	return os_create_base(fun_poi,prg,level,task_num,TASK_MODE_THREAD);
}
/** 
 * 任务循环控制
 * 使正在休眠的任务的时间减一
 * @param[in]  无
 * @retval  无  
 */
void task_loop(void){
	uint32 i;
	for(i=0;i<TASK_TCB_NUM;i++){
		//线程处于活跃状态
		if(TASK_TCB_LIST[i]!=null&&TASK_TCB_LIST[i]->status==true){
			//是否有延时
			if(TASK_TCB_LIST[i]->delay_count>0){
				TASK_TCB_LIST[i]->delay_count--;
			}
			//运行时间+1
			TASK_TCB_LIST[i]->run_count++;	
		}
	}
}
/** 
 * 删除该任务
 * 删除正在运行的任务
 * @param[in]  无
 * @retval  无  
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
 * 任务延时函数
 * 正在运行的任务延时函数
 * @param[in]  count 延时多少个系统心跳
 * @retval  无 
 */
void os_task_delay(uint32 count){
	enter_int();
	TCBIng->delay_count=count;
	exit_int();
	open_scheduling();
}
/** 
 * 任务调度函数
 * 进行任务调度函数
 * @param[in]  无
 * @retval  无 
 */
void *task_sw(void){
	uint32 i=0;
	TASK_TCB *max_TASK_TCB=TASK_TCB_LIST[0];
	static TASK_TCB *back_task_tcb=null;
	//定时器模式
	if(TCBIng->mode==1){
		//如果是定时器模式
		if(((TIMER*)TCBIng)->reset_flag==1){
			//退到压栈时候的值
			*task_mem_=TCBIng->mem_bottom_task-15;
			((TIMER*)TCBIng)->reset_flag=0;
		}
	}
	//查找没有通优先级的
	if(back_task_tcb!=null){
		uint32 spotted=0;
		for(i=0;i<TASK_TCB_NUM;i++){
			if(back_task_tcb==TASK_TCB_LIST[i]){
				spotted=1;
				continue; 
			}
			//确保是没有被调度过的任务
			if(spotted==1){
				if(TASK_TCB_LIST[i]!=null&&
					TASK_TCB_LIST[i]->status==true&&
				//任务没有被延时
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
				//任务没有被延时
					TASK_TCB_LIST[i]->delay_count==0){
				if(max_TASK_TCB==null||(
					max_TASK_TCB->status==false||
				//任务没有被延时
					max_TASK_TCB->delay_count>0)){
					max_TASK_TCB=TASK_TCB_LIST[i];
					continue;
				}
				//获取优先级最高的
				if(max_TASK_TCB->level > TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
				}
			}
		}
	}
	step:

	//当前的堆栈
	task_mem_=&max_TASK_TCB->mem_task;
	//保存当前运行的tcb
	TCBIng=max_TASK_TCB;
	//保存上次获得的最大优先级
	back_task_tcb=max_TASK_TCB;
	//返回堆栈的地址
	return max_TASK_TCB->mem_task;
}
//操作系统的空闲任务
void os_idle(void *prg){
	//开启systick调度
	for(;;){
		prg=prg;
	}
}
/** 
 * 操作系统启动
 * 操作系统启动
 * @param[in]  无
 * @retval  无 
 */
void os_start(void){
	//创建一个空闲任务,空闲任务的优先级为最高优先级
	if(os_create_task(os_idle,null,TASK_MIN_LEVEL,50)==false){return ;}
	SysTick_Config(SystemCoreClock / 1000);
	//SysTick_Config(8000000 / 1000);
	//跳转到汇编启动操作系统
	start_os();
}
/** 
 * 获取操作系统的运行时间
 * 获取操作系统的运行时间
 * @param[in]  无
 * @retval  sys_clk_count 操作系统的运行时间
 */
uint32 os_systick_count(void){
	return sys_clk_count;
}
/** 
 * systick的中断函数
 * systick的中断函数
 * @param[in]  无
 * @retval  无 
 */
void SysTick_Handler(void){
	//系统的运行时间+1
	sys_clk_count++;
	//线程休眠时间处理
	task_loop();
	//开启pendsv中断
	open_scheduling();
}



