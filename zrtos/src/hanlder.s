 ;�����л�
 IMPORT task_sw
 IMPORT task_mem_
 ;�����
 AREA    hanlder,CODE,READONLY 
 	 
enter_int
			export enter_int
			CPSID I ;PRIMASK=1 ���ж�
			BX LR ;����
exit_int
			export exit_int
			CPSIE I	;���ж�
			BX LR ;����
			
;��������ϵͳ
start_os	proc
			export start_os
			CPSID   I 
			;��������pendsvΪ������ȼ�
			;����pendsv���ж����ȼ�
			ldr r0,=0xE000ED22
			;������ȼ�
			ldr r1,=0xff
			;����
			strb r1,[r0]
			;����pspΪ0,�����ж��Ƿ��һ���������
			MOVS R0, #0 ;R0 = 0
			MSR PSP, R0 ;PSP = R0
			;����pendsv�ж�
			LDR R0, =0xE000ED04 ;R0 = 0xE000ED04
			LDR R1, =0x10000000 ;R1 = 0x10000000
			;���ô���
			STR.w R1, [R0] ;*(uint32_t *)NVIC_INT_CTRL = NVIC_PENDSVSET
			;���ж�
			CPSIE I ;
			;��ѭ��
os_start_hang 
			B os_start_hang
			endp

;����pendsv�жϣ��Ա�����жϵ���
open_scheduling proc
				export open_scheduling
				push {r0-r4,lr}
				LDR R0, =0xE000ED04
				LDR R1, =0x10000000 
				;����pendsv�ж�
				STR R1, [R0]
				pop	  {r0-r4,pc}
				endp
;pendsv�ж�
PendSV_Handler  PROC
                EXPORT  PendSV_Handler   
				REQUIRE8    ; ������������αָ���ֹ����������
				PRESERVE8   ; 8 �ֶ���	
				;�жϵ���ʱ���ܱ���ϣ�����ر��ж�
                cpsid I
				;���spָ���ֵ
				MRS R0, PSP ;R0 = PSP
					
				;�����һ��ִ��,��ִ��һ���жϵ���
				CBZ R0, thread_change 
				;���ǵ�һ���򱣻�r4-r11
				SUBS R0, R0, #0x20 ;R0 -= 0x20
				STM R0, {R4-R11} ;		
				
				;���汾�ε�ջ����ַ
				ldr r1,=task_mem_
				ldr r1,[r1]
				str R0,[r1]					
thread_change	;�������
				push {lr}
				ldr.w r0,=task_sw
				blx r0
				pop  {lr}

				LDM R0, {R4-R11} ;�ָ��µ�r4-r11��ֵ
				ADDS R0, R0, #0x20 ;R0 += 0x20
				MSR PSP, R0
				;�л����û��߳�ģʽ
				;lr �ĵ�2λΪ1ʱ�Զ��л�
				ORR LR, LR, #0x04  
				;���ж�
				cpsie I
				BX 	LR			
                ENDP			
				end
