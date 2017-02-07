#ifndef _MEM_H__
#define _MEM_H__
#include "type.h"

/** �ܷ�����ڴ��С */
#define MEM_BANK_MIN_SIZE 8
/** ������ڴ�Ĵ�С */
#define MEM_SIZE (6*1024)
/** �ڴ�����Ĵ�С */
#define MMTS (MEM_SIZE/MEM_BANK_MIN_SIZE)

uint8 z_get_free(void);
void *m_malloc(uint32 mem_num);
void m_free(void *mem);

void *z_malloc(uint32 mem_num);
uint32 z_free(void *mem_pointer);
#endif
