/*
 * Copyright (c) 2002 Milan Cutka
 * getSubtitle function uses code from mplayer
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

#include <windows.h>
#include "Tsubtitles.h"

Tsubtitles::Tsubtitles(void)
{
 subs=NULL;
 flnm[0]='\0';
};
Tsubtitles::~Tsubtitles()
{
 done();
}

void Tsubtitles::init(const char *aviFlnm,const char *subFlnm,float Ifps)
{
 done();
 if (aviFlnm[0]!='\0')
  {
   char dsk[256],dir[256],name[256],ext[256];
   _splitpath(aviFlnm,dsk,dir,name,ext);
   char path[256],fname[256];
   _makepath(path,dsk,dir,NULL,NULL);
   _makepath(fname,NULL,NULL,name,ext);
   char *subFlnm=sub_filename(path,fname);
   if (subFlnm)
    {
     __asm emms;
     subs=sub_read_file(strcpy(flnm,subFlnm?subFlnm:aviFlnm),Ifps);
     free(subFlnm);
    } 
  }
 else  
  subs=sub_read_file(strcpy(flnm,subFlnm),Ifps);
 oldsub=NULL;
 current_sub=0;
 nosub_range_start=-1;
 nosub_range_end=-1;
}
void Tsubtitles::done(void)
{
 if (subs) sub_free(subs);subs=NULL;
}
subtitle* Tsubtitles::getSubtitle(unsigned long frame)
{
 if (!subs) return NULL;

 if (oldsub)
  {
   if(frame>=oldsub->start && frame<=oldsub->end)
    return oldsub; // OK!
  }
 else 
  {
   if(frame>nosub_range_start && frame<nosub_range_end) 
    return oldsub; // OK!
  };
 // sub changed!
 if (frame<=0)
  return oldsub=NULL; // no sub here
 // check next sub.
 if(current_sub>=0 && current_sub+1<sub_num)
  {
   if (frame>subs[current_sub].end && frame<subs[current_sub+1].start)
    {
     nosub_range_start=subs[current_sub].end;
     nosub_range_end  =subs[current_sub+1].start;
     return oldsub=NULL;
     // next sub?
     ++current_sub;
     oldsub=&subs[current_sub];
     if (frame>=oldsub->start && frame<=oldsub->end) return oldsub; // OK!
    }
  }
  // use logarithmic search:
 int i=0,j=sub_num-1;
 while (j>=i)
  {
   current_sub=(i+j+1)/2;
   oldsub=&subs[current_sub];
   if (frame<oldsub->start) j=current_sub-1;
   else if(frame>oldsub->end) i=current_sub+1;
   else return oldsub; // found!
  }
 // check where are we...
 if (frame<oldsub->start)
  {
   if(current_sub<=0)
    {
     // before the first sub
     nosub_range_start=frame-1; // tricky
     nosub_range_end  =oldsub->start;
     return oldsub=NULL;
    }
   --current_sub;
   if (frame>subs[current_sub].end && frame<subs[current_sub+1].start)
    {
     // no sub
     nosub_range_start=subs[current_sub].end;
     nosub_range_end  =subs[current_sub+1].start;
     return oldsub=NULL;
    }
  }
 else
  {
   if (frame<=oldsub->end) /*printf("JAJJ!  ")*/; 
   else
    if (current_sub+1>=sub_num)
     {
      // at the end?
      nosub_range_start=oldsub->end;
      nosub_range_end=0x7FFFFFFF; // MAXINT
      return oldsub=NULL;
     }
    else
     if (frame>subs[current_sub].end && frame<subs[current_sub+1].start)
      {
       // no sub
       nosub_range_start=subs[current_sub].end;
       nosub_range_end  =subs[current_sub+1].start;
       return oldsub=NULL;
      }
  }
 return oldsub=NULL;
}
