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
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#pragma hdrstop
#include "Cpresets.h"
#include "resource.h"
#include "IffDecoder.h"
#include "TpresetSettings.h"

void TpresetsPage::init(void)
{
 ncol=0;
 unsigned int len;deci->getNumPresets(&len);
 for (unsigned int i=0;i<len;i++) 
  {
   TpresetSettings preset;
   deci->getPreset(i,&preset);
   localPresets.storePreset(preset);
  }
 hlv=GetDlgItem(m_hwnd,IDC_LV_PRESETS); 
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
 addCol(300,"preset name",false);
 ListView_SetItemCountEx(hlv,localPresets.size(),0);
 cfg2dlg();
}

void TpresetsPage::addCol(int w,const char *txt,bool right)
{
 LVCOLUMN lvc;
 lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM; 
 lvc.iSubItem=ncol;
 lvc.pszText=LPSTR(txt);    
 lvc.cx=w; 
 lvc.fmt=(right)?LVCFMT_RIGHT:LVCFMT_LEFT;
 ListView_InsertColumn(hlv,ncol,&lvc);
 ncol++;
};

void TpresetsPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_AUTOPRESET,cfgGet(IDFF_autoPreset));
 setCheck(IDC_CHB_AUTOPRESET_FILEFIRST,cfgGet(IDFF_autoPresetFileFirst));
 enableWindow(IDC_CHB_AUTOPRESET_FILEFIRST,getCheck(IDC_CHB_AUTOPRESET));
}

HRESULT TpresetsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_AUTOPRESET:
       cfgSet(IDFF_autoPreset,getCheck(IDC_CHB_AUTOPRESET));
       enableWindow(IDC_CHB_AUTOPRESET_FILEFIRST,getCheck(IDC_CHB_AUTOPRESET));
       return TRUE;
      case IDC_CHB_AUTOPRESET_FILEFIRST:
       cfgSet(IDFF_autoPresetFileFirst,getCheck(IDC_CHB_AUTOPRESET_FILEFIRST));
       return TRUE;
     }
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_PRESETS)
      switch (nmhdr->code)
       {
        case LVN_GETDISPINFO:
         {
          NMLVDISPINFO *di=(NMLVDISPINFO*)(lParam);
          int i=di->item.iItem;
          if (i==-1) break;
          //if (di->item.mask&LVIF_STATE) di->item.state|=0;
          //if (di->item.mask&LVIF_IMAGE) di->item.iImage=items[i]->getImageIndex();
          if (di->item.mask&LVIF_TEXT)
           {
            strcpy(di->item.pszText,localPresets[i].presetName);
           }
         };
        break;
       }
     break; 
    } 
  };
 return FALSE;
}

void TpresetsPage::getTip(char *tipS,int len)
{
 sprintf(tipS,"preset name");
}

TpresetsPage::TpresetsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_PRESETS);
}
