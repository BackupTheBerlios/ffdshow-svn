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

#undef MOVNTQ
#undef PAVGB
#undef PREFETCH
#undef PREFETCHW
#undef EMMS
#undef SFENCE

#ifdef HAVE_3DNOW
/* On K6 femms is faster of emms. On K7 femms is directly mapped on emms. */
#define EMMS     "femms"
#else
#define EMMS     "emms"
#endif

#ifdef HAVE_3DNOW
#define PREFETCH  "prefetch"
#define PREFETCHW "prefetchw"
#elif defined ( HAVE_MMX2 )
#define PREFETCH "prefetchnta"
#define PREFETCHW "prefetcht0"
#else
#define PREFETCH "/nop"
#define PREFETCHW "/nop"
#endif

#ifdef HAVE_MMX2
#define SFENCE "sfence"
#else
#define SFENCE "/nop"
#endif

#ifdef HAVE_MMX2
#define PAVGB(a,b) "pavgb " #a ", " #b " \n\t"
#elif defined (HAVE_3DNOW)
#define PAVGB(a,b) "pavgusb " #a ", " #b " \n\t"
#endif

#ifdef HAVE_MMX2
#define MOVNTQ(a,b) "movntq " #a ", " #b " \n\t"
#else
#define MOVNTQ(a,b) "movq " #a ", " #b " \n\t"
#endif

#define YSCALEYUV2YV12X(x) \
                        "xorl %%eax, %%eax              \n\t"\
                        "pxor %%mm3, %%mm3              \n\t"\
                        "pxor %%mm4, %%mm4              \n\t"\
                        "movl %0, %%edx                 \n\t"\
                        ".balign 16                     \n\t" /* FIXME Unroll? */\
                        "1:                             \n\t"\
                        "movl (%1, %%edx, 4), %%esi     \n\t"\
                        "movq (%2, %%edx, 8), %%mm0     \n\t" /* filterCoeff */\
                        "movq " #x "(%%esi, %%eax, 2), %%mm2    \n\t" /* srcData */\
                        "movq 8+" #x "(%%esi, %%eax, 2), %%mm5  \n\t" /* srcData */\
                        "pmulhw %%mm0, %%mm2            \n\t"\
                        "pmulhw %%mm0, %%mm5            \n\t"\
                        "paddw %%mm2, %%mm3             \n\t"\
                        "paddw %%mm5, %%mm4             \n\t"\
                        "addl $1, %%edx                 \n\t"\
                        " jnz 1b                        \n\t"\
                        "psraw $3, %%mm3                \n\t"\
                        "psraw $3, %%mm4                \n\t"\
                        "packuswb %%mm4, %%mm3          \n\t"\
                        MOVNTQ(%%mm3, (%3, %%eax))\
                        "addl $8, %%eax                 \n\t"\
                        "cmpl %4, %%eax                 \n\t"\
                        "pxor %%mm3, %%mm3              \n\t"\
                        "pxor %%mm4, %%mm4              \n\t"\
                        "movl %0, %%edx                 \n\t"\
                        "jb 1b                          \n\t"

#define YSCALEYUV2YV121 \
                        "movl %2, %%eax                 \n\t"\
                        ".balign 16                     \n\t" /* FIXME Unroll? */\
                        "1:                             \n\t"\
                        "movq (%0, %%eax, 2), %%mm0     \n\t"\
                        "movq 8(%0, %%eax, 2), %%mm1    \n\t"\
                        "psraw $7, %%mm0                \n\t"\
                        "psraw $7, %%mm1                \n\t"\
                        "packuswb %%mm1, %%mm0          \n\t"\
                        MOVNTQ(%%mm0, (%1, %%eax))\
                        "addl $8, %%eax                 \n\t"\
                        "jnc 1b                         \n\t"

/*
                        :: "m" (-lumFilterSize), "m" (-chrFilterSize),
                           "m" (lumMmxFilter+lumFilterSize*4), "m" (chrMmxFilter+chrFilterSize*4),
                           "r" (dest), "m" (dstW),
                           "m" (lumSrc+lumFilterSize), "m" (chrSrc+chrFilterSize)
                        : "%eax", "%ebx", "%ecx", "%edx", "%esi"
*/
static inline void RENAME(yuv2yuvX)(int16_t *lumFilter, int16_t **lumSrc, int lumFilterSize,
                                    int16_t *chrFilter, int16_t **chrSrc, int chrFilterSize,
				    uint8_t *dest, uint8_t *uDest, uint8_t *vDest, int dstW, int chrDstW,
                                    int16_t * lumMmxFilter, int16_t * chrMmxFilter)
{
#ifdef HAVE_MMX
        if(uDest != NULL)
        {
                asm volatile(
                                YSCALEYUV2YV12X(0)
                                :: "m" (-chrFilterSize), "r" (chrSrc+chrFilterSize),
				"r" (chrMmxFilter+chrFilterSize*4), "r" (uDest), "m" (chrDstW)
                                : "%eax", "%edx", "%esi"
                        );

                asm volatile(
                                YSCALEYUV2YV12X(4096)
                                :: "m" (-chrFilterSize), "r" (chrSrc+chrFilterSize),
				"r" (chrMmxFilter+chrFilterSize*4), "r" (vDest), "m" (chrDstW)
                                : "%eax", "%edx", "%esi"
                        );
        }

        asm volatile(
                        YSCALEYUV2YV12X(0)
                        :: "m" (-lumFilterSize), "r" (lumSrc+lumFilterSize),
                           "r" (lumMmxFilter+lumFilterSize*4), "r" (dest), "m" (dstW)
                        : "%eax", "%edx", "%esi"
                );
#else
yuv2yuvXinC(lumFilter, lumSrc, lumFilterSize,
            chrFilter, chrSrc, chrFilterSize,
	    dest, uDest, vDest, dstW, chrDstW);
#endif
}

