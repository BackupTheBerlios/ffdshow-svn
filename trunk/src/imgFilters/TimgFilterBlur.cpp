#include "TimgFilterBlur.h"
#include "TpresetSettings.h"
#include <math.h>
#include <stdlib.h>
#include "..\xvid\utils\mem_align.h"

TimgFilterBlur::TimgFilterBlur(void)
{
 oldStrength=-1;
 tempPict=NULL;
}
void TimgFilterBlur::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
 tempPict=(unsigned char*)xvid_malloc(strideY*dyY,MCACHE_LINE);
}
void TimgFilterBlur::done(void)
{
 if (tempPict) xvid_free(tempPict);
}
void TimgFilterBlur::process(unsigned char *srcY,unsigned char *,unsigned char *,
                             unsigned char *dstY,unsigned char *,unsigned char *,
                             TpresetSettings *cfg)
{
 if (oldStrength!=cfg->blurStrength)
  {
   oldStrength=cfg->blurStrength;
   int sum=0;
   sum+=kernel[0]=cfg->blurStrength;
   sum+=kernel[1]=255;
   sum+=kernel[2]=cfg->blurStrength;
   sum=(256*256)/sum;
   kernel[0]=(sum*kernel[0])>>8;
   kernel[1]=(sum*kernel[1])>>8;
   kernel[2]=(sum*kernel[2])>>8;
   kernel[1]+=255-(kernel[0]+kernel[1]+kernel[2]);
  }
 int dx=dxY,dy=dyY,stride=strideY;
 __declspec(align(8)) static __int64 k0;
 __declspec(align(8)) static __int64 k1;
 __declspec(align(8)) static __int64 k2;
 k0=(kernel[0]<<48)+(kernel[0]<<32)+(kernel[0]<<16)+kernel[0];
 k1=(kernel[1]<<48)+(kernel[1]<<32)+(kernel[1]<<16)+kernel[1];
 k2=(kernel[2]<<48)+(kernel[2]<<32)+(kernel[2]<<16)+kernel[2];
 unsigned char *src,*srcEnd,*dst;
 for (src=srcY,srcEnd=srcY+dy*stride,dst=tempPict;src<srcEnd;src+=stride,dst+=stride)
  {
   unsigned char *srcLnEnd=src+dx-1;
   __asm
    {
     mov esi,[src]
     inc esi
     mov edi,[dst]
     inc edi
     mov eax,[srcLnEnd]
     movq mm5,[k0]
     movq mm6,[k1]
     movq mm7,[k2]
    blur1:
     pxor mm0,mm0
     punpcklbw mm0,[esi-1];     
     psrlw mm0,8        
     pmullw mm0,mm5
     psrlw mm0,8

     pxor mm1,mm1
     punpcklbw mm1,[esi];     
     psrlw mm1,8        
     pmullw mm1,mm6
     psrlw mm1,8
     paddusb mm0,mm1

     pxor mm2,mm2
     punpcklbw mm2,[esi+1];     
     psrlw mm2,8        
     pmullw mm2,mm7
     psrlw mm2,8

     paddusb mm0,mm2

     pxor mm1,mm1
     packuswb mm0,mm1
     movq [edi],mm0
     add esi,4
     add edi,4
     cmp esi,eax
     jl  blur1
    }
  };
 unsigned char *srcL=srcY+stride,*srcR=srcL+dx-1,*dstL=tempPict+stride,*dstR=dstL+dx-1;
 for (int y=1;y<dy-1;srcL+=stride,srcR+=stride,dstL+=stride,dstR+=stride,y++)
  {
   *dstL=*srcL;
   *dstR=*srcR;
  }
 for (src=tempPict+stride,srcEnd=tempPict+(dy-1)*stride,dst=dstY+stride;src<srcEnd;src+=stride,dst+=stride)
  {
   unsigned char *srcLnEnd=src+dx;
   __asm
    {
     mov eax,stride;
     mov esi,[src]
     mov ecx,esi
     sub ecx,eax
     mov edx,esi
     add ecx,eax
     mov edi,[dst]
     mov eax,[srcLnEnd]
     movq mm5,[k0]
     movq mm6,[k1]
     movq mm7,[k2]
    blur2:
     pxor mm0,mm0
     punpcklbw mm0,[ecx];     
     psrlw mm0,8        
     pmullw mm0,mm5
     psrlw mm0,8

     pxor mm1,mm1
     punpcklbw mm1,[esi];     
     psrlw mm1,8        
     pmullw mm1,mm6
     psrlw mm1,8
     paddusb mm0,mm1

     pxor mm2,mm2
     punpcklbw mm2,[edx];     
     psrlw mm2,8        
     pmullw mm2,mm7
     psrlw mm2,8

     paddusb mm0,mm2

     pxor mm1,mm1
     packuswb mm0,mm1
     movq [edi],mm0

     add ecx,4
     add esi,4
     add edx,4
     add edi,4
     cmp esi,eax
     jl  blur2
    }
  };
 memcpy(dstY,tempPict,dx);
 memcpy(dstY+strideY*(dy-1),tempPict+strideY*(dy-1),dx);
 __asm emms;
}
