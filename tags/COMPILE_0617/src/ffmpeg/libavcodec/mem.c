/*
 * default memory allocator for libavcodec
 * Copyright (c) 2002 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "avcodec.h"
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

/* you can redefine av_malloc and av_free in your project to use your
   memory allocator. You do not need to suppress this file because the
   linker will do it automatically */

static void *xvid_malloc(size_t size, uint8_t alignment)
{
	uint8_t *mem_ptr;
  
	if(!alignment)
	{

		/* We have not to satisfy any alignment */
		if((mem_ptr = (uint8_t *) malloc(size + 1)) != NULL)
		{

			/* Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) */
			*mem_ptr = 0;

			/* Return the mem_ptr pointer */
			return (void *) mem_ptr++;

		}

	}
	else
	{
		uint8_t *tmp;
	
		/*
		 * Allocate the required size memory + alignment so we
		 * can realign the data if necessary
		 */

		if((tmp = (uint8_t *) malloc(size + alignment)) != NULL)
		{

			/* Align the tmp pointer */
			mem_ptr = (uint8_t *)((uint32_t)(tmp + alignment - 1)&(~(uint32_t)(alignment - 1)));

			/*
			 * Special case where malloc have already satisfied the alignment
			 * We must add alignment to mem_ptr because we must store
			 * (mem_ptr - tmp) in *(mem_ptr-1)
			 * If we do not add alignment to mem_ptr then *(mem_ptr-1) points
			 * to a forbidden memory space
			 */
			if(mem_ptr == tmp) mem_ptr += alignment;

			/*
			 * (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
			 * the real malloc block allocated and free it in xvid_free
			 */
			*(mem_ptr - 1) = (uint8_t)(mem_ptr - tmp);

			/* Return the aligned pointer */
			return (void *) mem_ptr;

		}
	}

	return NULL;

}
/* memory alloc */
void *av_malloc(int size)
{
    void *ptr = xvid_malloc(size,64);
    if (!ptr) return NULL;
    memset(ptr, 0, size);
    return ptr;
}

/* NOTE: ptr = NULL is explicetly allowed */
void av_free(void *ptr)
{
    /* XXX: this test should not be needed on most libcs */
    if (ptr)
         free((uint8_t*)ptr - *((uint8_t*)ptr - 1));
}

