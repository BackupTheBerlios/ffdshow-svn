/*
    Copyright (C) 2001-2002 Michael Niedermayer <michaelni@gmx.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
  supported Input formats: YV12, I420/IYUV, YUY2, BGR32, BGR24, BGR16, BGR15, RGB32, RGB24, Y8/Y800, YVU9/IF09
  supported output formats: YV12, I420/IYUV, {BGR,RGB}{1,4,8,15,16,24,32}, Y8/Y800, YVU9/IF09
  {BGR,RGB}{1,4,8,15,16} support dithering
  
  unscaled special converters (YV12=I420=IYUV, Y800=Y8)
  YV12 -> {BGR,RGB}{1,4,8,15,16,24,32}
  x -> x
  YUV9 -> YV12
  YUV9/YV12 -> Y800
  Y800 -> YUV9/YV12
  BGR24 -> BGR32 & RGB24 -> RGB32
  BGR32 -> BGR24 & RGB32 -> RGB24
  BGR15 -> BGR16
*/

/* 
tested special converters (most are tested actually but i didnt write it down ...)
 YV12 -> BGR16
 YV12 -> YV12
 BGR15 -> BGR16
 BGR16 -> BGR16
 YVU9 -> YV12

untested special converters
  YV12/I420 -> BGR15/BGR24/BGR32 (its the yuv2rgb stuff, so it should be ok)
  YV12/I420 -> YV12/I420
  YUY2/BGR15/BGR24/BGR32/RGB24/RGB32 -> same format
  BGR24 -> BGR32 & RGB24 -> RGB32
  BGR32 -> BGR24 & RGB32 -> RGB24
  BGR24 -> YV12
*/

#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "../config.h"
#include "../mangle.h"
#include "../mem_align.h"
#include <assert.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include "swscale.h"
#include "../cpudetect.h"
#include "../bswap.h"
#include "../libvo/img_format.h"
#include "../mp_msg.h"

#define MSG_WARN(args...) mp_msg(MSGT_SWS,MSGL_WARN, ##args )
#define MSG_FATAL(args...) mp_msg(MSGT_SWS,MSGL_FATAL, ##args )
#define MSG_ERR(args...) mp_msg(MSGT_SWS,MSGL_ERR, ##args )
#define MSG_V(args...) mp_msg(MSGT_SWS,MSGL_V, ##args )
#define MSG_DBG2(args...) mp_msg(MSGT_SWS,MSGL_DBG2, ##args )
#define MSG_INFO(args...) mp_msg(MSGT_SWS,MSGL_INFO, ##args )

#undef MOVNTQ
#undef PAVGB

//#undef HAVE_MMX2
//#define HAVE_3DNOW
//#undef HAVE_MMX
//#undef ARCH_X86
//#define WORDS_BIGENDIAN
#define DITHER1XBPP

#define FAST_BGR2YV12 // use 7 bit coeffs instead of 15bit

#define RET 0xC3 //near return opcode for X86

#ifdef MP_DEBUG
#define ASSERT(x) assert(x);
#else
#define ASSERT(x) ;
#endif

#ifdef M_PI
#define PI M_PI
#else
#define PI 3.14159265358979323846
#endif

//FIXME replace this with something faster
#define isPlanarYUV(x) ((x)==IMGFMT_YV12)
#define isYUV(x)       ((x)==IMGFMT_YUY2)
#define isGray(x)      (0)
#define isPacked(x)    (0)
#define isSupportedIn(x)  ((x)==IMGFMT_YV12)
#define isSupportedOut(x) ((x)==IMGFMT_YV12)

extern int verbose; // defined in mplayer.c
/*
NOTES
Special versions: fast Y 1:1 scaling (no interpolation in y direction)

TODO
more intelligent missalignment avoidance for the horizontal scaler
write special vertical cubic upscale version
Optimize C code (yv12 / minmax)
add support for packed pixel yuv input & output
add support for Y8 output
optimize bgr24 & bgr32
add BGR4 output support
write special BGR->BGR scaler
deglobalize yuv2rgb*.c
*/

#define ABS(a) ((a) > 0 ? (a) : (-(a)))
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

#ifdef ARCH_X86
#define CAN_COMPILE_X86_ASM
#endif

#ifdef CAN_COMPILE_X86_ASM
static uint64_t __attribute__((aligned(8))) yCoeff=    0x2568256825682568LL;
static uint64_t __attribute__((aligned(8))) vrCoeff=   0x3343334333433343LL;
static uint64_t __attribute__((aligned(8))) ubCoeff=   0x40cf40cf40cf40cfLL;
static uint64_t __attribute__((aligned(8))) vgCoeff=   0xE5E2E5E2E5E2E5E2LL;
static uint64_t __attribute__((aligned(8))) ugCoeff=   0xF36EF36EF36EF36ELL;
static uint64_t __attribute__((aligned(8))) bF8=       0xF8F8F8F8F8F8F8F8LL;
static uint64_t __attribute__((aligned(8))) bFC=       0xFCFCFCFCFCFCFCFCLL;
static uint64_t __attribute__((aligned(8))) w400=      0x0400040004000400LL;
static uint64_t __attribute__((aligned(8))) w80=       0x0080008000800080LL;
static uint64_t __attribute__((aligned(8))) w10=       0x0010001000100010LL;
static uint64_t __attribute__((aligned(8))) w02=       0x0002000200020002LL;
static uint64_t __attribute__((aligned(8))) bm00001111=0x00000000FFFFFFFFLL;
static uint64_t __attribute__((aligned(8))) bm00000111=0x0000000000FFFFFFLL;
static uint64_t __attribute__((aligned(8))) bm11111000=0xFFFFFFFFFF000000LL;
static uint64_t __attribute__((aligned(8))) bm01010101=0x00FF00FF00FF00FFLL;

static volatile uint64_t __attribute__((aligned(8))) b5Dither;
static volatile uint64_t __attribute__((aligned(8))) g5Dither;
static volatile uint64_t __attribute__((aligned(8))) g6Dither;
static volatile uint64_t __attribute__((aligned(8))) r5Dither;

static uint64_t __attribute__((aligned(8))) dither4[2]={
        0x0103010301030103LL,
        0x0200020002000200LL,};

static uint64_t __attribute__((aligned(8))) dither8[2]={
        0x0602060206020602LL,
        0x0004000400040004LL,};

static uint64_t __attribute__((aligned(8))) b16Mask=   0x001F001F001F001FLL;
static uint64_t __attribute__((aligned(8))) g16Mask=   0x07E007E007E007E0LL;
static uint64_t __attribute__((aligned(8))) r16Mask=   0xF800F800F800F800LL;
static uint64_t __attribute__((aligned(8))) b15Mask=   0x001F001F001F001FLL;
static uint64_t __attribute__((aligned(8))) g15Mask=   0x03E003E003E003E0LL;
static uint64_t __attribute__((aligned(8))) r15Mask=   0x7C007C007C007C00LL;

static uint64_t __attribute__((aligned(8))) M24A=   0x00FF0000FF0000FFLL;
static uint64_t __attribute__((aligned(8))) M24B=   0xFF0000FF0000FF00LL;
static uint64_t __attribute__((aligned(8))) M24C=   0x0000FF0000FF0000LL;

static const uint64_t bgr2YOffset __attribute__((aligned(8))) = 0x1010101010101010ULL;
static const uint64_t bgr2UVOffset __attribute__((aligned(8)))= 0x8080808080808080ULL;
static const uint64_t w1111       __attribute__((aligned(8))) = 0x0001000100010001ULL;
#endif

// clipping helper table for C implementations:
static unsigned char clip_table[768];

//global sws_flags from the command line
//0: SWS_FAST_BILINEAR
//1: SWS_BILINEAR
//2: SWS_BICUBIC
//3: SWS_X
//4: SWS_POINT
//5: SWS_AREA
//6: SWS_BICUBLIN
//7: SWS_GAUSS
//8: SWS_SINC
//9: SWS_LANCZOS
//10:SWS_SPLINE
//int sws_flags=2;

//global srcFilter
SwsFilter src_filter= {NULL, NULL, NULL, NULL};

//float sws_lum_gblur= 0.0;
//float sws_chr_gblur= 0.0;
int sws_chr_vshift= 0;
int sws_chr_hshift= 0;
//float sws_chr_sharpen= 0.0;
//float sws_lum_sharpen= 0.0;

/* cpuCaps combined from cpudetect and whats actually compiled in
   (if there is no support for something compiled in it wont appear here) */
static CpuCaps cpuCaps;

void (*swScale)(SwsContext *context, uint8_t* src[], int srcStride[], int srcSliceY,
             int srcSliceH, uint8_t* dst[], int dstStride[])=NULL;

static SwsVector *getConvVec(SwsVector *a, SwsVector *b);
static inline void orderYUV(int format, uint8_t * sortedP[], int sortedStride[], uint8_t * p[], int stride[]);

