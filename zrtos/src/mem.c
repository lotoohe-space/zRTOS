#include "mem.h"
#include "task.h"
/** 内存必须要8字节对齐 */
__align(8) uint8 mem_malloc[MEM_SIZE]={0};
//内存控制块
uint16 mmt[MMTS]={0};


void z_malloc_init(void){
	uint32 i=0;
	enter_int();
	for(i=0;i<MMTS;i++){mmt[i]=0;}
	exit_int();
}
//获取空余的内存
uint8 z_get_free(void){
	uint32 i;
	uint32 j=0;
	for(i=0;i<MMTS;i++){
		if(mmt[i]==0x00){
			j++;
		}
	}
	return (uint8)(((float)j)/((float)MMTS)*100);
}
//申请内存，该函数多任务安全
void *z_malloc(uint32 mem_num){
	static uint8 start_flag=0;
	uint32 i;
	uint32 j;
	uint8  suc_flag=false;
	volatile float bank_num_f;
	/** 任务需要分配多少块的内存 */
	volatile uint32 bank_num;
	if(!start_flag){
		start_flag=1;
		z_malloc_init();
	}
	bank_num_f	=(double)mem_num/(float)MEM_BANK_MIN_SIZE;
	bank_num=bank_num_f>((uint32)bank_num_f)?((uint32)bank_num_f)+1:((uint32)bank_num_f);
	if(bank_num>MMTS){return 0x00000000L;}
	enter_int();
	/** 查找需要的空块 */
	for(i=0;i<MMTS;i++){
		uint32 valid=0;
		//如果找到了空块，则看是否有连续这么多的空块
		if(mmt[i]==0){
			for(j=i;j<i+bank_num&&j<MMTS;j++){
				if(mmt[j]==0){valid++;}
				else{break;}//如果其中有内存块，则退出
			}
			if(valid>=bank_num){
				uint32 n;
				//申请成功了,设置标记为1
				for(n=i;n<i+bank_num;n++){mmt[n]=1;}
				//第一个管理格存放占用的大小
				mmt[i]=bank_num;
				//设置为找到了
				suc_flag=true;
				break;
			}
		}
	}
	exit_int();
	if(suc_flag){
		//如果找到成功,返回获取到的地址
		return (mem_malloc+i*MEM_BANK_MIN_SIZE);
	}
	return 0x00000000L;
}
//释放内存，该函数多任务安全
uint32 z_free(void *mem_pointer){
	//找到偏移地址
	uint32 offset;
	uint32 j=0;
	uint32 num;
	//为空则退出
	if(!mem_pointer){return false;}
	//找到内存块的偏移地址
	offset=(uint32)mem_pointer-(uint32)&mem_malloc[0];
	//找到管理块中的偏移，并释放
	offset/=MEM_BANK_MIN_SIZE;
	enter_int();
	//如果指向的内存中的块为0，说明被释放过了或者内存发生了泄露
	if(!mmt[offset]){exit_int();return false;}
	num=mmt[offset];
	//清空
	for(j=offset;j<offset+num;j++){mmt[j]=0;}
	exit_int();
	return true;
}

void *m_malloc(uint32 mem_num){
	return z_malloc(mem_num);
}

void m_free(void *mem){
	z_free(mem);
}

