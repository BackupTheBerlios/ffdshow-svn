/**************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  native api
 *
 *  This program is an implementation of a part of one or more MPEG-4
 *  Video tools as specified in ISO/IEC 14496-2 standard.  Those intending
 *  to use this software module in hardware or software products are
 *  advised that its use may infringe existing patents or copyrights, and
 *  any such use would be at such party's own risk.  The original
 *  developer of this software module and his/her company, and subsequent
 *  editors and their companies, will have no liability for use of this
 *  software or modifications or derivatives thereof.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *************************************************************************/

/**************************************************************************
 *
 *  History:
 *
 *  17.03.2002  Added interpolate8x8_halfpel_hv_xmm
 *  22.12.2001  API change: added xvid_init() - Isibaar
 *  16.12.2001  inital version; (c)2001 peter ross <pross@cs.rmit.edu.au>
 *
 *************************************************************************/


#include "xvid.h"
#include "image/colorspace.h"
#include "utils/emms.h"
#include "dct/idct.h"
//#include "utils/mem_transfer.h"

int xvid_init(void *handle, int opt, void *param1, void *param2)
{
    int cpu_flags;
    XVID_INIT_PARAM *init_param;

    init_param = (XVID_INIT_PARAM *) param1;

    // force specific cpu settings?
    if((init_param->cpu_flags & XVID_CPU_FORCE) > 0)
        cpu_flags = init_param->cpu_flags;
    else {

#ifdef ARCH_X86
        cpu_flags = check_cpu_features();
#else
        cpu_flags = 0;
#endif
        init_param->cpu_flags = cpu_flags;
    }

    // initialize the function pointers
    idct_int32_init();
    xvid_idct_ptr = idct_int32;
    emms = emms_c;

    colorspace_init();

    rgb555_to_yv12 = rgb555_to_yv12_c;
    rgb565_to_yv12 = rgb565_to_yv12_c;
    rgb24_to_yv12 = rgb24_to_yv12_c;
    rgb32_to_yv12 = rgb32_to_yv12_c;
    yuv_to_yv12 = yuv_to_yv12_c;
    yuyv_to_yv12 = yuyv_to_yv12_c;
    uyvy_to_yv12 = uyvy_to_yv12_c;

    yv12_to_rgb555 = yv12_to_rgb555_c;
    yv12_to_rgb565 = yv12_to_rgb565_c;
    yv12_to_rgb24 = yv12_to_rgb24_c;
    yv12_to_rgb32 = yv12_to_rgb32_c;
    yv12_to_yuv = yv12_to_yuv_c;
    yv12_to_yuyv = yv12_to_yuyv_c;
    yv12_to_uyvy = yv12_to_uyvy_c;


#ifdef ARCH_X86
    if((cpu_flags & XVID_CPU_MMX) > 0) {
        xvid_idct_ptr = idct_mmx;

        emms = emms_mmx;

        rgb24_to_yv12 = rgb24_to_yv12_mmx;
        rgb32_to_yv12 = rgb32_to_yv12_mmx;
        yuv_to_yv12 = yuv_to_yv12_mmx;
        yuyv_to_yv12 = yuyv_to_yv12_mmx;
        uyvy_to_yv12 = uyvy_to_yv12_mmx;

        yv12_to_rgb24 = yv12_to_rgb24_mmx;
        yv12_to_rgb32 = yv12_to_rgb32_mmx;
        yv12_to_yuyv = yv12_to_yuyv_mmx;
        yv12_to_uyvy = yv12_to_uyvy_mmx;


    }

    if((cpu_flags & XVID_CPU_MMXEXT) > 0) {
        xvid_idct_ptr = idct_xmm;
        yuv_to_yv12 = yuv_to_yv12_xmm;

    }

    if((cpu_flags & XVID_CPU_SSE2) > 0) {
#ifdef EXPERIMENTAL_SSE2_CODE
        xvid_idct_ptr = idct_sse2;
#endif
    }

#endif
#ifdef ARCH_PPC
#ifdef ARCH_PPC_ALTIVEC
    calc_cbp = calc_cbp_altivec;
    fdct = fdct_altivec;
    idct = idct_altivec;
    sad16 = sad16_altivec;
    sad8 = sad8_altivec;
    dev16 = dev16_altivec;
#else
    calc_cbp = calc_cbp_ppc;
#endif
#endif
    
    // API version
    init_param->api_version = API_VERSION;

    // something clever has to be done for this
    init_param->core_build = 1000;

    return XVID_ERR_OK;
}