static inline void RENAME(yuv2yuv1)(int16_t *lumSrc, int16_t *chrSrc,
				    uint8_t *dest, uint8_t *uDest, uint8_t *vDest, int dstW, int chrDstW)
{
#ifdef HAVE_MMX
        if(uDest != NULL)
        {
                asm volatile(
                                YSCALEYUV2YV121
				:: "r" (chrSrc + chrDstW), "r" (uDest + chrDstW),
				"g" (-chrDstW)
                                : "%eax"
                        );

                asm volatile(
                                YSCALEYUV2YV121
				:: "r" (chrSrc + 2048 + chrDstW), "r" (vDest + chrDstW),
				"g" (-chrDstW)
                                : "%eax"
                        );
        }

        asm volatile(
                YSCALEYUV2YV121
                :: "r" (lumSrc + dstW), "r" (dest + dstW),
                "g" (-dstW)
                : "%eax"
        );
#else
        int i;
        for(i=0; i<dstW; i++)
        {
                int val= lumSrc[i]>>7;

		if(val&256){
			if(val<0) val=0;
			else      val=255;
		}

		dest[i]= val;
        }

        if(uDest != NULL)
		for(i=0; i<chrDstW; i++)
                {
                        int u=chrSrc[i]>>7;
                        int v=chrSrc[i + 2048]>>7;

			if((u|v)&256){
				if(u<0)         u=0;
				else if (u>255) u=255;
				if(v<0)         v=0;
				else if (v>255) v=255;
			}

			uDest[i]= u;
			vDest[i]= v;
                }
#endif
}


//FIXME yuy2* can read upto 7 samples to much

static inline void RENAME(yuy2ToY)(uint8_t *dst, uint8_t *src, int width)
{
#ifdef HAVE_MMX
        asm volatile(
                "movq "MANGLE(bm01010101)", %%mm2\n\t"
                "movl %0, %%eax                 \n\t"
                "1:                             \n\t"
                "movq (%1, %%eax,2), %%mm0      \n\t"
                "movq 8(%1, %%eax,2), %%mm1     \n\t"
                "pand %%mm2, %%mm0              \n\t"
                "pand %%mm2, %%mm1              \n\t"
                "packuswb %%mm1, %%mm0          \n\t"
                "movq %%mm0, (%2, %%eax)        \n\t"
                "addl $8, %%eax                 \n\t"
                " js 1b                         \n\t"
                : : "g" (-width), "r" (src+width*2), "r" (dst+width)
                : "%eax"
        );
#else
        int i;
        for(i=0; i<width; i++)
                dst[i]= src[2*i];
#endif
}

static inline void RENAME(yuy2ToUV)(uint8_t *dstU, uint8_t *dstV, uint8_t *src1, uint8_t *src2, int width)
{
#if defined (HAVE_MMX2) || defined (HAVE_3DNOW)
        asm volatile(
                "movq "MANGLE(bm01010101)", %%mm4\n\t"
                "movl %0, %%eax                 \n\t"
                "1:                             \n\t"
                "movq (%1, %%eax,4), %%mm0      \n\t"
                "movq 8(%1, %%eax,4), %%mm1     \n\t"
                "movq (%2, %%eax,4), %%mm2      \n\t"
                "movq 8(%2, %%eax,4), %%mm3     \n\t"
                PAVGB(%%mm2, %%mm0)
                PAVGB(%%mm3, %%mm1)
                "psrlw $8, %%mm0                \n\t"
                "psrlw $8, %%mm1                \n\t"
                "packuswb %%mm1, %%mm0          \n\t"
                "movq %%mm0, %%mm1              \n\t"
                "psrlw $8, %%mm0                \n\t"
                "pand %%mm4, %%mm1              \n\t"
                "packuswb %%mm0, %%mm0          \n\t"
                "packuswb %%mm1, %%mm1          \n\t"
                "movd %%mm0, (%4, %%eax)        \n\t"
                "movd %%mm1, (%3, %%eax)        \n\t"
                "addl $4, %%eax                 \n\t"
                " js 1b                         \n\t"
                : : "g" (-width), "r" (src1+width*4), "r" (src2+width*4), "r" (dstU+width), "r" (dstV+width)
                : "%eax"
        );
#else
        int i;
        for(i=0; i<width; i++)
        {
                dstU[i]= (src1[4*i + 1] + src2[4*i + 1])>>1;
                dstV[i]= (src1[4*i + 3] + src2[4*i + 3])>>1;
        }
#endif
}