extern const uint8_t dither_2x2_4[2][8];
extern const uint8_t dither_2x2_8[2][8];
extern const uint8_t dither_8x8_32[8][8];
extern const uint8_t dither_8x8_73[8][8];
extern const uint8_t dither_8x8_220[8][8];

#ifdef CAN_COMPILE_X86_ASM
void in_asm_used_var_warning_killer()
{
 volatile int i= yCoeff+vrCoeff+ubCoeff+vgCoeff+ugCoeff+bF8+bFC+w400+w80+w10+
 bm00001111+bm00000111+bm11111000+b16Mask+g16Mask+r16Mask+b15Mask+g15Mask+r15Mask+
 M24A+M24B+M24C+w02 + b5Dither+g5Dither+r5Dither+g6Dither+dither4[0]+dither8[0]+bm01010101;
 if(i) i=0;
}
#endif

static inline void yuv2yuvXinC(int16_t *lumFilter, int16_t **lumSrc, int lumFilterSize,
                                    int16_t *chrFilter, int16_t **chrSrc, int chrFilterSize,
				    uint8_t *dest, uint8_t *uDest, uint8_t *vDest, int dstW, int chrDstW)
{
        //FIXME Optimize (just quickly writen not opti..)
        int i;
        for(i=0; i<dstW; i++)
        {
                int val=0;
                int j;
                for(j=0; j<lumFilterSize; j++)
                        val += lumSrc[j][i] * lumFilter[j];

                dest[i]= MIN(MAX(val>>19, 0), 255);
        }

        if(uDest != NULL)
		for(i=0; i<chrDstW; i++)
                {
                        int u=0;
                        int v=0;
                        int j;
                        for(j=0; j<chrFilterSize; j++)
                        {
                                u += chrSrc[j][i] * chrFilter[j];
                                v += chrSrc[j][i + 2048] * chrFilter[j];
                        }

                        uDest[i]= MIN(MAX(u>>19, 0), 255);
                        vDest[i]= MIN(MAX(v>>19, 0), 255);
                }
}


//Note: we have C, X86, MMX, MMX2, 3DNOW version therse no 3DNOW+MMX2 one
//Plain C versions
#if !defined (HAVE_MMX) || defined (RUNTIME_CPUDETECT)
#define COMPILE_C
#endif

#ifdef CAN_COMPILE_X86_ASM

#if (defined (HAVE_MMX) && !defined (HAVE_3DNOW) && !defined (HAVE_MMX2)) || defined (RUNTIME_CPUDETECT)
#define COMPILE_MMX
#endif

#if defined (HAVE_MMX2) || defined (RUNTIME_CPUDETECT)
#define COMPILE_MMX2
#endif

#if (defined (HAVE_3DNOW) && !defined (HAVE_MMX2)) || defined (RUNTIME_CPUDETECT)
#define COMPILE_3DNOW
#endif
#endif //CAN_COMPILE_X86_ASM

#undef HAVE_MMX
#undef HAVE_MMX2
#undef HAVE_3DNOW

#ifdef COMPILE_C
#undef HAVE_MMX
#undef HAVE_MMX2
#undef HAVE_3DNOW
#define RENAME(a) a ## _C
#include "swscale_template.c"
#endif

#ifdef CAN_COMPILE_X86_ASM

//X86 versions
/*
#undef RENAME
#undef HAVE_MMX
#undef HAVE_MMX2
#undef HAVE_3DNOW
#define ARCH_X86
#define RENAME(a) a ## _X86
#include "swscale_template.c"
*/
//MMX versions
#ifdef COMPILE_MMX
#undef RENAME
#define HAVE_MMX
#undef HAVE_MMX2
#undef HAVE_3DNOW
#define RENAME(a) a ## _MMX
#include "swscale_template.c"
#endif

//MMX2 versions
#ifdef COMPILE_MMX2
#undef RENAME
#define HAVE_MMX
#define HAVE_MMX2
#undef HAVE_3DNOW
#define RENAME(a) a ## _MMX2
#include "swscale_template.c"
#endif

//3DNOW versions
#ifdef COMPILE_3DNOW
#undef RENAME
#define HAVE_MMX
#undef HAVE_MMX2
#define HAVE_3DNOW
#define RENAME(a) a ## _3DNow
#include "swscale_template.c"
#endif

#endif //CAN_COMPILE_X86_ASM

// minor note: the HAVE_xyz is messed up after that line so dont use it


// old global scaler, dont use for new code
// will use sws_flags from the command line
void SwScale_YV12slice(unsigned char* src[], int srcStride[], int srcSliceY ,
                             int srcSliceH, uint8_t* dst[], int dstStride, int dstbpp,
                             int srcW, int srcH, int dstW, int dstH){

        static SwsContext *context=NULL;
        int dstFormat;
        int dstStride3[3]= {dstStride, dstStride>>1, dstStride>>1};

        switch(dstbpp)
        {
                case 12: dstFormat= IMGFMT_YV12;        break;
                default: return;
        }

        if(!context) context=getSwsContextFromCmdLine(srcW, srcH, IMGFMT_YV12, dstW, dstH, dstFormat,2,0,0,0,0);

        context->swScale(context, src, srcStride, srcSliceY, srcSliceH, dst, dstStride3);
}

void swsGetFlagsAndFilterFromCmdLine(int *flags, SwsFilter **srcFilterParam, SwsFilter **dstFilterParam,
                                     int sws_flags,int Isws_lum_gblur,int Isws_chr_gblur,int Isws_lum_sharpen,int Isws_chr_sharpen)
{
        static int firstTime=1;
        float sws_lum_gblur  =Isws_lum_gblur  /100.0;
        float sws_chr_gblur  =Isws_chr_gblur  /100.0;
        float sws_lum_sharpen=Isws_lum_sharpen/100.0;
        float sws_chr_sharpen=Isws_chr_sharpen/100.0;
	*flags=0;

#ifdef ARCH_X86
        if(gCpuCaps.hasMMX)
                asm volatile("emms\n\t"::: "memory"); //FIXME this shouldnt be required but it IS (even for non mmx versions)
#endif
        if(firstTime)
        {
                firstTime=0;
		*flags= SWS_PRINT_INFO;
        }
	else if(verbose>1) *flags= SWS_PRINT_INFO;

        if(src_filter.lumH) freeVec(src_filter.lumH);
        if(src_filter.lumV) freeVec(src_filter.lumV);
        if(src_filter.chrH) freeVec(src_filter.chrH);
        if(src_filter.chrV) freeVec(src_filter.chrV);

        if(sws_lum_gblur!=0.0){
                src_filter.lumH= getGaussianVec(sws_lum_gblur, 3.0);
                src_filter.lumV= getGaussianVec(sws_lum_gblur, 3.0);
        }else{
                src_filter.lumH= getIdentityVec();
                src_filter.lumV= getIdentityVec();
        }

        if(sws_chr_gblur!=0.0){
                src_filter.chrH= getGaussianVec(sws_chr_gblur, 3.0);
                src_filter.chrV= getGaussianVec(sws_chr_gblur, 3.0);
        }else{
                src_filter.chrH= getIdentityVec();
                src_filter.chrV= getIdentityVec();
        }

        if(sws_chr_sharpen!=0.0){
                SwsVector *g= getConstVec(-1.0, 3);
                SwsVector *id= getConstVec(10.0/sws_chr_sharpen, 1);
                g->coeff[1]=2.0;
                addVec(id, g);
                convVec(src_filter.chrH, id);
                convVec(src_filter.chrV, id);
                freeVec(g);
                freeVec(id);
        }

        if(sws_lum_sharpen!=0.0){
                SwsVector *g= getConstVec(-1.0, 3);
                SwsVector *id= getConstVec(10.0/sws_lum_sharpen, 1);
                g->coeff[1]=2.0;
                addVec(id, g);
                convVec(src_filter.lumH, id);
                convVec(src_filter.lumV, id);
                freeVec(g);
                freeVec(id);
        }

        if(sws_chr_hshift)
                shiftVec(src_filter.chrH, sws_chr_hshift);

        if(sws_chr_vshift)
                shiftVec(src_filter.chrV, sws_chr_vshift);

        normalizeVec(src_filter.chrH, 1.0);
        normalizeVec(src_filter.chrV, 1.0);
        normalizeVec(src_filter.lumH, 1.0);
        normalizeVec(src_filter.lumV, 1.0);

        if(verbose > 1) printVec(src_filter.chrH);
        if(verbose > 1) printVec(src_filter.lumH);

        switch(sws_flags)
        {
		case 0: *flags|= SWS_FAST_BILINEAR; break;
		case 1: *flags|= SWS_BILINEAR; break;
		case 2: *flags|= SWS_BICUBIC; break;
		case 3: *flags|= SWS_X; break;
		case 4: *flags|= SWS_POINT; break;
		case 5: *flags|= SWS_AREA; break;
		case 6: *flags|= SWS_BICUBLIN; break;
		case 7: *flags|= SWS_GAUSS; break;
		case 8: *flags|= SWS_SINC; break;
		case 9: *flags|= SWS_LANCZOS; break;
		case 10:*flags|= SWS_SPLINE; break;
		default:*flags|= SWS_BILINEAR; break;
        }

	*srcFilterParam= &src_filter;
	*dstFilterParam= NULL;
}

