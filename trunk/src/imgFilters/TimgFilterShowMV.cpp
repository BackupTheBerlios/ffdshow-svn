/*
 * Copyright (c) 2002 Milan Cutka
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

#pragma hdrstop
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "TimgFilterShowMV.h"
#include "TpresetSettings.h"

void TimgFilterShowMV::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
 firsttime=true;
 mv_dx=mv_dy=0;mv=NULL;
}
void TimgFilterShowMV::setMV(int Imv_dx,int Imv_dy,char *Imv)
{
 mv_dx=Imv_dx;mv_dy=Imv_dy;
 mv=Imv;
}
void TimgFilterShowMV::process(const unsigned char *srcY,const unsigned char *,const unsigned char *,
                               unsigned char *dstY,unsigned char *,unsigned char *,
                               const TpresetSettings *cfg)
{
 for (int y=0;y<dyY;y++)
  memcpy(dstY+y*strideY,srcY+y*strideY,dxY);
 if (!mv) return; 
 for(int mb_y=0;mb_y<mv_dy;mb_y++)
  {
   int y=mb_y*16+8;if (y>=dyY) break;
   for(int mb_x=0;mb_x<mv_dx;mb_x++)
    {
     int x=mb_x*16+8;if (x>=dxY) break;
     int mx= mv[2*(mb_y*mv_dx+mb_x)+0]+x;
     int my= mv[2*(mb_y*mv_dx+mb_x)+1]+y;
     if (mx<0) mx=0;
     if (my<0) my=0;
     if (mx>=dxY) mx= dxY-1;
     if (my>=dyY) my= dyY-1;
     int max= abs(mx-x);
     if (abs(my-y) > max) max= abs(my-y);
     // the ugliest linedrawing routine ... 
     for(int i=0; i<max; i++)
      {
       int x1= x + (mx-x)*i/max;      
       int y1= y + (my-y)*i/max;
       dstY[y1*strideY+x1]+=100;
      }
     dstY[y*strideY+x]+=100;
    }
  }
}