// Bilinear / Bicubic scaling
static inline void RENAME(hScale)(int16_t *dst, int dstW, uint8_t *src, int srcW, int xInc,
                                  int16_t *filter, int16_t *filterPos, int filterSize)
{
#ifdef HAVE_MMX
        if(filterSize==4) // allways true for upscaling, sometimes for down too
        {
                int counter= -2*dstW;
                filter-= counter*2;
                filterPos-= counter/2;
                dst-= counter/2;
                asm volatile(
                        "pxor %%mm7, %%mm7              \n\t"
                        "movq "MANGLE(w02)", %%mm6      \n\t"
                        "pushl %%ebp                    \n\t" // we use 7 regs here ...
                        "movl %%eax, %%ebp              \n\t"
                        ".balign 16                     \n\t"
                        "1:                             \n\t"
                        "movzwl (%2, %%ebp), %%eax      \n\t"
                        "movzwl 2(%2, %%ebp), %%ebx     \n\t"
                        "movq (%1, %%ebp, 4), %%mm1     \n\t"
                        "movq 8(%1, %%ebp, 4), %%mm3    \n\t"
                        "movd (%3, %%eax), %%mm0        \n\t"
                        "movd (%3, %%ebx), %%mm2        \n\t"
                        "punpcklbw %%mm7, %%mm0         \n\t"
                        "punpcklbw %%mm7, %%mm2         \n\t"
                        "pmaddwd %%mm1, %%mm0           \n\t"
                        "pmaddwd %%mm2, %%mm3           \n\t"
                        "psrad $8, %%mm0                \n\t"
                        "psrad $8, %%mm3                \n\t"
                        "packssdw %%mm3, %%mm0          \n\t"
                        "pmaddwd %%mm6, %%mm0           \n\t"
                        "packssdw %%mm0, %%mm0          \n\t"
                        "movd %%mm0, (%4, %%ebp)        \n\t"
                        "addl $4, %%ebp                 \n\t"
                        " jnc 1b                        \n\t"

                        "popl %%ebp                     \n\t"
                        : "+a" (counter)
                        : "c" (filter), "d" (filterPos), "S" (src), "D" (dst)
                        : "%ebx"
                );
        }
        else if(filterSize==8)
        {
                int counter= -2*dstW;
                filter-= counter*4;
                filterPos-= counter/2;
                dst-= counter/2;
                asm volatile(
                        "pxor %%mm7, %%mm7              \n\t"
                        "movq "MANGLE(w02)", %%mm6      \n\t"
                        "pushl %%ebp                    \n\t" // we use 7 regs here ...
                        "movl %%eax, %%ebp              \n\t"
                        ".balign 16                     \n\t"
                        "1:                             \n\t"
                        "movzwl (%2, %%ebp), %%eax      \n\t"
                        "movzwl 2(%2, %%ebp), %%ebx     \n\t"
                        "movq (%1, %%ebp, 8), %%mm1     \n\t"
                        "movq 16(%1, %%ebp, 8), %%mm3   \n\t"
                        "movd (%3, %%eax), %%mm0        \n\t"
                        "movd (%3, %%ebx), %%mm2        \n\t"
                        "punpcklbw %%mm7, %%mm0         \n\t"
                        "punpcklbw %%mm7, %%mm2         \n\t"
                        "pmaddwd %%mm1, %%mm0           \n\t"
                        "pmaddwd %%mm2, %%mm3           \n\t"

                        "movq 8(%1, %%ebp, 8), %%mm1    \n\t"
                        "movq 24(%1, %%ebp, 8), %%mm5   \n\t"
                        "movd 4(%3, %%eax), %%mm4       \n\t"
                        "movd 4(%3, %%ebx), %%mm2       \n\t"
                        "punpcklbw %%mm7, %%mm4         \n\t"
                        "punpcklbw %%mm7, %%mm2         \n\t"
                        "pmaddwd %%mm1, %%mm4           \n\t"
                        "pmaddwd %%mm2, %%mm5           \n\t"
                        "paddd %%mm4, %%mm0             \n\t"
                        "paddd %%mm5, %%mm3             \n\t"
                                                
                        "psrad $8, %%mm0                \n\t"
                        "psrad $8, %%mm3                \n\t"
                        "packssdw %%mm3, %%mm0          \n\t"
                        "pmaddwd %%mm6, %%mm0           \n\t"
                        "packssdw %%mm0, %%mm0          \n\t"
                        "movd %%mm0, (%4, %%ebp)        \n\t"
                        "addl $4, %%ebp                 \n\t"
                        " jnc 1b                        \n\t"

                        "popl %%ebp                     \n\t"
                        : "+a" (counter)
                        : "c" (filter), "d" (filterPos), "S" (src), "D" (dst)
                        : "%ebx"
                );
        }
        else
        {
                int counter= -2*dstW;
//              filter-= counter*filterSize/2;
                filterPos-= counter/2;
                dst-= counter/2;
                asm volatile(
                        "pxor %%mm7, %%mm7              \n\t"
                        "movq "MANGLE(w02)", %%mm6      \n\t"
                        ".balign 16                     \n\t"
                        "1:                             \n\t"
                        "movl %2, %%ecx                 \n\t"
                        "movzwl (%%ecx, %0), %%eax      \n\t"
                        "movzwl 2(%%ecx, %0), %%ebx     \n\t"
                        "movl %5, %%ecx                 \n\t"
                        "pxor %%mm4, %%mm4              \n\t"
                        "pxor %%mm5, %%mm5              \n\t"
                        "2:                             \n\t"
                        "movq (%1), %%mm1               \n\t"
                        "movq (%1, %6), %%mm3           \n\t"
                        "movd (%%ecx, %%eax), %%mm0     \n\t"
                        "movd (%%ecx, %%ebx), %%mm2     \n\t"
                        "punpcklbw %%mm7, %%mm0         \n\t"
                        "punpcklbw %%mm7, %%mm2         \n\t"
                        "pmaddwd %%mm1, %%mm0           \n\t"
                        "pmaddwd %%mm2, %%mm3           \n\t"
                        "paddd %%mm3, %%mm5             \n\t"
                        "paddd %%mm0, %%mm4             \n\t"
                        "addl $8, %1                    \n\t"
                        "addl $4, %%ecx                 \n\t"
                        "cmpl %4, %%ecx                 \n\t"
                        " jb 2b                         \n\t"
                        "addl %6, %1                    \n\t"
                        "psrad $8, %%mm4                \n\t"
                        "psrad $8, %%mm5                \n\t"
                        "packssdw %%mm5, %%mm4          \n\t"
                        "pmaddwd %%mm6, %%mm4           \n\t"
                        "packssdw %%mm4, %%mm4          \n\t"
                        "movl %3, %%eax                 \n\t"
                        "movd %%mm4, (%%eax, %0)        \n\t"
                        "addl $4, %0                    \n\t"
                        " jnc 1b                        \n\t"

                        : "+r" (counter), "+r" (filter)
                        : "m" (filterPos), "m" (dst), "m"(src+filterSize),
                          "m" (src), "r" (filterSize*2)
                        : "%ebx", "%eax", "%ecx"
                );
        }
#else
        int i;
        for(i=0; i<dstW; i++)
        {
                int j;
                int srcPos= filterPos[i];
                int val=0;
//              printf("filterPos: %d\n", filterPos[i]);
                for(j=0; j<filterSize; j++)
                {
//                      printf("filter: %d, src: %d\n", filter[i], src[srcPos + j]);
                        val += ((int)src[srcPos + j])*filter[filterSize*i + j];
                }
//              filter += hFilterSize;
                dst[i] = MIN(MAX(0, val>>7), (1<<15)-1); // the cubic equation does overflow ...
//              dst[i] = val>>7;
        }
#endif
}
      // *** horizontal scale Y line to temp buffer
