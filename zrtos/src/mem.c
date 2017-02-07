#include "mem.h"
#include "task.h"
/** �ڴ����Ҫ8�ֽڶ��� */
__align(8) uint8 mem_malloc[MEM_SIZE]={0};
//�ڴ���ƿ�
uint16 mmt[MMTS]={0};


void z_malloc_init(void){
	uint32 i=0;
	enter_int();
	for(i=0;i<MMTS;i++){mmt[i]=0;}
	exit_int();
}
//��ȡ������ڴ�
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
//�����ڴ棬�ú���������ȫ
void *z_malloc(uint32 mem_num){
	static uint8 start_flag=0;
	uint32 i;
	uint32 j;
	uint8  suc_flag=false;
	volatile float bank_num_f;
	/** ������Ҫ������ٿ���ڴ� */
	volatile uint32 bank_num;
	if(!start_flag){
		start_flag=1;
		z_malloc_init();
	}
	bank_num_f	=(double)mem_num/(float)MEM_BANK_MIN_SIZE;
	bank_num=bank_num_f>((uint32)bank_num_f)?((uint32)bank_num_f)+1:((uint32)bank_num_f);
	if(bank_num>MMTS){return 0x00000000L;}
	enter_int();
	/** ������Ҫ�Ŀտ� */
	for(i=0;i<MMTS;i++){
		uint32 valid=0;
		//����ҵ��˿տ飬���Ƿ���������ô��Ŀտ�
		if(mmt[i]==0){
			for(j=i;j<i+bank_num&&j<MMTS;j++){
				if(mmt[j]==0){valid++;}
				else{break;}//����������ڴ�飬���˳�
			}
			if(valid>=bank_num){
				uint32 n;
				//����ɹ���,���ñ��Ϊ1
				for(n=i;n<i+bank_num;n++){mmt[n]=1;}
				//��һ���������ռ�õĴ�С
				mmt[i]=bank_num;
				//����Ϊ�ҵ���
				suc_flag=true;
				break;
			}
		}
	}
	exit_int();
	if(suc_flag){
		//����ҵ��ɹ�,���ػ�ȡ���ĵ�ַ
		return (mem_malloc+i*MEM_BANK_MIN_SIZE);
	}
	return 0x00000000L;
}
//�ͷ��ڴ棬�ú���������ȫ
uint32 z_free(void *mem_pointer){
	//�ҵ�ƫ�Ƶ�ַ
	uint32 offset;
	uint32 j=0;
	uint32 num;
	//Ϊ�����˳�
	if(!mem_pointer){return false;}
	//�ҵ��ڴ���ƫ�Ƶ�ַ
	offset=(uint32)mem_pointer-(uint32)&mem_malloc[0];
	//�ҵ�������е�ƫ�ƣ����ͷ�
	offset/=MEM_BANK_MIN_SIZE;
	enter_int();
	//���ָ����ڴ��еĿ�Ϊ0��˵�����ͷŹ��˻����ڴ淢����й¶
	if(!mmt[offset]){exit_int();return false;}
	num=mmt[offset];
	//���
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

