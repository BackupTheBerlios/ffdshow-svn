#ifndef _IMGFILTER_H_
#define _IMGFILTER_H_

#include "TffRect.h"
#include "TffPict.h"

struct TpresetSettings;
struct IffDecoder;
class TimgFilter
{
private:
 TffRect oldRect;int oldFull;
protected:
 unsigned int dxY,strideY,dyY;
 unsigned int dxUV,strideUV,dyUV;
 IffDecoder *deci;
 virtual Trect* init(TffRect *rect,int full)
  {
   if (*rect!=oldRect || oldFull!=full)
    {
     done();
     Trect *r=(full==1)?&rect->full:&rect->clip;
     dxY=r->dx;dyY=r->dy;strideY=rect->stride;
     dxUV=dxY/2;dyUV=dyY/2;strideUV=strideY/2;
     r->calcDiff(rect->stride);
     oldRect=*rect;oldFull=full;
    }
   return (oldFull==1)?&rect->full:&rect->clip;
  }
public:
 TimgFilter(void) {deci=NULL;oldFull=-1;}
 virtual void done(void) {}
 void setDeci(IffDecoder *Ideci) {deci=Ideci;}
 virtual void process(TffPict *pict,TffRect &rect,const TpresetSettings *cfg)=0;
};

#endif