// will use sws_flags & src_filter (from cmd line)
SwsContext *getSwsContextFromCmdLine(int srcW, int srcH, int srcFormat, int dstW, int dstH, int dstFormat,
                                     int sws_flags,int Isws_lum_gblur,int Isws_chr_gblur,int Isws_lum_sharpen,int Isws_chr_sharpen)
{
	int flags;
	SwsFilter *dstFilterParam, *srcFilterParam;
	swsGetFlagsAndFilterFromCmdLine(&flags, &srcFilterParam, &dstFilterParam,
                                        sws_flags,Isws_lum_gblur,Isws_chr_gblur,Isws_lum_sharpen,Isws_chr_sharpen);

	return getSwsContext(srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, srcFilterParam, dstFilterParam);
}

static double getSplineCoeff(double a, double b, double c, double d, double dist)
{
//	printf("%f %f %f %f %f\n", a,b,c,d,dist);
	if(dist<=1.0) 	return ((d*dist + c)*dist + b)*dist +a;
	else		return getSplineCoeff(	0.0, 
						 b+ 2.0*c + 3.0*d,
						        c + 3.0*d,
						-b- 3.0*c - 6.0*d,
						dist-1.0);
}

static inline void initFilter(int16_t **outFilter, int16_t **filterPos, int *outFilterSize, int xInc,
                              int srcW, int dstW, int filterAlign, int one, int flags,
                              SwsVector *srcFilter, SwsVector *dstFilter)
{
        int i;
        int filterSize;
        int filter2Size;
        int minFilterSize;
        double *filter=NULL;
        double *filter2=NULL;
#ifdef ARCH_X86
        if(gCpuCaps.hasMMX)
                asm volatile("emms\n\t"::: "memory"); //FIXME this shouldnt be required but it IS (even for non mmx versions)
#endif

        // Note the +1 is for the MMXscaler which reads over the end
        *filterPos = (int16_t*)memalign(8, (dstW+1)*sizeof(int16_t));

        if(ABS(xInc - 0x10000) <10) // unscaled
        {
                int i;
                filterSize= 1;
                filter= (double*)memalign(8, dstW*sizeof(double)*filterSize);
                for(i=0; i<dstW*filterSize; i++) filter[i]=0;

                for(i=0; i<dstW; i++)
                {
                        filter[i*filterSize]=1;
                        (*filterPos)[i]=i;
                }

        }
        else if(flags&SWS_POINT) // lame looking point sampling mode
        {
                int i;
                int xDstInSrc;
                filterSize= 1;
                filter= (double*)memalign(8, dstW*sizeof(double)*filterSize);
                
                xDstInSrc= xInc/2 - 0x8000;
                for(i=0; i<dstW; i++)
                {
                        int xx= (xDstInSrc - ((filterSize-1)<<15) + (1<<15))>>16;

                        (*filterPos)[i]= xx;
                        filter[i]= 1.0;
                        xDstInSrc+= xInc;
                }
        }
	else if((xInc <= (1<<16) && (flags&SWS_AREA)) || (flags&SWS_FAST_BILINEAR)) // bilinear upscale
        {
                int i;
                int xDstInSrc;
                if     (flags&SWS_BICUBIC) filterSize= 4;
                else if(flags&SWS_X      ) filterSize= 4;
                else                       filterSize= 2; // SWS_BILINEAR / SWS_AREA 
                filter= (double*)memalign(8, dstW*sizeof(double)*filterSize);

                xDstInSrc= xInc/2 - 0x8000;
                for(i=0; i<dstW; i++)
                {
                        int xx= (xDstInSrc - ((filterSize-1)<<15) + (1<<15))>>16;
                        int j;

                        (*filterPos)[i]= xx;
                                //Bilinear upscale / linear interpolate / Area averaging
                                for(j=0; j<filterSize; j++)
                                {
                                        double d= ABS((xx<<16) - xDstInSrc)/(double)(1<<16);
                                        double coeff= 1.0 - d;
                                        if(coeff<0) coeff=0;
                                        filter[i*filterSize + j]= coeff;
                                        xx++;
                                }
                        xDstInSrc+= xInc;
                }
        }
	else
        {
		double xDstInSrc;
		double sizeFactor, filterSizeInSrc;
		const double xInc1= (double)xInc / (double)(1<<16);
		int param= (flags&SWS_PARAM_MASK)>>SWS_PARAM_SHIFT;

		if     (flags&SWS_BICUBIC)	sizeFactor= 4.0;
		else if(flags&SWS_X)		sizeFactor= 8.0;
		else if(flags&SWS_AREA)		sizeFactor= 1.0; //downscale only, for upscale it is bilinear
		else if(flags&SWS_GAUSS)	sizeFactor= 8.0;   // infinite ;)
		else if(flags&SWS_LANCZOS)	sizeFactor= param ? 2.0*param : 6.0;
		else if(flags&SWS_SINC)		sizeFactor= 100.0; // infinite ;)
		else if(flags&SWS_SPLINE)	sizeFactor= 20.0;  // infinite ;)
		else if(flags&SWS_BILINEAR)	sizeFactor= 2.0;
		else ASSERT(0)
		
		if(xInc1 <= 1.0)	filterSizeInSrc= sizeFactor; // upscale
		else			filterSizeInSrc= sizeFactor*srcW / (double)dstW;

		filterSize= (int)ceil(1 + filterSizeInSrc); // will be reduced later if possible
		if(filterSize > srcW-2) filterSize=srcW-2;

		filter= (double*)memalign(16, dstW*sizeof(double)*filterSize);

		xDstInSrc= xInc1 / 2.0 - 0.5;
                for(i=0; i<dstW; i++)
                {
			int xx= (int)(xDstInSrc - (filterSize-1)*0.5 + 0.5);
                        int j;
                        (*filterPos)[i]= xx;
                        for(j=0; j<filterSize; j++)
                        {
				double d= ABS(xx - xDstInSrc)/filterSizeInSrc*sizeFactor;
                                double coeff;
				if(flags & SWS_BICUBIC)
                                {
					double A= param ? -param*0.01 : -0.60;
					
                                        // Equation is from VirtualDub
                                        if(d<1.0)
                                                coeff = (1.0 - (A+3.0)*d*d + (A+2.0)*d*d*d);
                                        else if(d<2.0)
                                                coeff = (-4.0*A + 8.0*A*d - 5.0*A*d*d + A*d*d*d);
                                        else
                                                coeff=0.0;
                                }
/*				else if(flags & SWS_X)
				{
					double p= param ? param*0.01 : 0.3;
					coeff = d ? sin(d*PI)/(d*PI) : 1.0;
					coeff*= pow(2.0, - p*d*d);
				}*/
				else if(flags & SWS_X)
				{
					double A= param ? param*0.1 : 1.0;
					
					if(d<1.0)
						coeff = cos(d*PI);
					else
						coeff=-1.0;
					if(coeff<0.0) 	coeff= -pow(-coeff, A);
					else		coeff=  pow( coeff, A);
					coeff= coeff*0.5 + 0.5;
				}
                                else if(flags & SWS_AREA)
                                {
					double srcPixelSize= 1.0/xInc1;
                                        if(d + srcPixelSize/2 < 0.5) coeff= 1.0;
                                        else if(d - srcPixelSize/2 < 0.5) coeff= (0.5-d)/srcPixelSize + 0.5;
                                        else coeff=0.0;
                                }
				else if(flags & SWS_GAUSS)
				{
					double p= param ? param*0.1 : 3.0;
					coeff = pow(2.0, - p*d*d);
				}
				else if(flags & SWS_SINC)
				{
					coeff = d ? sin(d*PI)/(d*PI) : 1.0;
				}
				else if(flags & SWS_LANCZOS)
				{
					double p= param ? param : 3.0; 
					coeff = d ? sin(d*PI)*sin(d*PI/p)/(d*d*PI*PI/p) : 1.0;
					if(d>p) coeff=0;
				}
				else if(flags & SWS_BILINEAR)
                                {
                                        coeff= 1.0 - d;
                                        if(coeff<0) coeff=0;
                                }
				else if(flags & SWS_SPLINE)
				{
					double p=-2.196152422706632;
					coeff = getSplineCoeff(1.0, 0.0, p, -p-1.0, d);
				}
				else ASSERT(0)

                                filter[i*filterSize + j]= coeff;
                                xx++;
                        }
			xDstInSrc+= xInc1;
                }
        }

        /* apply src & dst Filter to filter -> filter2
           xvid_free(filter);
        */
        ASSERT(filterSize>0)
        filter2Size= filterSize;
        if(srcFilter) filter2Size+= srcFilter->length - 1;
        if(dstFilter) filter2Size+= dstFilter->length - 1;
        ASSERT(filter2Size>0)
        filter2= (double*)memalign(8, filter2Size*dstW*sizeof(double));

        for(i=0; i<dstW; i++)
        {
                int j;
                SwsVector scaleFilter;
                SwsVector *outVec;

                scaleFilter.coeff= filter + i*filterSize;
                scaleFilter.length= filterSize;

                if(srcFilter) outVec= getConvVec(srcFilter, &scaleFilter);
                else          outVec= &scaleFilter;

                ASSERT(outVec->length == filter2Size)
                //FIXME dstFilter

                for(j=0; j<outVec->length; j++)
                {
                        filter2[i*filter2Size + j]= outVec->coeff[j];
                }

                (*filterPos)[i]+= (filterSize-1)/2 - (filter2Size-1)/2;

                if(outVec != &scaleFilter) freeVec(outVec);
        }
        xvid_free(filter); filter=NULL;

        /* try to reduce the filter-size (step1 find size and shift left) */
        // Assume its near normalized (*0.5 or *2.0 is ok but * 0.001 is not)
        minFilterSize= 0;
        for(i=dstW-1; i>=0; i--)
        {
                int min= filter2Size;
                int j;
                double cutOff=0.0;

                /* get rid off near zero elements on the left by shifting left */
                for(j=0; j<filter2Size; j++)
                {
                        int k;
                        cutOff += ABS(filter2[i*filter2Size]);

                        if(cutOff > SWS_MAX_REDUCE_CUTOFF) break;

                        /* preserve Monotonicity because the core cant handle the filter otherwise */
                        if(i<dstW-1 && (*filterPos)[i] >= (*filterPos)[i+1]) break;

                        // Move filter coeffs left
                        for(k=1; k<filter2Size; k++)
                                filter2[i*filter2Size + k - 1]= filter2[i*filter2Size + k];
                        filter2[i*filter2Size + k - 1]= 0.0;
                        (*filterPos)[i]++;
                }

                cutOff=0.0;
                /* count near zeros on the right */
                for(j=filter2Size-1; j>0; j--)
                {
                        cutOff += ABS(filter2[i*filter2Size + j]);

                        if(cutOff > SWS_MAX_REDUCE_CUTOFF) break;
                        min--;
                }

                if(min>minFilterSize) minFilterSize= min;
        }

        ASSERT(minFilterSize > 0)
        filterSize= (minFilterSize +(filterAlign-1)) & (~(filterAlign-1));
        ASSERT(filterSize > 0)
        filter= (double*)memalign(8, filterSize*dstW*sizeof(double));
        *outFilterSize= filterSize;

        if(flags&SWS_PRINT_INFO)
		MSG_INFO("SwScaler: reducing / aligning filtersize %d -> %d\n", filter2Size, filterSize);
        /* try to reduce the filter-size (step2 reduce it) */
        for(i=0; i<dstW; i++)
        {
                int j;

                for(j=0; j<filterSize; j++)
                {
                        if(j>=filter2Size) filter[i*filterSize + j]= 0.0;
                        else               filter[i*filterSize + j]= filter2[i*filter2Size + j];
                }
        }
        xvid_free(filter2); filter2=NULL;
        

        //FIXME try to align filterpos if possible

        //fix borders
        for(i=0; i<dstW; i++)
        {
                int j;
                if((*filterPos)[i] < 0)
                {
                        // Move filter coeffs left to compensate for filterPos
                        for(j=1; j<filterSize; j++)
                        {
                                int left= MAX(j + (*filterPos)[i], 0);
                                filter[i*filterSize + left] += filter[i*filterSize + j];
                                filter[i*filterSize + j]=0;
                        }
                        (*filterPos)[i]= 0;
                }

                if((*filterPos)[i] + filterSize > srcW)
                {
                        int shift= (*filterPos)[i] + filterSize - srcW;
                        // Move filter coeffs right to compensate for filterPos
                        for(j=filterSize-2; j>=0; j--)
                        {
                                int right= MIN(j + shift, filterSize-1);
                                filter[i*filterSize +right] += filter[i*filterSize +j];
                                filter[i*filterSize +j]=0;
                        }
                        (*filterPos)[i]= srcW - filterSize;
                }
        }

        // Note the +1 is for the MMXscaler which reads over the end
        *outFilter= (int16_t*)memalign(8, *outFilterSize*(dstW+1)*sizeof(int16_t));
        memset(*outFilter, 0, *outFilterSize*(dstW+1)*sizeof(int16_t));

        /* Normalize & Store in outFilter */
        for(i=0; i<dstW; i++)
        {
                int j;
                double sum=0;
                double scale= one;
                for(j=0; j<filterSize; j++)
                {
                        sum+= filter[i*filterSize + j];
                }
                scale/= sum;
                for(j=0; j<filterSize; j++)
                {
                        (*outFilter)[i*(*outFilterSize) + j]= (int)(filter[i*filterSize + j]*scale);
                }
        }
        
        (*filterPos)[dstW]= (*filterPos)[dstW-1]; // the MMX scaler will read over the end
        for(i=0; i<*outFilterSize; i++)
        {
                int j= dstW*(*outFilterSize);
                (*outFilter)[j + i]= (*outFilter)[j + i - (*outFilterSize)];
        }

        xvid_free(filter);
}

