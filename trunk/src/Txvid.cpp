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

#include <windows.h>
#include "Txvid.h"

#define XVID_PATH "C:\\mydocuments\\ffdshow\\src\\ffmpeg\\xvid\\xvid.dll"

void Txvid::init(void)
{
 inited=true;
 xvid_dll=new Tdll(XVID_PATH);
 if (xvid_dll->ok)
  {
   xvid_dll->loadFunction((void**)&xvid_init,"xvid_init");
   xvid_dll->loadFunction((void**)&xvid_decore,"xvid_decore");
   ok=true;
  }
 else
  ok=false; 
}
void Txvid::done(void)
{
 if (xvid_dll)
  {
   delete xvid_dll;
   xvid_dll=NULL;
   xvid_init=NULL;
   xvid_decore=NULL;
  }; 
 ok=false;
}
