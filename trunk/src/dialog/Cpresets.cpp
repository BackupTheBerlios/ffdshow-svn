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
#include "IffDecoder.h"
#include "TffdshowPage.h"
#include "Cpresets.h"
#include "resource.h"
#include "TpresetSettings.h"
#include "ffdebug.h"

#define ID_TOOLBAR 1998

void TpresetsPage::init(void)
{
 hlv=GetDlgItem(m_hwnd,IDC_LV_PRESETS); 
 ncol=0;

 unsigned int len;deci->getNumPresets(&len);
 for (unsigned int i=0;i<len;i++) 
  {
   TpresetSettings *preset;
   deci->getPreset(i,&preset);
   localPresets.storePreset(new TpresetSettings(*preset));
  }
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
 addCol(300,"preset name",false);
 ListView_SetItemCountEx(hlv,localPresets.size(),0);
 deci->getActivePresetName(oldActivePresetName,260);
 lvSelectPreset(oldActivePresetName);
 cfg2dlg();
}
void TpresetsPage::lvSelectPreset(const char *presetName)
{
 for (int i=0;i<localPresets.size();i++)
  if (_stricmp(presetName,localPresets[i]->presetName)==0)
   {
    ListView_SetItemState(hlv,i,LVIS_SELECTED,LVIS_SELECTED);
    deci->setPreset(localPresets[i]);
    parent->presetChanged();
    return;
   }
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
      case IDC_BT_PRESET_NEW_MENU:
       HMENU hmn=LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_PRESET_NEW)),hmn2=GetSubMenu(hmn,0) ;
       POINT p;GetCursorPos(&p);
       int cmd=TrackPopupMenu(hmn2,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,p.x,p.y+2,0,m_hwnd,0);
       DestroyMenu(hmn);
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
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)(lParam);
          int i=nmdi->item.iItem;
          if (i==-1) break;
          //if (di->item.mask&LVIF_STATE) di->item.state|=0;
          //if (di->item.mask&LVIF_IMAGE) di->item.iImage=items[i]->getImageIndex();
          if (nmdi->item.mask&LVIF_TEXT)
           {
            strcpy(nmdi->item.pszText,localPresets[i]->presetName);
           }
          return TRUE;
         };
        case LVN_ITEMCHANGED:
         {
          NMLISTVIEW *nmlv=LPNMLISTVIEW(lParam);
          if (nmlv->iItem==-1) return TRUE;
          DEBUGS1("activate",nmlv->iItem);
          char presetName[1024];
          ListView_GetItemText(hlv,nmlv->iItem,0,presetName,1023);
          deci->setPreset(localPresets.getPreset(presetName));
          parent->presetChanged();
          return TRUE;
         }
        case NM_DBLCLK:
        case NM_CLICK:
         {
          NMITEMACTIVATE *nmia=LPNMITEMACTIVATE(lParam);
          if (nmia->iItem==-1) 
           {
            //ListView_SetItemState(hlv,0,LVIS_SELECTED,LVIS_SELECTED);
            char activePresetName[260];
            deci->getActivePresetName(activePresetName,260);
            //DEBUGS1(activePresetName,nmia->iItem);
            lvSelectPreset(activePresetName);
           }
          return TRUE;
         }
       }
     break; 
    }
  };
 return FALSE;
}

void TpresetsPage::applySettings(void)
{
 strcpy(oldActivePresetName,localPresets[ListView_GetNextItem(hlv,-1,LVNI_SELECTED)]->presetName);
}

TpresetsPage::~TpresetsPage()
{
 deci->loadPreset(oldActivePresetName);
}

TpresetsPage::TpresetsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_PRESETS);
}




