#include "timer.h"
#include "mem.h"
#include "task.h"

/** 
 * 创建一个定时器
 * 创建一个定时器，立刻开始运行
 * @param[in]  fun_poi 任务函数
 * @param[in]  prg	参数
 * @param[in]  task_num	任务栈的字长
 * @param[in]  timer_mode 定时器的模式
 * @param[in]  timer_sys_count 定时器的定时值
 * @retval  定时器的tcb指针
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
	//申请控制块的内存
	mTIMER=(TIMER *)z_malloc(sizeof(TIMER));
	if(mTIMER==null){return null;}
	//申请堆栈的内存
	mem=(uint32 *)z_malloc(sizeof(uint32)*task_num);
	if(mem==null){z_free(mTIMER);return null;}
	//创建任务
	if(os_create_base_manual(
		(TASK_TCB *)mTIMER,
		fun_poi,
		prg,
		TIMER_DEFINE_PRIORITY,
	//堆栈内存
		&mem[task_num-1],
		task_num,
	//定时器模式
		TASK_MODE_TIMER
	)==false){
		//任务表满了，所以失败了
		//释放内存
		z_free(mem);
		z_free(mTIMER);
		return null;
	}
	enter_int();
	//多少时间回调一次,一来就设置延时
	mTIMER->back_call_timer=timer_sys_count;
	//定时器模式
	mTIMER->mode=timer_mode;
	//设置未复位模式
	mTIMER->reset_flag=0;	
	//定时器开始处于停止状态
	mTIMER->mTASK_TCB.status=false;
	//函数的地址
	mTIMER->fun_addr=(uint32)fun_poi;
	exit_int();
	return mTIMER;
}
/** 
 * 设置定时器的定时值
 * 设置定时器的定时值
 * @param[in] mTIMER 定时器的指针
 * @param[in] val 定时器新的值
 * @retval  true 成功
 * @retval  false 失败
 */
uint32 timer_set_cycle_val(TIMER *mTIMER,uint32 val){
	if(mTIMER==null){return false;}
	enter_int();
	mTIMER->back_call_timer=val;
	exit_int();
	return true;		
}
/** 
 * 停止这个定时器
 * 停止这个定时器
 * @param[in] mTIMER 定时器的指针
 * @retval  true 成功
 * @retval  false 失败
 */
uint32 timer_stop(TIMER *mTIMER){
	if(mTIMER==null){return false;}
	enter_int();
	mTIMER->mTASK_TCB.status=false;
	exit_int();
	return true;	
}
/** 
 * 启动这个定时器
 * 启动这个定时器
 * @param[in] mTIMER 定时器的指针
 * @retval  true 成功
 * @retval  false 失败
 */
uint32 timer_start(TIMER *mTIMER){
	if(mTIMER==null){return false;}
	enter_int();
	mTIMER->mTASK_TCB.status=true;
	exit_int();
	return true;
}
/** 
 * 删除这个定时器
 * 删除这个定时器
 * @param[in]  无
 * @retval  无
 */
void timer_del(void){
	/* 这里直接调用task的删除函数，因为继承了task */
	os_task_del();
}
/** 
 * 复位定时器到最开始的状态
 * @param[in] mTIMER 定时器的指针
 * @retval  无
 */
void timer_reset(TIMER *mTIMER){
	uint32 *mem;
	enter_int();
	//复位栈顶的指针
	mTIMER->mTASK_TCB.mem_task=mTIMER->mTASK_TCB.mem_bottom_task;
	//复位定时值
	mTIMER->mTASK_TCB.delay_count=mTIMER->back_call_timer;
	//复位运行的时间
	mTIMER->mTASK_TCB.run_count=0;
	//设置为复位模式
	mTIMER->reset_flag=1;	
	mem=mTIMER->mTASK_TCB.mem_task;
	//复位寄存器的值
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
