#ifndef _MEM_H__
#define _MEM_H__
#include "type.h"

/** 能分配的内存大小 */
#define MEM_BANK_MIN_SIZE 8
/** 管理的内存的大小 */
#define MEM_SIZE (6*1024)
/** 内存管理表的大小 */
#define MMTS (MEM_SIZE/MEM_BANK_MIN_SIZE)

uint8 z_get_free(void);
void *m_malloc(uint32 mem_num);
void m_free(void *mem);

void *z_malloc(uint32 mem_num);
uint32 z_free(void *mem_pointer);
#endif
