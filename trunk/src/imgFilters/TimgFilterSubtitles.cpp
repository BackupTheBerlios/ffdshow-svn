#include "TimgFilterSubtitles.h"
#include "..\Tconfig.h"

void TimgFilterSubtitles::process(unsigned char *srcY,unsigned char *srcU,unsigned char *srcV,
                                  unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                                  Tconfig *cfg)
{
 if (cfg->fontChanged)
  {
   cfg->fontChanged=false;
   font.init(cfg->fontName,cfg->fontCharset,cfg->fontSize,cfg->fontWeight,cfg->fontSpacing,cfg->fontShadow,cfg->fontColor);
  }
 if (cfg->sub)
  { 
   int size=strideY*dyY;
   int y;
   for (y=0;y<dyY;y++)
    memcpy(dstY+y*strideY,srcY+y*strideY,dxY);
   for (y=0;y<dyUV;y++)
    {
     memcpy(dstU+y*strideUV,srcU+y*strideUV,dxUV);
     memcpy(dstV+y*strideUV,srcV+y*strideUV,dxUV);
    }; 
   font.print(dstY,dstU,dstV,dxY,strideY,dyY,cfg->sub,cfg->subPosX,cfg->subPosY);
  }; 
}                                  
