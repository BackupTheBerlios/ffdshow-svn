/*****************************************************************************
 *
 * XVID MPEG-4 VIDEO CODEC
 * - Aligned memory allocator -
 *
 * This program is an implementation of a part of one or more MPEG-4
 * Video tools as specified in ISO/IEC 14496-2 standard. ÿThose intending
 * to use this software module in hardware or software products are
 * advised that its use may infringe existing patents or copyrights, and
 * any such use would be at such party's own risk. ÿThe original
 * developer of this software module and his/her company, and subsequent
 * editors and their companies, will have no liability for use of this
 * software or modifications or derivatives thereof.
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation ; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. ÿSee the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA ÿ02111-1307 USA
 *
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "mem_align.h"

void *xvid_malloc(size_t size, uint8_t alignment)
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

void *xvid_calloc(size_t size1, size_t size2, uint8_t alignment)
{
 unsigned int size=size1*size2;
 uint8_t *ret=xvid_malloc(size,alignment);
 if (ret)
  memset(ret,0,size);
 return ret;
}

void xvid_free(void *mem_ptr)
{

	/* *(mem_ptr - 1) give us the offset to the real malloc block */
	free((uint8_t*)mem_ptr - *((uint8_t*)mem_ptr - 1));

}