#ifdef ARCH_X86
static void initMMX2HScaler(int dstW, int xInc, uint8_t *funnyCode, int16_t *filter, int32_t *filterPos, int numSplits)
{
        uint8_t *fragmentA;
        int imm8OfPShufW1A;
        int imm8OfPShufW2A;
        int fragmentLengthA;
        uint8_t *fragmentB;
        int imm8OfPShufW1B;
        int imm8OfPShufW2B;
        int fragmentLengthB;
        int fragmentPos;

        int xpos, i;

        // create an optimized horizontal scaling routine

        //code fragment

        asm volatile(
                "jmp 9f                         \n\t"
        // Begin
                "0:                             \n\t"
                "movq (%%edx, %%eax), %%mm3     \n\t" 
                "movd (%%ecx, %%esi), %%mm0     \n\t" 
                "movd 1(%%ecx, %%esi), %%mm1    \n\t"
                "punpcklbw %%mm7, %%mm1         \n\t"
                "punpcklbw %%mm7, %%mm0         \n\t"
                "pshufw $0xFF, %%mm1, %%mm1     \n\t"
                "1:                             \n\t"
                "pshufw $0xFF, %%mm0, %%mm0     \n\t"
                "2:                             \n\t"
                "psubw %%mm1, %%mm0             \n\t"
                "movl 8(%%ebx, %%eax), %%esi    \n\t"
                "pmullw %%mm3, %%mm0            \n\t"
                "psllw $7, %%mm1                \n\t"
                "paddw %%mm1, %%mm0             \n\t"

                "movq %%mm0, (%%edi, %%eax)     \n\t"

                "addl $8, %%eax                 \n\t"
        // End
                "9:                             \n\t"
//              "int $3\n\t"
                "leal 0b, %0                    \n\t"
                "leal 1b, %1                    \n\t"
                "leal 2b, %2                    \n\t"
                "decl %1                        \n\t"
                "decl %2                        \n\t"
                "subl %0, %1                    \n\t"
                "subl %0, %2                    \n\t"
                "leal 9b, %3                    \n\t"
                "subl %0, %3                    \n\t"


                :"=r" (fragmentA), "=r" (imm8OfPShufW1A), "=r" (imm8OfPShufW2A),
                "=r" (fragmentLengthA)
        );

        asm volatile(
                "jmp 9f                         \n\t"
        // Begin
                "0:                             \n\t"
                "movq (%%edx, %%eax), %%mm3     \n\t" 
                "movd (%%ecx, %%esi), %%mm0     \n\t" 
                "punpcklbw %%mm7, %%mm0         \n\t"
                "pshufw $0xFF, %%mm0, %%mm1     \n\t"
                "1:                             \n\t"
                "pshufw $0xFF, %%mm0, %%mm0     \n\t"
                "2:                             \n\t"
                "psubw %%mm1, %%mm0             \n\t"
                "movl 8(%%ebx, %%eax), %%esi    \n\t"
                "pmullw %%mm3, %%mm0            \n\t"
                "psllw $7, %%mm1                \n\t"
                "paddw %%mm1, %%mm0             \n\t"

                "movq %%mm0, (%%edi, %%eax)     \n\t"

                "addl $8, %%eax                 \n\t"
        // End
                "9:                             \n\t"
//              "int $3\n\t"
                "leal 0b, %0                    \n\t"
                "leal 1b, %1                    \n\t"
                "leal 2b, %2                    \n\t"
                "decl %1                        \n\t"
                "decl %2                        \n\t"
                "subl %0, %1                    \n\t"
                "subl %0, %2                    \n\t"
                "leal 9b, %3                    \n\t"
                "subl %0, %3                    \n\t"


                :"=r" (fragmentB), "=r" (imm8OfPShufW1B), "=r" (imm8OfPShufW2B),
                "=r" (fragmentLengthB)
        );

        xpos= 0; //lumXInc/2 - 0x8000; // difference between pixel centers
        fragmentPos=0;
        
        for(i=0; i<dstW/numSplits; i++)
        {
                int xx=xpos>>16;

                if((i&3) == 0)
                {
                        int a=0;
                        int b=((xpos+xInc)>>16) - xx;
                        int c=((xpos+xInc*2)>>16) - xx;
                        int d=((xpos+xInc*3)>>16) - xx;

                        filter[i  ] = (( xpos         & 0xFFFF) ^ 0xFFFF)>>9;
                        filter[i+1] = (((xpos+xInc  ) & 0xFFFF) ^ 0xFFFF)>>9;
                        filter[i+2] = (((xpos+xInc*2) & 0xFFFF) ^ 0xFFFF)>>9;
                        filter[i+3] = (((xpos+xInc*3) & 0xFFFF) ^ 0xFFFF)>>9;
                        filterPos[i/2]= xx;

                        if(d+1<4)
                        {
                                int maxShift= 3-(d+1);
                                int shift=0;

                                memcpy(funnyCode + fragmentPos, fragmentB, fragmentLengthB);

                                funnyCode[fragmentPos + imm8OfPShufW1B]=
                                        (a+1) | ((b+1)<<2) | ((c+1)<<4) | ((d+1)<<6);
                                funnyCode[fragmentPos + imm8OfPShufW2B]=
                                        a | (b<<2) | (c<<4) | (d<<6);

                                if(i+3>=dstW) shift=maxShift; //avoid overread
                                else if((filterPos[i/2]&3) <= maxShift) shift=filterPos[i/2]&3; //Align

                                if(shift && i>=shift)
                                {
                                        funnyCode[fragmentPos + imm8OfPShufW1B]+= 0x55*shift;
                                        funnyCode[fragmentPos + imm8OfPShufW2B]+= 0x55*shift;
                                        filterPos[i/2]-=shift;
                                }

                                fragmentPos+= fragmentLengthB;
                        }
                        else
                        {
                                int maxShift= 3-d;
                                int shift=0;

                                memcpy(funnyCode + fragmentPos, fragmentA, fragmentLengthA);

                                funnyCode[fragmentPos + imm8OfPShufW1A]=
                                funnyCode[fragmentPos + imm8OfPShufW2A]=
                                        a | (b<<2) | (c<<4) | (d<<6);

                                if(i+4>=dstW) shift=maxShift; //avoid overread
                                else if((filterPos[i/2]&3) <= maxShift) shift=filterPos[i/2]&3; //partial align

                                if(shift && i>=shift)
                                {
                                        funnyCode[fragmentPos + imm8OfPShufW1A]+= 0x55*shift;
                                        funnyCode[fragmentPos + imm8OfPShufW2A]+= 0x55*shift;
                                        filterPos[i/2]-=shift;
                                }

                                fragmentPos+= fragmentLengthA;
                        }

                        funnyCode[fragmentPos]= RET;
                }
                xpos+=xInc;
        }
        filterPos[i/2]= xpos>>16; // needed to jump to the next part
}
#endif // ARCH_X86

