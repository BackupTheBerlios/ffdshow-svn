#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_

#include <inttypes.h>

void *xvid_malloc(size_t size, uint8_t alignment);
void *xvid_calloc(size_t size1, size_t size2, uint8_t alignment);
void xvid_free(void *mem_ptr);

#endif /* _MEM_ALIGN_H_ */
