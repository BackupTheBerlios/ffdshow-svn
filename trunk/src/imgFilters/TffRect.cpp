#include "TffRect.h"
#include "TffPict.h"

void TffRect::clear(unsigned char *y,unsigned char *u,unsigned char *v)
{
 for (unsigned char *yEnd=y+stride*full.dy;y<yEnd;y+=stride)
  memset(y,0,full.dx);
 for (unsigned char *uEnd=u+(stride/2)*(full.dy/2);u<uEnd;u+=stride/2)
  memset(u,128,full.dx/2);
 for (unsigned char *vEnd=v+(stride/2)*(full.dy/2);v<vEnd;v+=stride/2)
  memset(v,128,full.dx/2);
}
