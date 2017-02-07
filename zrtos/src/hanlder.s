 ;任务切换
 IMPORT task_sw
 IMPORT task_mem_
 ;代码段
 AREA    hanlder,CODE,READONLY 
 	 
enter_int
			export enter_int
			CPSID I ;PRIMASK=1 关中断
			BX LR ;返回
exit_int
			export exit_int
			CPSIE I	;开中断
			BX LR ;返回
			
;开启操作系统
start_os	proc
			export start_os
			CPSID   I 
			;首先设置pendsv为最低优先级
			;设置pendsv的中断优先级
			ldr r0,=0xE000ED22
			;最低优先级
			ldr r1,=0xff
			;设置
			strb r1,[r0]
			;设置psp为0,用于判断是否第一次任务调度
			MOVS R0, #0 ;R0 = 0
			MSR PSP, R0 ;PSP = R0
			;开启pendsv中断
			LDR R0, =0xE000ED04 ;R0 = 0xE000ED04
			LDR R1, =0x10000000 ;R1 = 0x10000000
			;设置触发
			STR.w R1, [R0] ;*(uint32_t *)NVIC_INT_CTRL = NVIC_PENDSVSET
			;打开中断
			CPSIE I ;
			;死循环
os_start_hang 
			B os_start_hang
			endp

;出发pendsv中断，以便进行中断调度
open_scheduling proc
				export open_scheduling
				push {r0-r4,lr}
				LDR R0, =0xE000ED04
				LDR R1, =0x10000000 
				;进入pendsv中断
				STR R1, [R0]
				pop	  {r0-r4,pc}
				endp
;pendsv中断
PendSV_Handler  PROC
                EXPORT  PendSV_Handler   
				REQUIRE8    ; 加这两条对齐伪指令防止链接器报错
				PRESERVE8   ; 8 字对齐	
				;中断调度时不能被打断，这里关闭中断
                cpsid I
				;获得sp指针的值
				MRS R0, PSP ;R0 = PSP
					
				;如果第一次执行,则执行一次中断调度
				CBZ R0, thread_change 
				;不是第一次则保护r4-r11
				SUBS R0, R0, #0x20 ;R0 -= 0x20
				STM R0, {R4-R11} ;		
				
				;保存本次的栈顶地址
				ldr r1,=task_mem_
				ldr r1,[r1]
				str R0,[r1]					
thread_change	;任务调度
				push {lr}
				ldr.w r0,=task_sw
				blx r0
				pop  {lr}

				LDM R0, {R4-R11} ;恢复新的r4-r11的值
				ADDS R0, R0, #0x20 ;R0 += 0x20
				MSR PSP, R0
				;切换到用户线程模式
				;lr 的第2位为1时自动切换
				ORR LR, LR, #0x04  
				;开中断
				cpsie I
				BX 	LR			
                ENDP			
				end