//FIXME remove
void SwScale_Init(){
}

static void globalInit(){
    // generating tables:
    int i;
    for(i=0; i<768; i++){
        int c= MIN(MAX(i-256, 0), 255);
        clip_table[i]=c;
    }

cpuCaps= gCpuCaps;

#ifdef RUNTIME_CPUDETECT
#ifdef CAN_COMPILE_X86_ASM
        // ordered per speed fasterst first
        if(gCpuCaps.hasMMX2)
                swScale= swScale_MMX2;
        else if(gCpuCaps.has3DNow)
                swScale= swScale_3DNow;
        else if(gCpuCaps.hasMMX)
                swScale= swScale_MMX;
        else
                swScale= swScale_C;

#else
        swScale= swScale_C;
        cpuCaps.hasMMX2 = cpuCaps.hasMMX = cpuCaps.has3DNow = 0;
#endif
#else //RUNTIME_CPUDETECT
#ifdef HAVE_MMX2
        swScale= swScale_MMX2;
        cpuCaps.has3DNow = 0;
#elif defined (HAVE_3DNOW)
        swScale= swScale_3DNow;
        cpuCaps.hasMMX2 = 0;
#elif defined (HAVE_MMX)
        swScale= swScale_MMX;
        cpuCaps.hasMMX2 = cpuCaps.has3DNow = 0;
#else
        swScale= swScale_C;
        cpuCaps.hasMMX2 = cpuCaps.hasMMX = cpuCaps.has3DNow = 0;
#endif
#endif //!RUNTIME_CPUDETECT
}

/**
 * bring pointers in YUV order instead of YVU
 */
static inline void orderYUV(int format, uint8_t * sortedP[], int sortedStride[], uint8_t * p[], int stride[]){
	if(format == IMGFMT_YV12){
		sortedP[0]= p[0];
		sortedP[1]= p[1];
		sortedP[2]= p[2];
		sortedStride[0]= stride[0];
		sortedStride[1]= stride[1];
		sortedStride[2]= stride[2];
	}
}

/* unscaled copy like stuff (assumes nearly identical formats) */
static void simpleCopy(SwsContext *c, uint8_t* srcParam[], int srcStrideParam[], int srcSliceY,
             int srcSliceH, uint8_t* dstParam[], int dstStrideParam[]){

        int srcStride[3];
	int dstStride[3];
        uint8_t *src[3];
        uint8_t *dst[3];

	orderYUV(c->srcFormat, src, srcStride, srcParam, srcStrideParam);
	orderYUV(c->dstFormat, dst, dstStride, dstParam, dstStrideParam);

        if(isPacked(c->srcFormat))
        {
                if(dstStride[0]==srcStride[0])
                        memcpy(dst[0] + dstStride[0]*srcSliceY, src[0], srcSliceH*dstStride[0]);
                else
                {
                        int i;
                        uint8_t *srcPtr= src[0];
                        uint8_t *dstPtr= dst[0] + dstStride[0]*srcSliceY;
                        int length=0;

                        /* universal length finder */
                        while(length+c->srcW <= ABS(dstStride[0]) 
                           && length+c->srcW <= ABS(srcStride[0])) length+= c->srcW;
                        ASSERT(length!=0);

                        for(i=0; i<srcSliceH; i++)
                        {
                                memcpy(dstPtr, srcPtr, length);
                                srcPtr+= srcStride[0];
                                dstPtr+= dstStride[0];
                        }
                }
        }
        else 
	{ /* Planar YUV or gray */
                int plane;
                for(plane=0; plane<3; plane++)
                {
			int length= plane==0 ? c->srcW  : -((-c->srcW  )>>c->chrDstHSubSample);
			int y=      plane==0 ? srcSliceY: -((-srcSliceY)>>c->chrDstVSubSample);
			int height= plane==0 ? srcSliceH: -((-srcSliceH)>>c->chrDstVSubSample);

			if((isGray(c->srcFormat) || isGray(c->dstFormat)) && plane>0)
			{
				if(!isGray(c->dstFormat))
					memset(dst[plane], 128, dstStride[plane]*height);
			}
			else
			{
                        if(dstStride[plane]==srcStride[plane])
                                memcpy(dst[plane] + dstStride[plane]*y, src[plane], height*dstStride[plane]);
                        else
                        {
                                int i;
                                uint8_t *srcPtr= src[plane];
                                uint8_t *dstPtr= dst[plane] + dstStride[plane]*y;
                                for(i=0; i<height; i++)
                                {
                                        memcpy(dstPtr, srcPtr, length);
                                        srcPtr+= srcStride[plane];
                                        dstPtr+= dstStride[plane];
                                }
                        }
                }
        }
}
}

static int remove_dup_fourcc(int fourcc)
{
 return fourcc;
}

static void getSubSampleFactors(int *h, int *v, int format){
	switch(format){
	case IMGFMT_YV12://FIXME remove after different subsamplings are fully implemented
		*h=1;
		*v=1;
		break;
	default:
		*h=0;
		*v=0;
		break;
	}
}

