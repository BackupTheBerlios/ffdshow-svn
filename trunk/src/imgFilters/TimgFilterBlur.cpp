#include "TimgFilterBlur.h"
#include "../Tconfig.h"
#include <math.h>

TimgFilterBlur::TimgFilterBlur(void)
{
 oldStrength=-1;
}
void TimgFilterBlur::process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                             unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                             Tconfig *cfg)
{
 if (oldStrength!=cfg->blurStrength)
  {
   oldStrength=cfg->blurStrength;
   __asm emms;
   double kernelF[5],strength=(255-oldStrength)*2;
   for (int i=0;i<5;i++)
    {
     double x=(i-2)/2.0;
     double y=1-strength*x*x;
     if (y<0) y=0;
     kernel[i]=y*255;
    }
  }
 int dx=dxY,dy=dyY,stride=strideY;
 memcpy(dstY,srcY,dx);
 memcpy(dstY+stride*(dy-1),srcY+stride*(dy-1),dx);
}
