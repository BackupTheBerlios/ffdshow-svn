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
#include "IffDecoder.h"
#include "TffdshowPage.h"
#include "Cpresets.h"
#include "resource.h"
#include "TpresetSettings.h"
#include "ffdebug.h"

static LRESULT CALLBACK lvWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) 
{ 
 TpresetsPage *presetsPage=(TpresetsPage*)GetWindowLong(hwnd,GWL_USERDATA);
 switch (msg) 
  { 
   case WM_KEYDOWN: 
    switch (wParam) 
     { 
      case VK_F2: 
       ListView_EditLabel(presetsPage->hlv,ListView_GetNextItem(presetsPage->hlv,-1,LVNI_SELECTED)); 
       return 0; 
     } 
    break; 
   case WM_KEYUP: 
   case WM_CHAR: 
    switch (wParam) 
     { 
      case VK_F2:return 0; 
     } 
    break; 
  } 
 return CallWindowProc(presetsPage->lvOldWndProc,hwnd,msg,wParam,lParam); 
} 
void TpresetsPage::init(void)
{
 hlv=GetDlgItem(m_hwnd,IDC_LV_PRESETS); 
 SetWindowLong(hlv,GWL_USERDATA,LONG(this));
 lvOldWndProc=(WNDPROC)SetWindowLong(hlv,GWL_WNDPROC,LONG(lvWndProc));

 italicFont=boldFont=NULL;
 ncol=0;fileDlgFlnm[0]='\0';

 deci->getPresets(&localPresets);

 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
 addCol(cfgGet(IDFF_lvWidth0),"preset name",false);
 ListView_SetItemCountEx(hlv,localPresets.size(),0);
 deci->getActivePresetName(oldActivePresetName,260);
 lvSelectPreset(oldActivePresetName);
 cfg2dlg();
}
void TpresetsPage::lvSelectPreset(const char *presetName)
{
 for (unsigned int i=0;i<localPresets.size();i++)
  if (_stricmp(presetName,localPresets[i]->presetName)==0)
   {
    ListView_SetItemState(hlv,i,LVIS_SELECTED,LVIS_SELECTED);
    deci->setPresetPtr(localPresets[i]);
    parent->presetChanged();
    enableWindow(IDC_BT_PRESET_REMOVE,i);
    ListView_EnsureVisible(hlv,i,FALSE);
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
}

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
   case WM_DESTROY:
    cfgSet(IDFF_lvWidth0,ListView_GetColumnWidth(hlv,0));
    DeleteObject(italicFont);
    DeleteObject(boldFont);
    return FALSE;
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
      case IDC_BT_PRESET_NEW:
      case IDC_BT_PRESET_NEW_MENU:
       {
        int menuCmd=0;
        if (LOWORD(wParam)==IDC_BT_PRESET_NEW)
         menuCmd=ID_MNI_PRESET_NEWFROMDEFAULT;
        else
         {
          HMENU hmn=LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_PRESET)),hmn2=GetSubMenu(hmn,0) ;
          RECT r;
          GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_PRESET_NEW_MENU),&r);
          menuCmd=TrackPopupMenu(hmn2,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,m_hwnd,0);
          DestroyMenu(hmn);
         } 
        TpresetSettings *newPreset=NULL;
        switch (menuCmd)
         {
          case ID_MNI_PRESET_NEWFROMDEFAULT:
           {
            newPreset=new TpresetSettings(*localPresets[0]);
            break;
           }
          case ID_MNI_PRESET_NEWFROMSELECTED:
           {
            newPreset=new TpresetSettings(*localPresets[ListView_GetNextItem(hlv,-1,LVNI_SELECTED)]);
            break;
           }
          case ID_MNI_PRESET_NEW_FROMFILE:
           {
            OPENFILENAME ofn;
            memset(&ofn,0,sizeof(ofn));
            ofn.lStructSize    =sizeof(OPENFILENAME);
            ofn.hwndOwner      =m_hwnd;
            ofn.lpstrFilter    ="ffdshow preset (*."FFPRESET_EXT")\0*."FFPRESET_EXT"\0All files (*.*)\0*.*\0\0";
            ofn.lpstrInitialDir=".";
            ofn.lpstrFile      =fileDlgFlnm;
            ofn.lpstrTitle     ="Load ffdshow preset";
            ofn.lpstrDefExt    =FFPRESET_EXT;  
            ofn.nMaxFile       =MAX_PATH;
            ofn.Flags          =OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ENABLESIZING;
            if (GetOpenFileName(&ofn))
             {
              newPreset=new TpresetSettings;
              newPreset->loadFile(fileDlgFlnm);
             }
            break; 
           } 
         }
        if (newPreset)
         {
          localPresets.nextUniqueName(newPreset);
          localPresets.storePreset(newPreset); 
          ListView_SetItemCountEx(hlv,localPresets.size(),0);
          lvSelectPreset(newPreset->presetName);
          //SetActiveWindow(hlv);
          //PostMessage(hlv,LVM_EDITLABEL,ListView_GetNextItem(hlv,-1,LVNI_SELECTED),0); 
         } 
        return TRUE;
       }
      case IDC_BT_PRESET_READFROMFILE:
       {
        int i=ListView_GetNextItem(hlv,-1,LVNI_SELECTED);
        char presetFlnm[MAX_PATH];
        _splitpath(localPresets[i]->presetName,NULL,NULL,presetFlnm,NULL);
        OPENFILENAME ofn;
        memset(&ofn,0,sizeof(ofn));
        ofn.lStructSize    =sizeof(OPENFILENAME);
        ofn.hwndOwner      =m_hwnd;
        ofn.lpstrFilter    ="ffdshow preset (*."FFPRESET_EXT")\0*."FFPRESET_EXT"\0All files (*.*)\0*.*\0\0";
        ofn.lpstrInitialDir=".";
        ofn.lpstrFile      =presetFlnm;
        ofn.lpstrTitle     ="Load ffdshow preset";
        ofn.lpstrDefExt    =FFPRESET_EXT;  
        ofn.nMaxFile       =MAX_PATH;
        ofn.Flags          =OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ENABLESIZING;
        if (GetOpenFileName(&ofn))
         {
          char presetName[260];strcpy(presetName,localPresets[i]->presetName);
          localPresets[i]->loadFile(presetFlnm);
          strcpy(localPresets[i]->presetName,presetName);
          lvSelectPreset(presetName);
         }
        return TRUE; 
       } 
      case IDC_BT_PRESET_SAVETOFILE:
       {
        int i=ListView_GetNextItem(hlv,-1,LVNI_SELECTED);
        char presetFlnm[MAX_PATH];
        _splitpath(localPresets[i]->presetName,NULL,NULL,presetFlnm,NULL);
        OPENFILENAME ofn;
        memset(&ofn,0,sizeof(ofn));
        ofn.lStructSize    =sizeof(OPENFILENAME);
        ofn.hwndOwner      =m_hwnd;
        ofn.lpstrFilter    ="ffdshow preset (*."FFPRESET_EXT")\0*."FFPRESET_EXT"\0All files (*.*)\0*.*\0\0";
        ofn.lpstrFile      =presetFlnm;
        ofn.lpstrInitialDir=".";
        ofn.lpstrTitle     ="Save ffdshow preset";
        ofn.lpstrDefExt    =FFPRESET_EXT;  
        ofn.nMaxFile       =MAX_PATH;
        ofn.Flags          =OFN_PATHMUSTEXIST|OFN_ENABLESIZING;
        if (GetSaveFileName(&ofn))
         localPresets[i]->saveFile(presetFlnm);
        return TRUE; 
       }
      case IDC_BT_PRESET_REMOVE:
       {
        int i=ListView_GetNextItem(hlv,-1,LVNI_SELECTED);
        if (i!=0 && MessageBox(m_hwnd,"Do you really want to remove selected preset?","Removing preset",MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES)
         {
          char presetName[1024];
          ListView_GetItemText(hlv,i,0,presetName,1023);
          localPresets.removePreset(presetName);
          if (deci->isDefaultPreset(presetName)==S_OK)
           deci->setDefaultPresetName(localPresets[0]->presetName);
          ListView_SetItemCountEx(hlv,localPresets.size(),0);
          ListView_GetItemText(hlv,0,0,presetName,1023);
          lvSelectPreset(presetName);
         }
        return TRUE; 
       }
      case IDC_BT_PRESET_RENAME:
      case IDC_BT_PRESET_RENAME_MENU:
       {
        int menuCmd=0;
        char AVIname[260];deci->getAVIname(AVIname,260);
        if (LOWORD(wParam)==IDC_BT_PRESET_RENAME)
         menuCmd=ID_MNI_PRESET_RENAME;
        else
         {
          HMENU hmn=LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_PRESET)),hmn2=GetSubMenu(hmn,1);
          RECT r;
          GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_PRESET_RENAME_MENU),&r);
          if (AVIname[0]=='\0')
           EnableMenuItem(hmn2,1,MF_BYPOSITION|MF_GRAYED);
          menuCmd=TrackPopupMenu(hmn2,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,m_hwnd,0);
          DestroyMenu(hmn);
         } 
        int i=ListView_GetNextItem(hlv,-1,LVNI_SELECTED); 
        if (menuCmd==ID_MNI_PRESET_RENAMETOFILE)
         {
          char presetName[260];TpresetSettings::normalizePresetName(presetName,AVIname);
          localPresets.nextUniqueName(presetName);
          deci->renameActivePreset(presetName);
          parent->presetChanged();
          InvalidateRect(hlv,NULL,false);
         } 
        else if (menuCmd==ID_MNI_PRESET_RENAME)
         { 
          SetFocus(hlv);
          ListView_EditLabel(hlv,i);
         }
        return TRUE;
       }
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
           strcpy(nmdi->item.pszText,localPresets[i]->presetName);
          return TRUE;
         }
        case LVN_ITEMCHANGED:
         {
          NMLISTVIEW *nmlv=LPNMLISTVIEW(lParam);
          if (nmlv->iItem==-1) return TRUE;
          DEBUGS1("activate",nmlv->iItem);
          char presetName[1024];
          ListView_GetItemText(hlv,nmlv->iItem,0,presetName,1023);
          parent->applying=true;
          lvSelectPreset(presetName);
          parent->applying=false;
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
            parent->applying=true;
            lvSelectPreset(activePresetName);
            parent->applying=false;
           }
          else if (nmhdr->code==NM_DBLCLK)
           {
            if (cfgGet(IDFF_autoLoadedFromFile)==1)
             {
              cfgSet(IDFF_autoLoadedFromFile,0);
              InvalidateRect(hlv,NULL,false);
             }
            else 
             {
              char presetName[260];
              deci->getActivePresetName(presetName,260);
              deci->setDefaultPresetName(presetName);
              InvalidateRect(hlv,NULL,false);
             }  
           }
          return TRUE;
         }
        case LVN_ENDLABELEDIT:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)(lParam);
          if (!nmdi->item.pszText) return FALSE;
          if (localPresets.getPreset(nmdi->item.pszText)==NULL && TpresetSettings::isValidPresetName(nmdi->item.pszText))
           {
            char presetName[260];
            TpresetSettings::normalizePresetName(presetName,nmdi->item.pszText);
            deci->renameActivePreset(presetName);
            SetWindowLong(m_hwnd,DWL_MSGRESULT,TRUE);
            parent->presetChanged();
           }
          else
           SetWindowLong(m_hwnd,DWL_MSGRESULT,FALSE);
         } 
        case NM_CUSTOMDRAW:
         {
          NMLVCUSTOMDRAW *lvcd=LPNMLVCUSTOMDRAW(lParam);
          if (lvcd->nmcd.dwDrawStage==CDDS_PREPAINT)
           {
            SetWindowLong(m_hwnd,DWL_MSGRESULT,CDRF_NOTIFYITEMDRAW);
            return TRUE;
           } 
          if (lvcd->nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
           {
            //TconfPage *page=(TconfPage*)tvcd->nmcd.lItemlParam;
            //if (page->getInter()==-1) return FALSE;
            if (!italicFont)
             {
              LOGFONT oldFont;
              HFONT hf=(HFONT)GetCurrentObject(lvcd->nmcd.hdc,OBJ_FONT);
              GetObject(hf,sizeof(LOGFONT),&oldFont);
              oldFont.lfItalic=TRUE;
              italicFont=CreateFontIndirect(&oldFont);
              oldFont.lfItalic=FALSE;oldFont.lfWeight=FW_BLACK;
              boldFont=CreateFontIndirect(&oldFont);
             }
            TpresetSettings *preset=localPresets[lvcd->nmcd.dwItemSpec];
            if (preset->autoLoadedFromFile) 
             SelectObject(lvcd->nmcd.hdc,italicFont);
            char defaultPreset[260];
            deci->getDefaultPresetName(defaultPreset,260); 
            if (_stricmp(defaultPreset,preset->presetName)==0)
             SelectObject(lvcd->nmcd.hdc,boldFont);
            SetWindowLong(m_hwnd,DWL_MSGRESULT,/*CDRF_NOTIFYPOSTPAINT*/CDRF_NEWFONT);
            return TRUE;
           }
          return TRUE;
         }
       }
     break; 
    }
  }
 return FALSE;
}
void TpresetsPage::applySettings(void)
{
 deci->setPresets(&localPresets);
 deci->savePresets();
 strcpy(oldActivePresetName,localPresets[ListView_GetNextItem(hlv,-1,LVNI_SELECTED)]->presetName);
}
TpresetsPage::~TpresetsPage()
{
 deci->setActivePreset(oldActivePresetName);
}

TpresetsPage::TpresetsPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_PRESETS);
}