static inline void RENAME(hyscale)(uint16_t *dst, int dstWidth, uint8_t *src, int srcW, int xInc,
                                   int flags, int canMMX2BeUsed, int16_t *hLumFilter,
                                   int16_t *hLumFilterPos, int hLumFilterSize, void *funnyYCode, 
                                   int srcFormat, uint8_t *formatConvBuffer, int16_t *mmx2Filter,
                                   int32_t *mmx2FilterPos)
{

#ifdef HAVE_MMX
        // use the new MMX scaler if th mmx2 cant be used (its faster than the x86asm one)
    if(!(flags&SWS_FAST_BILINEAR) || (!canMMX2BeUsed))
#else
    if(!(flags&SWS_FAST_BILINEAR))
#endif
    {
        RENAME(hScale)(dst, dstWidth, src, srcW, xInc, hLumFilter, hLumFilterPos, hLumFilterSize);
    }
    else // Fast Bilinear upscale / crap downscale
    {
#ifdef ARCH_X86
#ifdef HAVE_MMX2
        int i;
        if(canMMX2BeUsed)
        {
                asm volatile(
                        "pxor %%mm7, %%mm7              \n\t"
                        "movl %0, %%ecx                 \n\t"
                        "movl %1, %%edi                 \n\t"
                        "movl %2, %%edx                 \n\t"
                        "movl %3, %%ebx                 \n\t"
                        "xorl %%eax, %%eax              \n\t" // i
                        PREFETCH" (%%ecx)               \n\t"
                        PREFETCH" 32(%%ecx)             \n\t"
                        PREFETCH" 64(%%ecx)             \n\t"

#define FUNNY_Y_CODE \
                        "movl (%%ebx), %%esi            \n\t"\
                        "call *%4                       \n\t"\
                        "addl (%%ebx, %%eax), %%ecx     \n\t"\
                        "addl %%eax, %%edi              \n\t"\
                        "xorl %%eax, %%eax              \n\t"\

FUNNY_Y_CODE
FUNNY_Y_CODE
FUNNY_Y_CODE
FUNNY_Y_CODE
FUNNY_Y_CODE
FUNNY_Y_CODE
FUNNY_Y_CODE
FUNNY_Y_CODE

                        :: "m" (src), "m" (dst), "m" (mmx2Filter), "m" (mmx2FilterPos),
                        "m" (funnyYCode)
                        : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"
                );
                for(i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--) dst[i] = src[srcW-1]*128;
        }
        else
        {
#endif
        //NO MMX just normal asm ...
        asm volatile(
                "xorl %%eax, %%eax              \n\t" // i
                "xorl %%ebx, %%ebx              \n\t" // xx
                "xorl %%ecx, %%ecx              \n\t" // 2*xalpha
                ".balign 16                     \n\t"
                "1:                             \n\t"
                "movzbl  (%0, %%ebx), %%edi     \n\t" //src[xx]
                "movzbl 1(%0, %%ebx), %%esi     \n\t" //src[xx+1]
                "subl %%edi, %%esi              \n\t" //src[xx+1] - src[xx]
                "imull %%ecx, %%esi             \n\t" //(src[xx+1] - src[xx])*2*xalpha
                "shll $16, %%edi                \n\t"
                "addl %%edi, %%esi              \n\t" //src[xx+1]*2*xalpha + src[xx]*(1-2*xalpha)
                "movl %1, %%edi                 \n\t"
                "shrl $9, %%esi                 \n\t"
                "movw %%si, (%%edi, %%eax, 2)   \n\t"
                "addw %4, %%cx                  \n\t" //2*xalpha += xInc&0xFF
                "adcl %3, %%ebx                 \n\t" //xx+= xInc>>8 + carry

                "movzbl (%0, %%ebx), %%edi      \n\t" //src[xx]
                "movzbl 1(%0, %%ebx), %%esi     \n\t" //src[xx+1]
                "subl %%edi, %%esi              \n\t" //src[xx+1] - src[xx]
                "imull %%ecx, %%esi             \n\t" //(src[xx+1] - src[xx])*2*xalpha
                "shll $16, %%edi                \n\t"
                "addl %%edi, %%esi              \n\t" //src[xx+1]*2*xalpha + src[xx]*(1-2*xalpha)
                "movl %1, %%edi                 \n\t"
                "shrl $9, %%esi                 \n\t"
                "movw %%si, 2(%%edi, %%eax, 2)  \n\t"
                "addw %4, %%cx                  \n\t" //2*xalpha += xInc&0xFF
                "adcl %3, %%ebx                 \n\t" //xx+= xInc>>8 + carry


                "addl $2, %%eax                 \n\t"
                "cmpl %2, %%eax                 \n\t"
                " jb 1b                         \n\t"


                :: "r" (src), "m" (dst), "m" (dstWidth), "m" (xInc>>16), "m" (xInc&0xFFFF)
                : "%eax", "%ebx", "%ecx", "%edi", "%esi"
                );
#ifdef HAVE_MMX2
        } //if MMX2 cant be used
#endif
#else
        int i;
        unsigned int xpos=0;
        for(i=0;i<dstWidth;i++)
        {
                register unsigned int xx=xpos>>16;
                register unsigned int xalpha=(xpos&0xFFFF)>>9;
                dst[i]= (src[xx]<<7) + (src[xx+1] - src[xx])*xalpha;
                xpos+=xInc;
        }
#endif
    }
}