SwsContext *getSwsContext(int srcW, int srcH, int srcFormat, int dstW, int dstH, int dstFormat, int flags,
                         SwsFilter *srcFilter, SwsFilter *dstFilter){

        SwsContext *c;
        int i;
        int usesFilter;
	int unscaled, needsDither;
        SwsFilter dummyFilter= {NULL, NULL, NULL, NULL};
#ifdef ARCH_X86
        if(gCpuCaps.hasMMX)
                asm volatile("emms\n\t"::: "memory");
#endif
        if(swScale==NULL) globalInit();
//srcFormat= IMGFMT_Y800;
//dstFormat= IMGFMT_Y800;
	/* avoid dupplicate Formats, so we dont need to check to much */
	srcFormat = remove_dup_fourcc(srcFormat);
	dstFormat = remove_dup_fourcc(dstFormat);

	unscaled = (srcW == dstW && srcH == dstH);
	needsDither= 0;

        if(!isSupportedIn(srcFormat)) 
        {
		MSG_ERR("swScaler: %s is not supported as input format\n", vo_format_name(srcFormat));
                return NULL;
        }
        if(!isSupportedOut(dstFormat))
        {
		MSG_ERR("swScaler: %s is not supported as output format\n", vo_format_name(dstFormat));
                return NULL;
        }

        /* sanity check */
        if(srcW<4 || srcH<1 || dstW<8 || dstH<1) //FIXME check if these are enough and try to lowwer them after fixing the relevant parts of the code
        {
		 MSG_ERR("swScaler: %dx%d -> %dx%d is invalid scaling dimension\n", 
                        srcW, srcH, dstW, dstH);
                return NULL;
        }

        if(!dstFilter) dstFilter= &dummyFilter;
        if(!srcFilter) srcFilter= &dummyFilter;

        c= memalign(8, sizeof(SwsContext));
        memset(c, 0, sizeof(SwsContext));

        c->srcW= srcW;
        c->srcH= srcH;
        c->dstW= dstW;
        c->dstH= dstH;
        c->lumXInc= ((srcW<<16) + (dstW>>1))/dstW;
        c->lumYInc= ((srcH<<16) + (dstH>>1))/dstH;
        c->flags= flags;
        c->dstFormat= dstFormat;
        c->srcFormat= srcFormat;

        usesFilter=0;
        if(dstFilter->lumV!=NULL && dstFilter->lumV->length>1) usesFilter=1;
        if(dstFilter->lumH!=NULL && dstFilter->lumH->length>1) usesFilter=1;
        if(dstFilter->chrV!=NULL && dstFilter->chrV->length>1) usesFilter=1;
        if(dstFilter->chrH!=NULL && dstFilter->chrH->length>1) usesFilter=1;
        if(srcFilter->lumV!=NULL && srcFilter->lumV->length>1) usesFilter=1;
        if(srcFilter->lumH!=NULL && srcFilter->lumH->length>1) usesFilter=1;
        if(srcFilter->chrV!=NULL && srcFilter->chrV->length>1) usesFilter=1;
        if(srcFilter->chrH!=NULL && srcFilter->chrH->length>1) usesFilter=1;
        
	getSubSampleFactors(&c->chrSrcHSubSample, &c->chrSrcVSubSample, srcFormat);
	getSubSampleFactors(&c->chrDstHSubSample, &c->chrDstVSubSample, dstFormat);

	// reuse chroma for 2 pixles rgb/bgr unless user wants full chroma interpolation
	//if((isBGR(dstFormat) || isRGB(dstFormat)) && !(flags&SWS_FULL_CHR_H_INT)) c->chrDstHSubSample=1;

	// drop some chroma lines if the user wants it
	c->vChrDrop= (flags&SWS_SRC_V_CHR_DROP_MASK)>>SWS_SRC_V_CHR_DROP_SHIFT;
	c->chrSrcVSubSample+= c->vChrDrop;

	// drop every 2. pixel for chroma calculation unless user wants full chroma
	//if((isBGR(srcFormat) || isRGB(srcFormat)) && !(flags&SWS_FULL_CHR_H_INP)) 
	//	c->chrSrcHSubSample=1;

	c->chrIntHSubSample= c->chrDstHSubSample;
	c->chrIntVSubSample= c->chrSrcVSubSample;
	
	// note the -((-x)>>y) is so that we allways round toward +inf
	c->chrSrcW= -((-srcW) >> c->chrSrcHSubSample);
	c->chrSrcH= -((-srcH) >> c->chrSrcVSubSample);
	c->chrDstW= -((-dstW) >> c->chrDstHSubSample);
	c->chrDstH= -((-dstH) >> c->chrDstVSubSample);
	
	//if(isBGR(dstFormat))
	//	c->yuvTable= yuv2rgb_c_init(dstFormat & 0xFF, MODE_RGB, c->table_rV, c->table_gU, c->table_gV, c->table_bU);
	//if(isRGB(dstFormat))
	//	c->yuvTable= yuv2rgb_c_init(dstFormat & 0xFF, MODE_BGR, c->table_rV, c->table_gU, c->table_gV, c->table_bU);

        /* unscaled special Cases */
	if(unscaled && !usesFilter)
	{
                /* simple copy */
		if(   srcFormat == dstFormat )
                {
                        c->swScale= simpleCopy;

                        if(flags&SWS_PRINT_INFO)
				MSG_INFO("SwScaler: using unscaled %s -> %s special converter\n", 
                                        vo_format_name(srcFormat), vo_format_name(dstFormat));
                        return c;
                }
                
		if(c->swScale){
                        if(flags&SWS_PRINT_INFO)
				MSG_INFO("SwScaler: using unscaled %s -> %s special converter\n", 
                                        vo_format_name(srcFormat), vo_format_name(dstFormat));
                        return c;
                }
        }

        if(cpuCaps.hasMMX2)
        {
                c->canMMX2BeUsed= (dstW >=srcW && (dstW&31)==0 && (srcW&15)==0) ? 1 : 0;
                if(!c->canMMX2BeUsed && dstW >=srcW && (srcW&15)==0 && (flags&SWS_FAST_BILINEAR))
                {
                        if(flags&SWS_PRINT_INFO)
				MSG_INFO("SwScaler: output Width is not a multiple of 32 -> no MMX2 scaler\n");
                }
        }
        else
                c->canMMX2BeUsed=0;

        c->chrXInc= ((c->chrSrcW<<16) + (c->chrDstW>>1))/c->chrDstW;
        c->chrYInc= ((c->chrSrcH<<16) + (c->chrDstH>>1))/c->chrDstH;

        // match pixel 0 of the src to pixel 0 of dst and match pixel n-2 of src to pixel n-2 of dst
        // but only for the FAST_BILINEAR mode otherwise do correct scaling
        // n-2 is the last chrominance sample available
        // this is not perfect, but noone shuld notice the difference, the more correct variant
        // would be like the vertical one, but that would require some special code for the
        // first and last pixel
        if(flags&SWS_FAST_BILINEAR)
        {
                if(c->canMMX2BeUsed)
                {
                        c->lumXInc+= 20;
                        c->chrXInc+= 20;
                }
                //we dont use the x86asm scaler if mmx is available
                else if(cpuCaps.hasMMX)
                {
                        c->lumXInc = ((srcW-2)<<16)/(dstW-2) - 20;
                        c->chrXInc = ((c->chrSrcW-2)<<16)/(c->chrDstW-2) - 20;
                }
        }

        /* precalculate horizontal scaler filter coefficients */
        {
                const int filterAlign= cpuCaps.hasMMX ? 4 : 1;

                initFilter(&c->hLumFilter, &c->hLumFilterPos, &c->hLumFilterSize, c->lumXInc,
				 srcW      ,       dstW, filterAlign, 1<<14,
				 (flags&SWS_BICUBLIN) ? (flags|SWS_BICUBIC)  : flags,
                                 srcFilter->lumH, dstFilter->lumH);
                initFilter(&c->hChrFilter, &c->hChrFilterPos, &c->hChrFilterSize, c->chrXInc,
				 c->chrSrcW, c->chrDstW, filterAlign, 1<<14,
				 (flags&SWS_BICUBLIN) ? (flags|SWS_BILINEAR) : flags,
                                 srcFilter->chrH, dstFilter->chrH);

#ifdef ARCH_X86
// cant downscale !!!
                if(c->canMMX2BeUsed && (flags & SWS_FAST_BILINEAR))
                {
                        c->lumMmx2Filter   = (int16_t*)memalign(8, (dstW        /8+8)*sizeof(int16_t));
                        c->chrMmx2Filter   = (int16_t*)memalign(8, (c->chrDstW  /4+8)*sizeof(int16_t));
                        c->lumMmx2FilterPos= (int32_t*)memalign(8, (dstW      /2/8+8)*sizeof(int32_t));
                        c->chrMmx2FilterPos= (int32_t*)memalign(8, (c->chrDstW/2/4+8)*sizeof(int32_t));

                        initMMX2HScaler(      dstW, c->lumXInc, c->funnyYCode , c->lumMmx2Filter, c->lumMmx2FilterPos, 8);
                        initMMX2HScaler(c->chrDstW, c->chrXInc, c->funnyUVCode, c->chrMmx2Filter, c->chrMmx2FilterPos, 4);
                }
#endif
        } // Init Horizontal stuff



        /* precalculate vertical scaler filter coefficients */
        initFilter(&c->vLumFilter, &c->vLumFilterPos, &c->vLumFilterSize, c->lumYInc,
			srcH      ,        dstH, 1, (1<<12)-4,
			(flags&SWS_BICUBLIN) ? (flags|SWS_BICUBIC)  : flags,
                        srcFilter->lumV, dstFilter->lumV);
        initFilter(&c->vChrFilter, &c->vChrFilterPos, &c->vChrFilterSize, c->chrYInc,
			c->chrSrcH, c->chrDstH, 1, (1<<12)-4,
			(flags&SWS_BICUBLIN) ? (flags|SWS_BILINEAR) : flags,
                         srcFilter->chrV, dstFilter->chrV);

        // Calculate Buffer Sizes so that they wont run out while handling these damn slices
        c->vLumBufSize= c->vLumFilterSize;
        c->vChrBufSize= c->vChrFilterSize;
        for(i=0; i<dstH; i++)
        {
                int chrI= i*c->chrDstH / dstH;
                int nextSlice= MAX(c->vLumFilterPos[i   ] + c->vLumFilterSize - 1,
				 ((c->vChrFilterPos[chrI] + c->vChrFilterSize - 1)<<c->chrSrcVSubSample));
		nextSlice&= ~3; // Slices start at boundaries which are divisable through 4
                if(c->vLumFilterPos[i   ] + c->vLumBufSize < nextSlice)
                        c->vLumBufSize= nextSlice - c->vLumFilterPos[i   ];
		if(c->vChrFilterPos[chrI] + c->vChrBufSize < (nextSlice>>c->chrSrcVSubSample))
			c->vChrBufSize= (nextSlice>>c->chrSrcVSubSample) - c->vChrFilterPos[chrI];
        }

        // allocate pixbufs (we use dynamic allocation because otherwise we would need to
        c->lumPixBuf= (int16_t**)memalign(4, c->vLumBufSize*2*sizeof(int16_t*));
        c->chrPixBuf= (int16_t**)memalign(4, c->vChrBufSize*2*sizeof(int16_t*));
        //Note we need at least one pixel more at the end because of the mmx code (just in case someone wanna replace the 4000/8000)
        for(i=0; i<c->vLumBufSize; i++)
                c->lumPixBuf[i]= c->lumPixBuf[i+c->vLumBufSize]= (uint16_t*)memalign(8, 4000);
        for(i=0; i<c->vChrBufSize; i++)
                c->chrPixBuf[i]= c->chrPixBuf[i+c->vChrBufSize]= (uint16_t*)memalign(8, 8000);

        //try to avoid drawing green stuff between the right end and the stride end
        for(i=0; i<c->vLumBufSize; i++) memset(c->lumPixBuf[i], 0, 4000);
        for(i=0; i<c->vChrBufSize; i++) memset(c->chrPixBuf[i], 64, 8000);

        ASSERT(c->chrDstH <= dstH)

        // pack filter data for mmx code
        if(cpuCaps.hasMMX)
        {
                c->lumMmxFilter= (int16_t*)memalign(8, c->vLumFilterSize*      dstH*4*sizeof(int16_t));
                c->chrMmxFilter= (int16_t*)memalign(8, c->vChrFilterSize*c->chrDstH*4*sizeof(int16_t));
                for(i=0; i<c->vLumFilterSize*dstH; i++)
                        c->lumMmxFilter[4*i]=c->lumMmxFilter[4*i+1]=c->lumMmxFilter[4*i+2]=c->lumMmxFilter[4*i+3]=
                                c->vLumFilter[i];
                for(i=0; i<c->vChrFilterSize*c->chrDstH; i++)
                        c->chrMmxFilter[4*i]=c->chrMmxFilter[4*i+1]=c->chrMmxFilter[4*i+2]=c->chrMmxFilter[4*i+3]=
                                c->vChrFilter[i];
        }

        if(flags&SWS_PRINT_INFO)
        {
#ifdef DITHER1XBPP
                char *dither= " dithered";
#else
                char *dither= "";
#endif
                if(flags&SWS_FAST_BILINEAR)
			MSG_INFO("\nSwScaler: FAST_BILINEAR scaler, ");
                else if(flags&SWS_BILINEAR)
			MSG_INFO("\nSwScaler: BILINEAR scaler, ");
                else if(flags&SWS_BICUBIC)
			MSG_INFO("\nSwScaler: BICUBIC scaler, ");
                else if(flags&SWS_X)
			MSG_INFO("\nSwScaler: Experimental scaler, ");
                else if(flags&SWS_POINT)
			MSG_INFO("\nSwScaler: Nearest Neighbor / POINT scaler, ");
                else if(flags&SWS_AREA)
			MSG_INFO("\nSwScaler: Area Averageing scaler, ");
		else if(flags&SWS_BICUBLIN)
			MSG_INFO("\nSwScaler: luma BICUBIC / chroma BILINEAR scaler, ");
		else if(flags&SWS_GAUSS)
			MSG_INFO("\nSwScaler: Gaussian scaler, ");
		else if(flags&SWS_SINC)
			MSG_INFO("\nSwScaler: Sinc scaler, ");
		else if(flags&SWS_LANCZOS)
			MSG_INFO("\nSwScaler: Lanczos scaler, ");
		else if(flags&SWS_SPLINE)
			MSG_INFO("\nSwScaler: Bicubic spline scaler, ");
                else
			MSG_INFO("\nSwScaler: ehh flags invalid?! ");

		if(0)
			MSG_INFO("from %s to%s %s ", 
				vo_format_name(srcFormat), dither, vo_format_name(dstFormat));
		else
			MSG_INFO("from %s to %s ", 
				vo_format_name(srcFormat), vo_format_name(dstFormat));

                if(cpuCaps.hasMMX2)
			MSG_INFO("using MMX2\n");
                else if(cpuCaps.has3DNow)
			MSG_INFO("using 3DNOW\n");
                else if(cpuCaps.hasMMX)
			MSG_INFO("using MMX\n");
                else
			MSG_INFO("using C\n");
        }

        if((flags & SWS_PRINT_INFO) && verbose)
        {
                if(cpuCaps.hasMMX)
                {
                        if(c->canMMX2BeUsed && (flags&SWS_FAST_BILINEAR))
				MSG_V("SwScaler: using FAST_BILINEAR MMX2 scaler for horizontal scaling\n");
                        else
                        {
                                if(c->hLumFilterSize==4)
					MSG_V("SwScaler: using 4-tap MMX scaler for horizontal luminance scaling\n");
                                else if(c->hLumFilterSize==8)
					MSG_V("SwScaler: using 8-tap MMX scaler for horizontal luminance scaling\n");
                                else
					MSG_V("SwScaler: using n-tap MMX scaler for horizontal luminance scaling\n");

                                if(c->hChrFilterSize==4)
					MSG_V("SwScaler: using 4-tap MMX scaler for horizontal chrominance scaling\n");
                                else if(c->hChrFilterSize==8)
					MSG_V("SwScaler: using 8-tap MMX scaler for horizontal chrominance scaling\n");
                                else
					MSG_V("SwScaler: using n-tap MMX scaler for horizontal chrominance scaling\n");
                        }
                }
                else
                {
#ifdef ARCH_X86
			MSG_V("SwScaler: using X86-Asm scaler for horizontal scaling\n");
#else
                        if(flags & SWS_FAST_BILINEAR)
				MSG_V("SwScaler: using FAST_BILINEAR C scaler for horizontal scaling\n");
                        else
				MSG_V("SwScaler: using C scaler for horizontal scaling\n");
#endif
                }
                if(isPlanarYUV(dstFormat))
                {
                        if(c->vLumFilterSize==1)
				MSG_V("SwScaler: using 1-tap %s \"scaler\" for vertical scaling (YV12 like)\n", cpuCaps.hasMMX ? "MMX" : "C");
                        else
				MSG_V("SwScaler: using n-tap %s scaler for vertical scaling (YV12 like)\n", cpuCaps.hasMMX ? "MMX" : "C");
                }
                else
                {
                        if(c->vLumFilterSize==1 && c->vChrFilterSize==2)
				MSG_V("SwScaler: using 1-tap %s \"scaler\" for vertical luminance scaling (BGR)\n"
                                       "SwScaler:       2-tap scaler for vertical chrominance scaling (BGR)\n",cpuCaps.hasMMX ? "MMX" : "C");
                        else if(c->vLumFilterSize==2 && c->vChrFilterSize==2)
				MSG_V("SwScaler: using 2-tap linear %s scaler for vertical scaling (BGR)\n", cpuCaps.hasMMX ? "MMX" : "C");
                        else
				MSG_V("SwScaler: using n-tap %s scaler for vertical scaling (BGR)\n", cpuCaps.hasMMX ? "MMX" : "C");
                }

		MSG_V("SwScaler: %dx%d -> %dx%d\n", srcW, srcH, dstW, dstH);
        }
        if((flags & SWS_PRINT_INFO) && verbose>1)
        {
		MSG_DBG2("SwScaler:Lum srcW=%d srcH=%d dstW=%d dstH=%d xInc=%d yInc=%d\n",
                        c->srcW, c->srcH, c->dstW, c->dstH, c->lumXInc, c->lumYInc);
		MSG_DBG2("SwScaler:Chr srcW=%d srcH=%d dstW=%d dstH=%d xInc=%d yInc=%d\n",
                        c->chrSrcW, c->chrSrcH, c->chrDstW, c->chrDstH, c->chrXInc, c->chrYInc);
        }

        c->swScale= swScale;
        return c;
}

