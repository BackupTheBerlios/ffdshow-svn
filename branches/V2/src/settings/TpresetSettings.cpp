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

#include "stdafx.h"
#include "reg.h"
#include "TpresetSettings.h"
#include "ffdebug.h"
#include <algorithm>
#include "IffDecoder.h"
#include "Tfilter.h"

using namespace std;

TpresetSettings::TpresetSettings(IffDecoder *deci,const char *IpresetName) 
{
 strcpy(settings.presetName,IpresetName);
 settings.autoLoadedFromFile=0;
 initFilters(deci);
}
TpresetSettings::TpresetSettings(TpresetSettings &orig)
{
 doneFilters();
 settings=orig.settings;
 for (vector<Tfilter*>::iterator i=orig.filters.begin();i!=orig.filters.end();i++)
  filters.push_back((*i)->duplicate());
}
TpresetSettings::~TpresetSettings()
{
 doneFilters();
}
void TpresetSettings::initFilters(IffDecoder *deci)
{
 doneFilters();
 Tfilter::TcreateFcVector *cfcs;deci->getCfcs((void**)&cfcs);
 for (Tfilter::TcreateFcVector::iterator i=cfcs->begin();i!=cfcs->end();i++)
  filters.push_back((*i)());
 sortOrder();
}
void TpresetSettings::doneFilters(void)
{
 for (vector<Tfilter*>::iterator i=filters.begin();i!=filters.end();i++)
  delete *i;
 filters.clear(); 
}

bool TpresetSettings::isValidPresetName(const char *presetName)
{
 if (presetName[0]=='\0') return false;
 for (unsigned int i=0;i<strlen(presetName);i++)
  if (presetName[i]=='*' || presetName[i]=='?') return false;
 return true; 
}

void TpresetSettings::loadDefault(void)
{
 strcpy(settings.presetName,FFPRESET_DEFAULT);
 loadReg();
}
void TpresetSettings::loadReg(void)
{
 char presetRegStr[256];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",settings.presetName);
 TregOpRegRead t(HKEY_CURRENT_USER,presetRegStr);
 reg_op(t);
 sortOrder();
}
void TpresetSettings::saveReg(void)
{
 char presetRegStr[256];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",settings.presetName);
 TregOpRegWrite t(HKEY_CURRENT_USER,presetRegStr);
 reg_op(t);
}
void TpresetSettings::loadFile(const char *flnm)
{
 char sections[4096]="";
 GetPrivateProfileSectionNames(sections,4095,flnm);
 if (sections[0]=='\0') return;
 _splitpath(flnm,NULL,NULL,settings.presetName,NULL);
 TregOpFileRead t(flnm,sections);
 reg_op(t);
 sortOrder();
}
void TpresetSettings::saveFile(const char *flnm)
{
 DeleteFile(flnm);
 TregOpFileWrite t(flnm,"presetSettings");
 reg_op(t);
}
void TpresetSettings::reg_op(TregOp &t)
{
 t._REG_OP_N("flip",settings.flip,0);
 t._REG_OP_N("idct",settings.idct,0);
 for (unsigned int i=0;i<filters.size();i++)
  filters[i]->reg_op(t);
}

static bool filterOrderCompare(Tfilter *fs1,Tfilter *fs2)
{
 return fs1->order<fs2->order;
}
void TpresetSettings::sortOrder(void)
{
 sort(filters.begin(),filters.end(),filterOrderCompare);
}
Tfilter* TpresetSettings::getFilter(int filterID)
{
 for (vector<Tfilter*>::iterator i=filters.begin();i!=filters.end();i++)
  if ((*i)->getID()==filterID)
   return *i;
 return NULL;  
}