inline static void RENAME(hcscale)(uint16_t *dst, int dstWidth, uint8_t *src1, uint8_t *src2,
                                   int srcW, int xInc, int flags, int canMMX2BeUsed, int16_t *hChrFilter,
                                   int16_t *hChrFilterPos, int hChrFilterSize, void *funnyUVCode,
                                   int srcFormat, uint8_t *formatConvBuffer, int16_t *mmx2Filter,
                                   int32_t *mmx2FilterPos)
{

#ifdef HAVE_MMX
        // use the new MMX scaler if th mmx2 cant be used (its faster than the x86asm one)
    if(!(flags&SWS_FAST_BILINEAR) || (!canMMX2BeUsed))
#else
    if(!(flags&SWS_FAST_BILINEAR))
#endif
    {
        RENAME(hScale)(dst     , dstWidth, src1, srcW, xInc, hChrFilter, hChrFilterPos, hChrFilterSize);
        RENAME(hScale)(dst+2048, dstWidth, src2, srcW, xInc, hChrFilter, hChrFilterPos, hChrFilterSize);
    }
    else // Fast Bilinear upscale / crap downscale
    {
#ifdef ARCH_X86
#ifdef HAVE_MMX2
        int i;
        if(canMMX2BeUsed)
        {
                asm volatile(
                        "pxor %%mm7, %%mm7              \n\t"
                        "movl %0, %%ecx                 \n\t"
                        "movl %1, %%edi                 \n\t"
                        "movl %2, %%edx                 \n\t"
                        "movl %3, %%ebx                 \n\t"
                        "xorl %%eax, %%eax              \n\t" // i
                        PREFETCH" (%%ecx)               \n\t"
                        PREFETCH" 32(%%ecx)             \n\t"
                        PREFETCH" 64(%%ecx)             \n\t"

#define FUNNY_UV_CODE \
                        "movl (%%ebx), %%esi            \n\t"\
                        "call *%4                       \n\t"\
                        "addl (%%ebx, %%eax), %%ecx     \n\t"\
                        "addl %%eax, %%edi              \n\t"\
                        "xorl %%eax, %%eax              \n\t"\

FUNNY_UV_CODE
FUNNY_UV_CODE
FUNNY_UV_CODE
FUNNY_UV_CODE
                        "xorl %%eax, %%eax              \n\t" // i
                        "movl %5, %%ecx                 \n\t" // src
                        "movl %1, %%edi                 \n\t" // buf1
                        "addl $4096, %%edi              \n\t"
                        PREFETCH" (%%ecx)               \n\t"
                        PREFETCH" 32(%%ecx)             \n\t"
                        PREFETCH" 64(%%ecx)             \n\t"

FUNNY_UV_CODE
FUNNY_UV_CODE
FUNNY_UV_CODE
FUNNY_UV_CODE

                        :: "m" (src1), "m" (dst), "m" (mmx2Filter), "m" (mmx2FilterPos),
                        "m" (funnyUVCode), "m" (src2)
                        : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"
                );
                for(i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--)
                {
//                      printf("%d %d %d\n", dstWidth, i, srcW);
                        dst[i] = src1[srcW-1]*128;
                        dst[i+2048] = src2[srcW-1]*128;
                }
        }
        else
        {
#endif
        asm volatile(
                "xorl %%eax, %%eax              \n\t" // i
                "xorl %%ebx, %%ebx              \n\t" // xx
                "xorl %%ecx, %%ecx              \n\t" // 2*xalpha
                ".balign 16                     \n\t"
                "1:                             \n\t"
                "movl %0, %%esi                 \n\t"
                "movzbl  (%%esi, %%ebx), %%edi  \n\t" //src[xx]
                "movzbl 1(%%esi, %%ebx), %%esi  \n\t" //src[xx+1]
                "subl %%edi, %%esi              \n\t" //src[xx+1] - src[xx]
                "imull %%ecx, %%esi             \n\t" //(src[xx+1] - src[xx])*2*xalpha
                "shll $16, %%edi                \n\t"
                "addl %%edi, %%esi              \n\t" //src[xx+1]*2*xalpha + src[xx]*(1-2*xalpha)
                "movl %1, %%edi                 \n\t"
                "shrl $9, %%esi                 \n\t"
                "movw %%si, (%%edi, %%eax, 2)   \n\t"

                "movzbl  (%5, %%ebx), %%edi     \n\t" //src[xx]
                "movzbl 1(%5, %%ebx), %%esi     \n\t" //src[xx+1]
                "subl %%edi, %%esi              \n\t" //src[xx+1] - src[xx]
                "imull %%ecx, %%esi             \n\t" //(src[xx+1] - src[xx])*2*xalpha
                "shll $16, %%edi                \n\t"
                "addl %%edi, %%esi              \n\t" //src[xx+1]*2*xalpha + src[xx]*(1-2*xalpha)
                "movl %1, %%edi                 \n\t"
                "shrl $9, %%esi                 \n\t"
                "movw %%si, 4096(%%edi, %%eax, 2)\n\t"

                "addw %4, %%cx                  \n\t" //2*xalpha += xInc&0xFF
                "adcl %3, %%ebx                 \n\t" //xx+= xInc>>8 + carry
                "addl $1, %%eax                 \n\t"
                "cmpl %2, %%eax                 \n\t"
                " jb 1b                         \n\t"

                :: "m" (src1), "m" (dst), "m" (dstWidth), "m" (xInc>>16), "m" (xInc&0xFFFF),
                "r" (src2)
                : "%eax", "%ebx", "%ecx", "%edi", "%esi"
                );
#ifdef HAVE_MMX2
        } //if MMX2 cant be used
#endif
#else
        int i;
        unsigned int xpos=0;
        for(i=0;i<dstWidth;i++)
        {
                register unsigned int xx=xpos>>16;
                register unsigned int xalpha=(xpos&0xFFFF)>>9;
                dst[i]=(src1[xx]*(xalpha^127)+src1[xx+1]*xalpha);
                dst[i+2048]=(src2[xx]*(xalpha^127)+src2[xx+1]*xalpha);
/* slower
          dst[i]= (src1[xx]<<7) + (src1[xx+1] - src1[xx])*xalpha;
          dst[i+2048]=(src2[xx]<<7) + (src2[xx+1] - src2[xx])*xalpha;
*/
                xpos+=xInc;
        }
#endif
   }
}