/**
 * returns a normalized gaussian curve used to filter stuff
 * quality=3 is high quality, lowwer is lowwer quality
 */

SwsVector *getGaussianVec(double variance, double quality){
        const int length= (int)(variance*quality + 0.5) | 1;
        int i;
        double *coeff= memalign(sizeof(double), length*sizeof(double));
        double middle= (length-1)*0.5;
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= length;

        for(i=0; i<length; i++)
        {
                double dist= i-middle;
                coeff[i]= exp( -dist*dist/(2*variance*variance) ) / sqrt(2*variance*PI);
        }

        normalizeVec(vec, 1.0);

        return vec;
}

SwsVector *getConstVec(double c, int length){
        int i;
        double *coeff= memalign(sizeof(double), length*sizeof(double));
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= length;

        for(i=0; i<length; i++)
                coeff[i]= c;

        return vec;
}


SwsVector *getIdentityVec(void){
        double *coeff= memalign(sizeof(double), sizeof(double));
        SwsVector *vec= memalign(8,sizeof(SwsVector));
        coeff[0]= 1.0;

        vec->coeff= coeff;
        vec->length= 1;

        return vec;
}

void normalizeVec(SwsVector *a, double height){
        int i;
        double sum=0;
        double inv;

        for(i=0; i<a->length; i++)
                sum+= a->coeff[i];

        inv= height/sum;

        for(i=0; i<a->length; i++)
                a->coeff[i]*= height;
}

