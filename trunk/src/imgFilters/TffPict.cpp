#include "stdafx.h"
#include "TffRect.h"
#include "TffPict.h"
#include <string.h>

unsigned char* TffPict::TtempPicture::getCurNext(unsigned int stride,const Trect *r,int div)
{
 unsigned char *p=getTempCur();
 if (p==av)
  {
   p=getTempNext();
   if (!r) return p;
   unsigned int y;
   switch (div)
    {
     case 1:for (y=r->y*stride;y<r->dy*stride;y+=stride)
             memcpy(p+y+r->x,av+y+r->x,r->dx);
            return p;
     case 2:for (y=(r->y/2)*(stride/2);y<(r->dy/2)*(stride/2);y+=stride/2)
             memcpy(p+y+r->x/2,av+y+r->x/2,r->dx/2);
            return p;
    }         
  }
 return p;
}
