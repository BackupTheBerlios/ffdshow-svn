/*
 * Copyright (c) 2002 Milan Cutka
 * algorithms based on VirtualDub filters by Donald A. Graft
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "TimgFilterSharpen.h"
#include "TpresetSettings.h"
#include "Tconfig.h"
#include "..\xvid\utils\mem_align.h"
#include "..\xvid\xvid.h"

const int TpresetSettings::xsharp_strengthDef=20,TpresetSettings::xsharp_thresholdDef=150;
const int TpresetSettings::unsharp_strengthDef=40,TpresetSettings::unsharp_thresholdDef=0;

__declspec(align(8)) static const __int64 m255=0x00ff00ff00ff00ff;
__declspec(align(8)) static const __int64 ones=0xffffffffffffffff;

TimgFilterSharpen::TimgFilterSharpen(void)
{
 Ymin=Ymax=NULL;
}
void TimgFilterSharpen::done(void)
{
 if (Ymin)
  {
   xvid_free(Ymin);
   Ymin=NULL;
  };
 if (Ymax)
  {
   xvid_free(Ymax);
   Ymax=NULL;
  } 
}
void TimgFilterSharpen::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
 Ymin=(unsigned char*)xvid_malloc(strideY*dyY*2,MCACHE_LINE);
 Ymax=(unsigned char*)xvid_malloc(strideY*dyY,MCACHE_LINE);
}

void TimgFilterSharpen::xsharpen(unsigned char *src,unsigned char *dst,TpresetSettings *cfg)
{
 int dx=dxY,stride=strideY,dy=dyY;
 unsigned char *srcLnEnd=src+stride*dy;
 unsigned char *srcLn,*dstLn,*YminLn,*YmaxLn;

 #if 0
 for (srcLn=src,YminLn=Ymin,YmaxLn=Ymax;srcLn<srcLnEnd;srcLn+=stride,YminLn+=stride,YmaxLn+=stride)
  {
   unsigned char *srcPend=srcLn+(dx-1);
   for (unsigned char *srcP=srcLn+1,*YminP=YminLn+1,*YmaxP=YmaxLn+1;srcP<srcPend;srcP++,YminP++,YmaxP++)
    {
     unsigned char min=srcP[-1];
     if (srcP[0]<min) min=srcP[0];
     if (srcP[1]<min) min=srcP[1];
     *YminP=min;
     unsigned char max=srcP[-1];
     if (srcP[0]>max) max=srcP[0];
     if (srcP[1]>max) max=srcP[1];
     *YmaxP=max;
    }
  };  
 #else
 for (srcLn=src,YminLn=Ymin,YmaxLn=Ymax;srcLn<srcLnEnd;srcLn+=stride,YminLn+=stride,YmaxLn+=stride)
  {
   unsigned char *srcPend=srcLn+(dx-1);
   __asm
    {
     mov esi,[srcLn]
     mov ecx,[YminLn]
     mov edx,[YmaxLn]
     inc esi
     inc ecx
     inc edx
    lineLoop1:
     movq mm1,[esi-1]
     movq mm2,[esi]
     movq mm5,mm2
     movq mm3,[esi+1]

     PMINUB mm2,mm1
     PMINUB mm2,mm3 //mm2 - min

     PMAXUB mm5,mm1
     PMAXUB mm5,mm3 //mm5 - max

     movq [ecx],mm2
     movq [edx],mm5

     add esi,8
     add ecx,8
     add edx,8
     cmp esi,[srcPend]
     jl lineLoop1
    }
  };
 #endif

 srcLnEnd=src+stride*(dy-1);
 memcpy(dst,src,dx);
 memcpy(dst+stride*(dy-1),src+stride*(dy-1),dx);

 #if 0
 int mfd_strength =cfg->xsharp_strength; // 0-127
 int mfd_strengthInv=127-mfd_strength;
 int mfd_threshold=cfg->xsharp_threshold; // 0-255
 for (srcLn=src+stride,dstLn=dst+stride,YminLn=Ymin+stride,YmaxLn=Ymax+stride;srcLn<srcLnEnd;srcLn+=stride,dstLn+=stride,YminLn+=stride,YmaxLn+=stride)
  {
   for (int x=1;x<dx-1;x++)
    {
     unsigned char lumamin=YminLn[x-dx];
     if (YminLn[x+0]<lumamin) lumamin=YminLn[x+0];
     if (YminLn[x+dx]<lumamin) lumamin=YminLn[x+dx];
     unsigned char lumamax=YmaxLn[x-dx];
     if (YmaxLn[x+0]>lumamax) lumamax=YmaxLn[x+0];
     if (YmaxLn[x+dx]>lumamax) lumamax=YmaxLn[x+dx];
     int p=-1;
     unsigned char lumac=srcLn[x+0];
     int mindiff = lumac - lumamin;
     int maxdiff = lumamax - lumac;
     if (mindiff > maxdiff)
      {
       if ( mfd_threshold > maxdiff)
        p = lumamax;
      }
     else
      {
       if (mfd_threshold > mindiff )
        p = lumamin;
      };

     if (p == -1)
      dstLn[x] = srcLn[x];
     else
      {
       int Y=srcLn[x];
       int y=p;
       y=(mfd_strength * y + mfd_strengthInv * Y) / 128;
       dstLn[x] = y;
      } 
    }  
  }
 #else    
 __int64 mfd_strength=cfg->xsharp_strength; // 0-127
 __int64 mfd_strengthInv=127-mfd_strength;
 __int64 mfd_threshold=cfg->xsharp_threshold; // 0-255
 __declspec(align(8)) static __int64 mtf_strength64;
 __declspec(align(8)) static __int64 mtf_strengthInv64;
 __declspec(align(8)) static __int64 mtf_thresh64;
 mtf_strength64=mfd_strength +(mfd_strength <<16)+(mfd_strength <<32)+(mfd_strength <<48);
 mtf_strengthInv64=mfd_strengthInv +(mfd_strengthInv <<16)+(mfd_strengthInv <<32)+(mfd_strengthInv <<48);
 mtf_thresh64=mfd_threshold+(mfd_threshold<<16)+(mfd_threshold<<32)+(mfd_threshold<<48);
 for (srcLn=src+stride,dstLn=dst+stride,YminLn=Ymin+stride,YmaxLn=Ymax+stride;srcLn<srcLnEnd;srcLn+=stride,dstLn+=stride,YminLn+=stride,YmaxLn+=stride)
  {
   unsigned char *srcPend=srcLn+dx-1;
   __asm
    {
     mov eax,[stride]
     mov ebx,eax
     neg ebx
     
     mov ecx,[YminLn]
     mov edx,[YmaxLn]
     mov esi,[srcLn]
     mov edi,[dstLn]
    lineLoop2:
     pxor mm1,mm1
     punpcklbw mm1,[ecx];     
     psrlw mm1,8              //mm0 - 0,YminLn[0]    ,0,YminLn[1]    ,0,YminLn[2]    ,0,YminLn[3] 
       
     pxor mm2,mm2
     punpcklbw mm2,[ecx+ebx]  
     psrlw mm2,8              //mm1 - 0,YminLn[-dx+0],0,YminLn[-dx+1],0,YminLn[-dx+2],0,YminLn[-dx+3]
       
     pxor mm3,mm3
     punpcklbw mm3,[ecx+eax]  
     psrlw mm3,8              //mm2 - 0,YminLn[+dx+0],0,YminLn[+dx+1],0,YminLn[+dx+2],0,YminLn[+dx+3]
       
     PMINSW mm1,mm2
     PMINSW mm1,mm3           //mm1 - minimum z okolia (lumamin)
       
     pxor mm3,mm3
     punpcklbw mm3,[edx];     
     psrlw mm3,8              //mm0 - 0,YmaxLn[0]    ,0,YmaxLn[1]    ,0,YmaxLn[2]    ,0,YmaxLn[3] 

     pxor mm0,mm0
     punpcklbw mm0,[edx+ebx]  
     psrlw mm0,8              //mm3 - 0,YmaxLn[-dx+0],0,YmaxLn[-dx+1],0,YmaxLn[-dx+2],0,YmaxLn[-dx+3]
       
     pxor mm2,mm2
     punpcklbw mm2,[edx+eax]  
     psrlw mm2,8              //mm4 - 0,YmaxLn[+dx+0],0,YmaxLn[+dx+1],0,YmaxLn[+dx+2],0,YmaxLn[+dx+3]

     PMAXSW mm3,mm0
     PMAXSW mm3,mm2           //mm3 - maximum z okolia (lumamax)
       
     pxor mm0,mm0
     punpcklbw mm0,[esi]  
     psrlw mm0,8              //mm0 - 0,lumac[0],0,lumac[1],0,lumac[2],0,lumca[3]
       
     movq mm7,[ones]//pxor mm7,mm7      //mm7 - 0
     movq mm2,mm0      //  mm1 - lumamin
     psubw mm2,mm1     //mm2 - mindiff = lumac - lumamin
     movq mm4,mm3      //  mm3 - lumamax
     psubw mm4,mm0     //mm4 - maxdiff = lumamax - lumac
     movq mm5,mm2
     pcmpgtw mm5,mm4   //mm5 - mindiff>maxdiff
     movq mm6,[mtf_thresh64] 
     pcmpgtw mm6,mm4   //mm6 - mfd_threshold>maxdiff
     pand mm6,mm5      //mm6 - mindiff>maxdiff AND mfd_threshold>maxdiff 
     pand mm7,mm6
     pand mm7,mm3//por  mm7,mm3
     movq mm4,[mtf_thresh64]
     pcmpgtw mm4,mm2   //mm4 - mfd_threshol>mindiff
     pxor mm5,[ones]
     pand mm4,mm5      //mm4 - mindiff<maxdiff AND mfd_threshold>maxdiff    
     pand mm1,mm4
     por  mm7,mm1      //mm7 - p, ale len to, co treba zmenit
     por  mm4,mm6      //mm4- =y (mindiff>maxdiff AND mfd_threshold>maxdiff) OR (mindiff<maxdiff AND mfd_threshold>maxdiff)
     pmullw mm7,[mtf_strength64]
     movq mm6,mm0      //mm6 - Y
     pmullw mm6,[mtf_strengthInv64]
     paddw mm7,mm6
     psrlw mm7,7
     pand mm7,mm4
     pxor mm4,[ones]
     pand mm0,mm4
     por  mm0,mm7
     pxor mm1,mm1
     packuswb mm0,mm1
     movd [edi],mm0
     add edi,4
     add ecx,4
     add edx,4
     add esi,4
     cmp esi,[srcPend]
     jl lineLoop2
    };
  }  
 #endif  
 unsigned char *srcL=src+stride,*srcR=srcL+dx-1,*dstL=dst+stride,*dstR=dstL+dx-1;
 for (int y=1;y<dy-1;srcL+=stride,srcR+=stride,dstL+=stride,dstR+=stride,y++)
  {
   *dstL=*srcL;
   *dstR=*srcR;
  }
 __asm {emms}; 
}

void TimgFilterSharpen::unsharpen(unsigned char *src,unsigned char *dst,TpresetSettings *cfg)
{
 int dx=dxY,stride=strideY,dy=dyY;
 memcpy(dst,src,dx);
 memcpy(dst+stride*(dy-1),src+stride*(dy-1),dx);
 int y;
 unsigned char *srcOrig=src,*dstOrig=dst;

 static const int C_SCALE=32;
#ifdef C_UNSHARP
 src+=stride+1;dst+=stride+1;
 int T=cfg->unsharp_threshold;
 int C=cfg->unsharp_strength+C_SCALE;  //strength = 0-250
 for (y=1;y<dy-2;y++,src+=(stride-dx)+2,dst+=(stride-dx)+2) //threshold = 0-100
  for (int x=1;x<dx-1;x++,src++,dst++)
   {
	int y_sum=0;
    y_sum+=src[-stride-1];
    y_sum+=src[-stride  ];
    y_sum+=src[-stride+1];
    y_sum+=src[       -1];
    y_sum+=src[        0];
    y_sum+=src[       +1];
    y_sum+=src[+stride-1];
    y_sum+=src[+stride  ];
    y_sum+=src[+stride+1];
    y_sum/=9;
	// Now we have the neighborhood average for the current pixel. 

	// Decide whether this pixel is above threshold. Pass it through if it isn't. 
    int diff=*src-y_sum;
	if (abs(diff)>T)
 	 {
 	  // Apply the unsharp masking and output the pixel. 
	  int tmp=C*diff/C_SCALE+y_sum;
  	  if (tmp<0) tmp=0;else if (tmp>255) tmp=255;
	  *dst=tmp;
  	 }
	else
	 *dst=*src;
   }
#else
 unsigned short *Ysum=(unsigned short*)Ymin;

 unsigned short *YsumLn=Ysum;
 unsigned char *srcLn=src;
 int x;
 for (x=1;x<dx-1;x++) YsumLn[x]=(unsigned int)srcLn[x-1]+(unsigned int)srcLn[x]+(unsigned int)srcLn[x+1];
 YsumLn+=stride;
 srcLn+=stride;
 for (x=1;x<dx-1;x++) YsumLn[x]=(unsigned int)srcLn[x-1]+(unsigned int)srcLn[x]+(unsigned int)srcLn[x+1];

 static const __int64 div9=7281;
 __declspec(align(8)) static const __int64 div9_64=(div9<<48)+(div9<<32)+(div9<<16)+div9;
 __int64 T=cfg->unsharp_threshold;
 __int64 C=cfg->unsharp_strength+C_SCALE;  //strength = 0-250
 __declspec(align(8)) static __int64 T_64;
 T_64=(T<<48)+(T<<32)+(T<<16)+T;
 __declspec(align(8)) static __int64 C_64;
 C_64=(C<<48)+(C<<32)+(C<<16)+C;

 //for (int i=0;i<20;i++) src[2*stride+i]=i;

 unsigned char *srcEnd=src+(dy-1)*stride;
 unsigned short *sum=Ysum,*sum1=Ysum+stride;
 src+=stride;dst+=stride;sum+=stride;sum1+=stride;
 for (;src<srcEnd;src+=stride,dst+=stride,sum+=stride,sum1+=stride)
  {
   unsigned  char *srcLnEnd=src+dx-1;
   __asm
    {
     mov edx,[stride]
     mov esi,[src]
     add esi,edx
     mov edi,[sum]
     lea edi,[edi+edx*2+2];
     mov eax,[srcLnEnd]
     add eax,edx
    sumL1:
     movq mm0,[esi]

     pxor mm1,mm1
     punpcklbw mm1,mm0  
     psrlw mm1,8        

     PSRLQ mm0,8
     pxor mm2,mm2
     punpcklbw mm2,mm0  
     psrlw mm2,8        

     paddw mm1,mm2

     PSRLQ mm0,8
     pxor mm2,mm2
     punpcklbw mm2,mm0  
     psrlw mm2,8        

     paddw mm1,mm2

     movq [edi],mm1
     
     add esi,4
     add edi,8
     cmp esi,eax
     jl  sumL1
    };
   //for (x=1;x<dx-1;x++) sum1[x]=(unsigned int)src[stride+x-1]+(unsigned int)src[stride+x]+(unsigned int)src[stride+x+1];
   __asm
    {
     mov esi,[src]
     mov edi,[dst]
     mov edx,[stride]    //edx = stride
     add edx,edx
     mov ebx,edx
     neg ebx             //ebx = -stride
     mov ecx,[sum]       //pointer to sum array
     mov eax,[srcLnEnd]  //eax = srcLnEnd
     movq mm7,[div9_64]
     movq mm6,[T_64]     //mm6 = T
     movq mm5,[C_64]     //mm5 = C
    unsharpL1:
     movq  mm1,[ecx]
     paddw mm1,[ecx+ebx]  
     paddw mm1,[ecx+edx]

     pmulhw mm1,mm7      //mm1 = y_sum = y_sum/9

     pxor mm2,mm2
     punpcklbw mm2,[esi]  
     psrlw mm2,8         //mm2 = *src

     psubw mm2,mm1       //mm2 = diff
/*
     MOVQ mm1, mm2     
     PSRAW mm2,15   
     PXOR mm2, mm1        //mm2 = abs(diff)

     PCMPGTW mm2,mm6
*/
     pmullw mm2,mm5       //mm2 = C*diff
     psraw  mm2,5         //mm2 = C*diff/32
     paddsw  mm2,mm1      //mm2 = C*diff/32 + y_sum

     pxor mm0,mm0
     packuswb mm2,mm0
     movd [edi],mm2

     add esi,4
     add edi,4
     add ecx,8
     cmp  esi,eax
     jl unsharpL1
    }
  }
 __asm{emms};
#endif 
 unsigned char *srcL=srcOrig+stride,*srcR=srcL+dx-1,*dstL=dstOrig+stride,*dstR=dstL+dx-1;
 for (y=1;y<dy-1;srcL+=stride,srcR+=stride,dstL+=stride,dstR+=stride,y++)
  {
   *dstL=*srcL;
   *dstR=*srcR;
  }
}

void TimgFilterSharpen::process(unsigned char *srcY,unsigned char *,unsigned char *,
                                unsigned char *dstY,unsigned char *,unsigned char *,
                                TpresetSettings *cfg)
{
 if (cfg->sharpenMethod==0)
  {
   if (config.cpu_flags&XVID_CPU_MMXEXT)
    xsharpen(srcY,dstY,cfg);
  }
 else if (cfg->sharpenMethod==1)
  unsharpen(srcY,dstY,cfg);
}                                