void scaleVec(SwsVector *a, double scalar){
        int i;

        for(i=0; i<a->length; i++)
                a->coeff[i]*= scalar;
}

static SwsVector *getConvVec(SwsVector *a, SwsVector *b){
        int length= a->length + b->length - 1;
        double *coeff= memalign(sizeof(double), length*sizeof(double));
        int i, j;
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= length;

        for(i=0; i<length; i++) coeff[i]= 0.0;

        for(i=0; i<a->length; i++)
        {
                for(j=0; j<b->length; j++)
                {
                        coeff[i+j]+= a->coeff[i]*b->coeff[j];
                }
        }

        return vec;
}

static SwsVector *sumVec(SwsVector *a, SwsVector *b){
        int length= MAX(a->length, b->length);
        double *coeff= memalign(sizeof(double), length*sizeof(double));
        int i;
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= length;

        for(i=0; i<length; i++) coeff[i]= 0.0;

        for(i=0; i<a->length; i++) coeff[i + (length-1)/2 - (a->length-1)/2]+= a->coeff[i];
        for(i=0; i<b->length; i++) coeff[i + (length-1)/2 - (b->length-1)/2]+= b->coeff[i];

        return vec;
}

static SwsVector *diffVec(SwsVector *a, SwsVector *b){
        int length= MAX(a->length, b->length);
        double *coeff= memalign(sizeof(double), length*sizeof(double));
        int i;
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= length;

        for(i=0; i<length; i++) coeff[i]= 0.0;

        for(i=0; i<a->length; i++) coeff[i + (length-1)/2 - (a->length-1)/2]+= a->coeff[i];
        for(i=0; i<b->length; i++) coeff[i + (length-1)/2 - (b->length-1)/2]-= b->coeff[i];

        return vec;
}

/* shift left / or right if "shift" is negative */
static SwsVector *getShiftedVec(SwsVector *a, int shift){
        int length= a->length + ABS(shift)*2;
        double *coeff= memalign(sizeof(double), length*sizeof(double));
        int i;
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= length;

        for(i=0; i<length; i++) coeff[i]= 0.0;

        for(i=0; i<a->length; i++)
        {
                coeff[i + (length-1)/2 - (a->length-1)/2 - shift]= a->coeff[i];
        }

        return vec;
}

void shiftVec(SwsVector *a, int shift){
        SwsVector *shifted= getShiftedVec(a, shift);
        xvid_free(a->coeff);
        a->coeff= shifted->coeff;
        a->length= shifted->length;
        xvid_free(shifted);
}

void addVec(SwsVector *a, SwsVector *b){
        SwsVector *sum= sumVec(a, b);
        xvid_free(a->coeff);
        a->coeff= sum->coeff;
        a->length= sum->length;
        xvid_free(sum);
}

void subVec(SwsVector *a, SwsVector *b){
        SwsVector *diff= diffVec(a, b);
        xvid_free(a->coeff);
        a->coeff= diff->coeff;
        a->length= diff->length;
        xvid_free(diff);
}

void convVec(SwsVector *a, SwsVector *b){
        SwsVector *conv= getConvVec(a, b);
        xvid_free(a->coeff);
        a->coeff= conv->coeff;
        a->length= conv->length;
        xvid_free(conv);
}

SwsVector *cloneVec(SwsVector *a){
        double *coeff= memalign(sizeof(double), a->length*sizeof(double));
        int i;
        SwsVector *vec= memalign(8,sizeof(SwsVector));

        vec->coeff= coeff;
        vec->length= a->length;

        for(i=0; i<a->length; i++) coeff[i]= a->coeff[i];

        return vec;
}

void printVec(SwsVector *a){
        int i;
        double max=0;
        double min=0;
        double range;

        for(i=0; i<a->length; i++)
                if(a->coeff[i]>max) max= a->coeff[i];

        for(i=0; i<a->length; i++)
                if(a->coeff[i]<min) min= a->coeff[i];

        range= max - min;

        for(i=0; i<a->length; i++)
        {
                int x= (int)((a->coeff[i]-min)*60.0/range +0.5);
		MSG_DBG2("%1.3f ", a->coeff[i]);
		for(;x>0; x--) MSG_DBG2(" ");
		MSG_DBG2("|\n");
        }
}

void freeVec(SwsVector *a){
        if(!a) return;
        if(a->coeff) xvid_free(a->coeff);
        a->coeff=NULL;
        a->length=0;
        xvid_free(a);
}

void freeSwsContext(SwsContext *c){
        int i;
        if(!c) return;

        if(c->lumPixBuf)
        {
                for(i=0; i<c->vLumBufSize; i++)
                {
                        if(c->lumPixBuf[i]) xvid_free(c->lumPixBuf[i]);
                        c->lumPixBuf[i]=NULL;
                }
                xvid_free(c->lumPixBuf);
                c->lumPixBuf=NULL;
        }

        if(c->chrPixBuf)
        {
                for(i=0; i<c->vChrBufSize; i++)
                {
                        if(c->chrPixBuf[i]) xvid_free(c->chrPixBuf[i]);
                        c->chrPixBuf[i]=NULL;
                }
                xvid_free(c->chrPixBuf);
                c->chrPixBuf=NULL;
        }

        if(c->vLumFilter) xvid_free(c->vLumFilter);
        c->vLumFilter = NULL;
        if(c->vChrFilter) xvid_free(c->vChrFilter);
        c->vChrFilter = NULL;
        if(c->hLumFilter) xvid_free(c->hLumFilter);
        c->hLumFilter = NULL;
        if(c->hChrFilter) xvid_free(c->hChrFilter);
        c->hChrFilter = NULL;

        if(c->vLumFilterPos) xvid_free(c->vLumFilterPos);
        c->vLumFilterPos = NULL;
        if(c->vChrFilterPos) xvid_free(c->vChrFilterPos);
        c->vChrFilterPos = NULL;
        if(c->hLumFilterPos) xvid_free(c->hLumFilterPos);
        c->hLumFilterPos = NULL;
        if(c->hChrFilterPos) xvid_free(c->hChrFilterPos);
        c->hChrFilterPos = NULL;

        if(c->lumMmxFilter) xvid_free(c->lumMmxFilter);
        c->lumMmxFilter = NULL;
        if(c->chrMmxFilter) xvid_free(c->chrMmxFilter);
        c->chrMmxFilter = NULL;

        if(c->lumMmx2Filter) xvid_free(c->lumMmx2Filter);
        c->lumMmx2Filter=NULL;
        if(c->chrMmx2Filter) xvid_free(c->chrMmx2Filter);
        c->chrMmx2Filter=NULL;
        if(c->lumMmx2FilterPos) xvid_free(c->lumMmx2FilterPos);
        c->lumMmx2FilterPos=NULL;
        if(c->chrMmx2FilterPos) xvid_free(c->chrMmx2FilterPos);
        c->chrMmx2FilterPos=NULL;
	if(c->yuvTable) xvid_free(c->yuvTable);
	c->yuvTable=NULL;

        xvid_free(c);
}


