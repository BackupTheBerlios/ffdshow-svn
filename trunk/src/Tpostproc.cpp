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
#include "Tpostproc.h"
#include <math.h>
#include "TpresetSettings.h"
#include <time.h>
#include "xvid\utils\mem_align.h"

extern "C" void modifyPPmode(PPMode *pp,Tconfig *cfg);
void modifyPPmode(PPMode *pp,Tconfig *cfg)
{
 
}

void Tpostproc::init(void)
{
 //cfg->ppsetting=1;
 postproc_dll=new Tdll("C:\\mydocuments\\ffdshow\\src\\mplayer\\libpostproc.dll");
 postproc_dll->loadFunction((void**)&init_mplayer,"init_mplayer");
 postproc_dll->loadFunction((void**)&postprocess,"postprocess");
 postproc_dll->loadFunction((void**)&getPpModeForQuality,"getPpModeForQuality");
 postproc_dll->loadFunction((void**)&setModifyPPmode,"setModifyPPmode");
 postproc_dll->loadFunction((void**)&freeSwsContext,"freeSwsContext");
 postproc_dll->loadFunction((void**)&getSwsContextFromCmdLine,"getSwsContextFromCmdLine");
 postproc_dll->loadFunction((void**)&set_sws_params,"set_sws_params");
 postproc_dll->loadFunction((void**)&rgb24toyv12,"rgb24toyv12");
 postproc_dll->loadFunction((void**)&yuy2toyv12,"yuy2toyv12");
 if (postproc_dll->ok)
  { 
   init_mplayer();
   //setModifyPPmode(/*modifyPPmode*/NULL,cfg);
   ok=true;
  }
 else
  ok=false; 
};

void Tpostproc::done(void)
{
 if (postproc_dll)
  {
   delete postproc_dll;
   postproc_dll=NULL;
  }; 
 init_mplayer=NULL;
 postprocess=NULL;
 getPpModeForQuality=NULL;
 setModifyPPmode=NULL;
 ok=false;
};

int Tpostproc::getPPmode(const TpresetSettings *cfg)
{
 int result=0;
 if (!cfg->ppIsCustom)
  {
   int ppqual=cfg->autoq?cfg->currentq:cfg->ppqual;
   if (ppqual<0) ppqual=0;
   if (ppqual>GET_PP_QUALITY_MAX) ppqual=GET_PP_QUALITY_MAX;
   static const int ppPresets[1+GET_PP_QUALITY_MAX]=
    {
     0,
	   LUM_H_DEBLOCK,
	   LUM_H_DEBLOCK | LUM_V_DEBLOCK,
	   LUM_H_DEBLOCK | LUM_V_DEBLOCK | CHROM_H_DEBLOCK,
	   LUM_H_DEBLOCK | LUM_V_DEBLOCK | CHROM_H_DEBLOCK | CHROM_V_DEBLOCK,
	   LUM_H_DEBLOCK | LUM_V_DEBLOCK | CHROM_H_DEBLOCK | CHROM_V_DEBLOCK | LUM_DERING,
	   LUM_H_DEBLOCK | LUM_V_DEBLOCK | CHROM_H_DEBLOCK | CHROM_V_DEBLOCK | LUM_DERING | CHROM_DERING
    };
   result=ppPresets[ppqual];
  }
 else
  result=cfg->ppcustom;  
 if (cfg->levelFixLum) result|=LUM_LEVEL_FIX;
 if (cfg->levelFixChrom) result|=CHROM_LEVEL_FIX;
 return result;
}
