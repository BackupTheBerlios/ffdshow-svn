/*
 * Copyright (c) 2002 Milan Cutka
 * line drawing routine found at http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html
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
#include "TmovieSource.h"

void TimgFilterShowMV::init(int Idx,int Istride,int Idy)
{
 TimgFilter::init(Idx,Istride,Idy);
 firsttime=true;
}
void TimgFilterShowMV::process(TtempPictures *pict,const TpresetSettings *cfg,const TmovieSource *movie)
{
 TmovieSource::TmotionVectors mv=movie->getMV();
 if (!mv.vectors) return;
 const unsigned char *srcY=pict->getCurY();unsigned char *dstY=pict->getNextY();
 for (int y=0;y<dyY;y++) memcpy(dstY+y*strideY,srcY+y*strideY,dxY);
 for (int mb_y=0;mb_y<mv.dy;mb_y++)
  {
   int y=mb_y*16+8;
   if (y>=dyY) break;
   for (int mb_x=0;mb_x<mv.dx;mb_x++)
    {
     int x=mb_x*16+8;
     if (x>=dxY) break;
     int mx=mv.vectors[2*(mb_y*mv.dx+mb_x)+0]+x;
     int my=mv.vectors[2*(mb_y*mv.dx+mb_x)+1]+y;
     if (mx<0) mx=0;if (mx>=dxY) mx=dxY-1;
     if (my<0) my=0;if (my>=dyY) my=dyY-1;
     line(dstY,x,y,mx,my);
     dstY[y*strideY+x]+=100;
    }
  }
}

void TimgFilterShowMV::line(unsigned char *dst,int _x0,int _y0,int _x1,int _y1)
{
 if (_x0==_x1 && _y0==_y1)
  {
   dst[_y0*strideY+_x0]+=100;
   return;
  };
 #define SET_PIXEL(adr,a1,a2) (a1),(a2),*adr+=100

 int dy=_y1-_y0;
 int dx=_x1-_x0;
 int stepx,stepy;

 if (dy<0)
  {
   dy=-dy;
   stepy=-strideY;
  }
 else
  stepy=strideY;
 if (dx<0)
  {
   dx=-dx;
   stepx=-1;
  }
 else
  stepx=1;

 unsigned char *adr0=dst+_y0*strideY+_x0,*adr1=dst+_y1*strideY+_x1;
 #define x0 adr0
 #define y0 adr0
 #define x1 adr1
 #define y1 adr1

 SET_PIXEL(adr0,x0,y0);
 SET_PIXEL(adr1,x1,y1);
 if (dx>dy)
  {
   int length=(dx-1)>>2;
   int extras=(dx-1)&3;
   int incr2=(dy<<2)-(dx<<1);
   if (incr2<0)
    {
     int c=dy<<1;
     int incr1=c<<1;
     int d=incr1-dx;
     for (int i=0; i<length; i++)
      {
       x0+=stepx;
       x1-=stepx;
       if (d<0)
        {      // Pattern:
         SET_PIXEL(adr0,x0,y0);   //
         SET_PIXEL(adr0,x0+=stepx,y0); //  x o o
         SET_PIXEL(adr1,x1,y1);   //
         SET_PIXEL(adr1,x1-=stepx,y1);
         d+=incr1;
        }
       else
        {
         if (d<c)
          {       // Pattern:
           SET_PIXEL(adr0,x0,y0);    //      o
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);  //  x o
           SET_PIXEL(adr1,x1,y1);    //
           SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
          }
         else
          {
           SET_PIXEL(adr0,x0,y0+=stepy);   // Pattern:
           SET_PIXEL(adr0,x0+=stepx,y0);   //    o o
           SET_PIXEL(adr1,x1,y1-=stepy);   //  x
           SET_PIXEL(adr1,x1-=stepx,y1);   //
          }
         d+=incr2;
        }
      }
     if (extras>0)
      {
       if (d<0)
        {
         SET_PIXEL(adr0,x0+=stepx,y0);
         if (extras>1)
          SET_PIXEL(adr0,x0+=stepx,y0);
         if (extras>2)
          SET_PIXEL(adr1,x1-=stepx,y1);
        }
       else
        if (d<c)
         {
          SET_PIXEL(adr0,x0+=stepx,y0);
          if (extras>1)
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>2)
           SET_PIXEL(adr1,x1-=stepx,y1);
         }
        else
         {
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>1)
           SET_PIXEL(adr0,x0+=stepx,y0);
          if (extras>2)
           SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
         }
      }
    }
   else
    {
     int c=(dy-dx)<<1;
     int incr1=c<<1;
     int d=incr1+dx;
     for (int i=0; i<length; i++)
      {
       x0+=stepx;
       x1-=stepx;
       if (d>0)
        {
         SET_PIXEL(adr0,x0,y0+=stepy);   // Pattern:
         SET_PIXEL(adr0,x0+=stepx,y0+=stepy);  //      o
         SET_PIXEL(adr1,x1,y1-=stepy);   //    o
         SET_PIXEL(adr1,x1-=stepx,y1-=stepy);  //  x
         d+=incr1;
        }
       else
        {
         if (d<c)
          {
           SET_PIXEL(adr0,x0,y0);    // Pattern:
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);       //      o
           SET_PIXEL(adr1,x1,y1);                         //  x o
           SET_PIXEL(adr1,x1-=stepx,y1-=stepy);       //
          }
         else
          {
           SET_PIXEL(adr0,x0,y0+=stepy);   // Pattern:
           SET_PIXEL(adr0,x0+=stepx,y0);   //    o o
           SET_PIXEL(adr1,x1,y1-=stepy);   //  x
           SET_PIXEL(adr1,x1-=stepx,y1);   //
          }
         d+=incr2;
        }
      }
     if (extras>0)
      {
       if (d>0)
        {
         SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
         if (extras>1)
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
         if (extras>2)
          SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
        }
       else
        if (d<c)
         {
          SET_PIXEL(adr0,x0+=stepx,y0);
          if (extras>1)
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>2)
           SET_PIXEL(adr1,x1-=stepx,y1);
         }
        else
         {
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>1)
           SET_PIXEL(adr0,x0+=stepx,y0);
          if (extras>2)
           {
            if (d>c)
             SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
            else
             SET_PIXEL(adr1,x1-=stepx,y1);
           }
         }
      }
    }
  }
 else
  {
   int length=(dy-1)>>2;
   int extras=(dy-1)&3;
   int incr2=(dx<<2)-(dy<<1);
   if (incr2<0)
    {
     int c=dx<<1;
     int incr1=c<<1;
     int d=incr1-dy;
     for (int i=0; i<length; i++)
      {
       y0+=stepy;
       y1-=stepy;
       if (d<0)
        {
         SET_PIXEL(adr0,x0,y0);
         SET_PIXEL(adr0,x0,y0+=stepy);
         SET_PIXEL(adr1,x1,y1);
         SET_PIXEL(adr1,x1,y1-=stepy);
         d+=incr1;
        }
       else
        {
         if (d<c)
          {
           SET_PIXEL(adr0,x0,y0);
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
           SET_PIXEL(adr1,x1,y1);
           SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
          }
         else
          {
           SET_PIXEL(adr0,x0+=stepx,y0);
           SET_PIXEL(adr0,x0,y0+=stepy);
           SET_PIXEL(adr1,x1-=stepx,y1);
           SET_PIXEL(adr1,x1,y1-=stepy);
          }
         d+=incr2;
        }
      }
     if (extras>0)
      {
       if (d<0)
        {
         SET_PIXEL(adr0,x0,y0+=stepy);
         if (extras>1)
          SET_PIXEL(adr0,x0,y0+=stepy);
         if (extras>2)
          SET_PIXEL(adr1,x1,y1-=stepy);
        }
       else
        if (d<c)
         {
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>1)
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>2)
           SET_PIXEL(adr1,x1,y1-=stepy);
         }
        else
         {
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>1)
           SET_PIXEL(adr0,x0,y0+=stepy);
          if (extras>2)
           SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
         }
      }
    }
   else
    {
     int c=(dx-dy)<<1;
     int incr1=c<<1;
     int d=incr1+dy;
     for (int i=0; i<length; i++)
      {
       y0+=stepy;
       y1-=stepy;
       if (d>0)
        {
         SET_PIXEL(adr0,x0+=stepx,y0);
         SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
         SET_PIXEL(adr1,x1-=stepy,y1);
         SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
         d+=incr1;
        }
       else
        {
         if (d<c)
          {
           SET_PIXEL(adr0,x0,y0);
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
           SET_PIXEL(adr1,x1,y1);
           SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
          }
         else
          {
           SET_PIXEL(adr0,x0+=stepx,y0);
           SET_PIXEL(adr0,x0,y0+=stepy);
           SET_PIXEL(adr1,x1-=stepx,y1);
           SET_PIXEL(adr1,x1,y1-=stepy);
          }
         d+=incr2;
        }
      }
     if (extras>0)
      {
       if (d>0)
        {
         SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
         if (extras>1)
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
         if (extras>2)
          SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
        }
       else
        if (d<c)
         {
          SET_PIXEL(adr0,x0,y0+=stepy);
          if (extras>1)
           SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>2)
           SET_PIXEL(adr1,x1,y1-=stepy);
         }
        else
         {
          SET_PIXEL(adr0,x0+=stepx,y0+=stepy);
          if (extras>1)
           SET_PIXEL(adr0,x0,y0+=stepy);
          if (extras>2)
           {
            if (d>c)
             SET_PIXEL(adr1,x1-=stepx,y1-=stepy);
            else
             SET_PIXEL(adr1,x1,y1-=stepy);
           }
         }
      }
    }
  }
}
