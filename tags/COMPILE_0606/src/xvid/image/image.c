/**************************************************************************
 *
 *	XVID MPEG-4 VIDEO CODEC
 *	image stuff
 *
 *	This program is an implementation of a part of one or more MPEG-4
 *	Video tools as specified in ISO/IEC 14496-2 standard.  Those intending
 *	to use this software module in hardware or software products are
 *	advised that its use may infringe existing patents or copyrights, and
 *	any such use would be at such party's own risk.  The original
 *	developer of this software module and his/her company, and subsequent
 *	editors and their companies, will have no liability for use of this
 *	software or modifications or derivatives thereof.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *************************************************************************/

/**************************************************************************
 *
 *	History:
 *
 *  26.03.2002  interlacing support (field-based edging in set_edges)
 *	26.01.2002	rgb555, rgb565
 *	07.01.2001	commented u,v interpolation (not required for uv-block-based)
 *  23.12.2001  removed #ifdefs, added function pointers + init_common()
 *	22.12.2001	cpu #ifdefs
 *  19.12.2001  image_dump(); useful for debugging
 *	 6.12.2001	inital version; (c)2001 peter ross <pross@cs.rmit.edu.au>
 *
 *************************************************************************/


#include <stdlib.h>
#include <string.h>  // memcpy, memset

#include "../portab.h"
#include "../xvid.h"       // XVID_CSP_XXX's
//#include "../divx4.h"
#include "image.h"
#include "colorspace.h"

#define SAFETY	64
#define EDGE_SIZE2  (EDGE_SIZE/2)

int image_input(IMAGE * image, uint32_t width, int height, uint32_t edged_width,
			uint8_t * src, int csp)
{

/*	if (csp & XVID_CSP_VFLIP)
	{
		height = -height;
	}
*/

	switch(csp & ~XVID_CSP_VFLIP)
	{
	case XVID_CSP_RGB555 :
		rgb555_to_yv12(image->y, image->u, image->v, src, 
						width, height, edged_width);
		return 0;

	case XVID_CSP_RGB565 :
		rgb565_to_yv12(image->y, image->u, image->v, src, 
						width, height, edged_width);
		return 0;


	case XVID_CSP_RGB24 :
		rgb24_to_yv12(image->y, image->u, image->v, src, 
							width, height, edged_width);
		return 0;

	case XVID_CSP_RGB32 :
		rgb32_to_yv12(image->y, image->u, image->v, src, 
						width, height, edged_width);
		return 0;

	case XVID_CSP_I420 :
		yuv_to_yv12(image->y, image->u, image->v, src, 
						width, height, edged_width);
		return 0;

	case XVID_CSP_YV12 :	/* u/v swapped */
		yuv_to_yv12(image->y, image->v, image->u, src, 
						width, height, edged_width);
		return 0;

	case XVID_CSP_YUY2 :
		yuyv_to_yv12(image->y, image->u, image->v, src, 
						width, height, edged_width);
		return 0;

	case XVID_CSP_YVYU :	/* u/v swapped */
		yuyv_to_yv12(image->y, image->v, image->u, src, 
						width, height, edged_width);
		return 0;

	case XVID_CSP_UYVY :
		uyvy_to_yv12(image->y, image->u, image->v, src, 
						width, height, edged_width);
		return 0;
/*
	case XVID_CSP_USER :
		user_to_yuv_c(image->y, image->u, image->v, edged_width, 
					(DEC_PICTURE*)src,
				   width, height);
		return 0;
*/
	case XVID_CSP_NULL :
		break;
		
    }

	return -1;
}

int image_output(IMAGE * image, uint32_t width, int height, uint32_t edged_width,
			uint8_t * dst, uint32_t dst_stride,	int csp)
{
	if (csp & XVID_CSP_VFLIP)
	{
		height = -height;
	}

	switch(csp & ~XVID_CSP_VFLIP)
	{
	case XVID_CSP_RGB555 :
		yv12_to_rgb555(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_RGB565 :
		yv12_to_rgb565(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_RGB24 :
		yv12_to_rgb24(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_RGB32 :
		yv12_to_rgb32(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;
	
	case XVID_CSP_I420 :
		yv12_to_yuv(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_YV12 :	// u,v swapped
		yv12_to_yuv(dst, dst_stride,
				image->y, image->v, image->u, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_YUY2 :
		yv12_to_yuyv(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_YVYU :	// u,v swapped
		yv12_to_yuyv(dst, dst_stride,
				image->y, image->v, image->u, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_UYVY :
		yv12_to_uyvy(dst, dst_stride,
				image->y, image->u, image->v, edged_width, edged_width / 2,
				width, height);
		return 0;

	case XVID_CSP_NULL :
		return 0;

	}

	return -1;
}