static void RENAME(swScale)(SwsContext *c, uint8_t* srcParam[], int srcStrideParam[], int srcSliceY,
             int srcSliceH, uint8_t* dstParam[], int dstStrideParam[]){

        /* load a few things into local vars to make the code more readable? and faster */
        const int srcW= c->srcW;
        const int dstW= c->dstW;
        const int dstH= c->dstH;
        const int chrDstW= c->chrDstW;
	const int chrSrcW= c->chrSrcW;
        const int lumXInc= c->lumXInc;
        const int chrXInc= c->chrXInc;
        const int dstFormat= c->dstFormat;
	const int srcFormat= c->srcFormat;
        const int flags= c->flags;
        const int canMMX2BeUsed= c->canMMX2BeUsed;
        int16_t *vLumFilterPos= c->vLumFilterPos;
        int16_t *vChrFilterPos= c->vChrFilterPos;
        int16_t *hLumFilterPos= c->hLumFilterPos;
        int16_t *hChrFilterPos= c->hChrFilterPos;
        int16_t *vLumFilter= c->vLumFilter;
        int16_t *vChrFilter= c->vChrFilter;
        int16_t *hLumFilter= c->hLumFilter;
        int16_t *hChrFilter= c->hChrFilter;
        int16_t *lumMmxFilter= c->lumMmxFilter;
        int16_t *chrMmxFilter= c->chrMmxFilter;
        const int vLumFilterSize= c->vLumFilterSize;
        const int vChrFilterSize= c->vChrFilterSize;
        const int hLumFilterSize= c->hLumFilterSize;
        const int hChrFilterSize= c->hChrFilterSize;
        int16_t **lumPixBuf= c->lumPixBuf;
        int16_t **chrPixBuf= c->chrPixBuf;
        const int vLumBufSize= c->vLumBufSize;
        const int vChrBufSize= c->vChrBufSize;
        uint8_t *funnyYCode= c->funnyYCode;
        uint8_t *funnyUVCode= c->funnyUVCode;
        uint8_t *formatConvBuffer= c->formatConvBuffer;
	const int chrSrcSliceY= srcSliceY >> c->chrSrcVSubSample;
	const int chrSrcSliceH= -((-srcSliceH) >> c->chrSrcVSubSample);

        /* vars whch will change and which we need to storw back in the context */
        int dstY= c->dstY;
        int lumBufIndex= c->lumBufIndex;
        int chrBufIndex= c->chrBufIndex;
        int lastInLumBuf= c->lastInLumBuf;
        int lastInChrBuf= c->lastInChrBuf;
        int srcStride[3];
        int dstStride[3];
        uint8_t *src[3];
        uint8_t *dst[3];
        
	orderYUV(c->srcFormat, src, srcStride, srcParam, srcStrideParam);
	orderYUV(c->dstFormat, dst, dstStride, dstParam, dstStrideParam);

	if(isPacked(c->srcFormat)){
                src[0]=
                src[1]=
                src[2]= srcParam[0];
		srcStride[0]=
                srcStride[1]=
		srcStride[2]= srcStrideParam[0];
        }
	srcStride[1]<<= c->vChrDrop;
	srcStride[2]<<= c->vChrDrop;

//	printf("swscale %X %X %X -> %X %X %X\n", (int)src[0], (int)src[1], (int)src[2],
//		(int)dst[0], (int)dst[1], (int)dst[2]);

#if 0 //self test FIXME move to a vfilter or something
{
static volatile int i=0;
i++;
if(srcFormat==IMGFMT_YV12 && i==1 && srcSliceH>= c->srcH)
	selfTest(src, srcStride, c->srcW, c->srcH);
i--;
        }
#endif

//printf("sws Strides:%d %d %d -> %d %d %d\n", srcStride[0],srcStride[1],srcStride[2],
//dstStride[0],dstStride[1],dstStride[2]);

        if(dstStride[0]%8 !=0 || dstStride[1]%8 !=0 || dstStride[2]%8 !=0)
        {
                static int firstTime=1; //FIXME move this into the context perhaps
                if(flags & SWS_PRINT_INFO && firstTime)
                {
                        mp_msg(MSGT_SWS,MSGL_WARN,"SwScaler: Warning: dstStride is not aligned!\n"
                                        "SwScaler:          ->cannot do aligned memory acesses anymore\n");
                        firstTime=0;
                }
        }

        /* Note the user might start scaling the picture in the middle so this will not get executed
           this is not really intended but works currently, so ppl might do it */
        if(srcSliceY ==0){
                lumBufIndex=0;
                chrBufIndex=0;
                dstY=0; 
                lastInLumBuf= -1;
                lastInChrBuf= -1;
        }

        for(;dstY < dstH; dstY++){
                unsigned char *dest =dst[0]+dstStride[0]*dstY;
		const int chrDstY= dstY>>c->chrDstVSubSample;
		unsigned char *uDest=dst[1]+dstStride[1]*chrDstY;
		unsigned char *vDest=dst[2]+dstStride[2]*chrDstY;

                const int firstLumSrcY= vLumFilterPos[dstY]; //First line needed as input
                const int firstChrSrcY= vChrFilterPos[chrDstY]; //First line needed as input
                const int lastLumSrcY= firstLumSrcY + vLumFilterSize -1; // Last line needed as input
                const int lastChrSrcY= firstChrSrcY + vChrFilterSize -1; // Last line needed as input

                //handle holes (FAST_BILINEAR & weird filters)
                if(firstLumSrcY > lastInLumBuf) lastInLumBuf= firstLumSrcY-1;
                if(firstChrSrcY > lastInChrBuf) lastInChrBuf= firstChrSrcY-1;
//printf("%d %d %d\n", firstChrSrcY, lastInChrBuf, vChrBufSize);
                ASSERT(firstLumSrcY >= lastInLumBuf - vLumBufSize + 1)
                ASSERT(firstChrSrcY >= lastInChrBuf - vChrBufSize + 1)

                // Do we have enough lines in this slice to output the dstY line
		if(lastLumSrcY < srcSliceY + srcSliceH && lastChrSrcY < -((-srcSliceY - srcSliceH)>>c->chrSrcVSubSample))
                {
                        //Do horizontal scaling
                        while(lastInLumBuf < lastLumSrcY)
                        {
                                uint8_t *s= src[0]+(lastInLumBuf + 1 - srcSliceY)*srcStride[0];
                                lumBufIndex++;
//                              printf("%d %d %d %d\n", lumBufIndex, vLumBufSize, lastInLumBuf,  lastLumSrcY);
                                ASSERT(lumBufIndex < 2*vLumBufSize)
                                ASSERT(lastInLumBuf + 1 - srcSliceY < srcSliceH)
                                ASSERT(lastInLumBuf + 1 - srcSliceY >= 0)
//                              printf("%d %d\n", lumBufIndex, vLumBufSize);
                                RENAME(hyscale)(lumPixBuf[ lumBufIndex ], dstW, s, srcW, lumXInc,
                                                flags, canMMX2BeUsed, hLumFilter, hLumFilterPos, hLumFilterSize,
                                                funnyYCode, c->srcFormat, formatConvBuffer, 
                                                c->lumMmx2Filter, c->lumMmx2FilterPos);
                                lastInLumBuf++;
                        }
                        while(lastInChrBuf < lastChrSrcY)
                        {
				uint8_t *src1= src[1]+(lastInChrBuf + 1 - chrSrcSliceY)*srcStride[1];
				uint8_t *src2= src[2]+(lastInChrBuf + 1 - chrSrcSliceY)*srcStride[2];
                                chrBufIndex++;
                                ASSERT(chrBufIndex < 2*vChrBufSize)
				ASSERT(lastInChrBuf + 1 - chrSrcSliceY < (chrSrcSliceH))
				ASSERT(lastInChrBuf + 1 - chrSrcSliceY >= 0)
                                //FIXME replace parameters through context struct (some at least)

				if(!(isGray(srcFormat) || isGray(dstFormat)))
					RENAME(hcscale)(chrPixBuf[ chrBufIndex ], chrDstW, src1, src2, chrSrcW, chrXInc,
                                                flags, canMMX2BeUsed, hChrFilter, hChrFilterPos, hChrFilterSize,
                                                funnyUVCode, c->srcFormat, formatConvBuffer, 
                                                c->chrMmx2Filter, c->chrMmx2FilterPos);
                                lastInChrBuf++;
                        }
                        //wrap buf index around to stay inside the ring buffer
                        if(lumBufIndex >= vLumBufSize ) lumBufIndex-= vLumBufSize;
                        if(chrBufIndex >= vChrBufSize ) chrBufIndex-= vChrBufSize;
                }
                else // not enough lines left in this slice -> load the rest in the buffer
                {
/*              printf("%d %d Last:%d %d LastInBuf:%d %d Index:%d %d Y:%d FSize: %d %d BSize: %d %d\n",
                        firstChrSrcY,firstLumSrcY,lastChrSrcY,lastLumSrcY,
                        lastInChrBuf,lastInLumBuf,chrBufIndex,lumBufIndex,dstY,vChrFilterSize,vLumFilterSize,
			vChrBufSize, vLumBufSize);*/

                        //Do horizontal scaling
                        while(lastInLumBuf+1 < srcSliceY + srcSliceH)
                        {
                                uint8_t *s= src[0]+(lastInLumBuf + 1 - srcSliceY)*srcStride[0];
                                lumBufIndex++;
                                ASSERT(lumBufIndex < 2*vLumBufSize)
                                ASSERT(lastInLumBuf + 1 - srcSliceY < srcSliceH)
                                ASSERT(lastInLumBuf + 1 - srcSliceY >= 0)
                                RENAME(hyscale)(lumPixBuf[ lumBufIndex ], dstW, s, srcW, lumXInc,
                                                flags, canMMX2BeUsed, hLumFilter, hLumFilterPos, hLumFilterSize,
                                                funnyYCode, c->srcFormat, formatConvBuffer, 
                                                c->lumMmx2Filter, c->lumMmx2FilterPos);
                                lastInLumBuf++;
                        }
			while(lastInChrBuf+1 < (chrSrcSliceY + chrSrcSliceH))
                        {
				uint8_t *src1= src[1]+(lastInChrBuf + 1 - chrSrcSliceY)*srcStride[1];
				uint8_t *src2= src[2]+(lastInChrBuf + 1 - chrSrcSliceY)*srcStride[2];
                                chrBufIndex++;
                                ASSERT(chrBufIndex < 2*vChrBufSize)
				ASSERT(lastInChrBuf + 1 - chrSrcSliceY < chrSrcSliceH)
				ASSERT(lastInChrBuf + 1 - chrSrcSliceY >= 0)

				if(!(isGray(srcFormat) || isGray(dstFormat)))
					RENAME(hcscale)(chrPixBuf[ chrBufIndex ], chrDstW, src1, src2, chrSrcW, chrXInc,
                                                flags, canMMX2BeUsed, hChrFilter, hChrFilterPos, hChrFilterSize,
                                                funnyUVCode, c->srcFormat, formatConvBuffer, 
                                                c->chrMmx2Filter, c->chrMmx2FilterPos);
                                lastInChrBuf++;
                        }
                        //wrap buf index around to stay inside the ring buffer
                        if(lumBufIndex >= vLumBufSize ) lumBufIndex-= vLumBufSize;
                        if(chrBufIndex >= vChrBufSize ) chrBufIndex-= vChrBufSize;
                        break; //we cant output a dstY line so lets try with the next slice
                }

#ifdef HAVE_MMX
                b5Dither= dither8[dstY&1];
                g6Dither= dither4[dstY&1];
                g5Dither= dither8[dstY&1];
                r5Dither= dither8[(dstY+1)&1];
#endif
            if(dstY < dstH-2)
            {
		if(isPlanarYUV(dstFormat) || isGray(dstFormat)) //YV12 like
                {
			if((dstY&1) || isGray(dstFormat)) uDest=vDest= NULL; //FIXME split functions in lumi / chromi
                        if(vLumFilterSize == 1 && vChrFilterSize == 1) // Unscaled YV12
                        {
                                int16_t *lumBuf = lumPixBuf[0];
                                int16_t *chrBuf= chrPixBuf[0];
				RENAME(yuv2yuv1)(lumBuf, chrBuf, dest, uDest, vDest, dstW, chrDstW);
                        }
                        else //General YV12
                        {
                                int16_t **lumSrcPtr= lumPixBuf + lumBufIndex + firstLumSrcY - lastInLumBuf + vLumBufSize;
                                int16_t **chrSrcPtr= chrPixBuf + chrBufIndex + firstChrSrcY - lastInChrBuf + vChrBufSize;
                                RENAME(yuv2yuvX)(
                                        vLumFilter+dstY*vLumFilterSize     , lumSrcPtr, vLumFilterSize,
					vChrFilter+chrDstY*vChrFilterSize, chrSrcPtr, vChrFilterSize,
					dest, uDest, vDest, dstW, chrDstW,
					lumMmxFilter+dstY*vLumFilterSize*4, chrMmxFilter+chrDstY*vChrFilterSize*4);
                        }
                }
            }
            else // hmm looks like we cant use MMX here without overwriting this arrays tail
            {
                int16_t **lumSrcPtr= lumPixBuf + lumBufIndex + firstLumSrcY - lastInLumBuf + vLumBufSize;
                int16_t **chrSrcPtr= chrPixBuf + chrBufIndex + firstChrSrcY - lastInChrBuf + vChrBufSize;
		if(isPlanarYUV(dstFormat) || isGray(dstFormat)) //YV12
                {
			if((dstY&1) || isGray(dstFormat)) uDest=vDest= NULL;
                        yuv2yuvXinC(
                                vLumFilter+dstY*vLumFilterSize     , lumSrcPtr, vLumFilterSize,
				vChrFilter+chrDstY*vChrFilterSize, chrSrcPtr, vChrFilterSize,
				dest, uDest, vDest, dstW, chrDstW);
                }
            }
        }

#ifdef HAVE_MMX
        __asm __volatile(SFENCE:::"memory");
        __asm __volatile(EMMS:::"memory");
#endif
        /* store changed local vars back in the context */
        c->dstY= dstY;
        c->lumBufIndex= lumBufIndex;
        c->chrBufIndex= chrBufIndex;
        c->lastInLumBuf= lastInLumBuf;
        c->lastInChrBuf= lastInChrBuf;
}
