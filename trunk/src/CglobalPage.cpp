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
#include "CglobalPage.h"
#include "Cffdshow.h"
#include "resource.h"
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "IffDecoder.h"
#include "Tconfig.h"

void TglobalPage::now2dlg(void)
{
 char pomS[1024];
 char AVIname[1024];
 deci->get_AVIname(AVIname,1023);
 sprintf(pomS,"Now playing: %s",AVIname);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_PLAYING,WM_SETTEXT,0,LPARAM(pomS));
 char AVIfourcc[20];
 deci->get_AVIfourcc(AVIfourcc,19); 
 sprintf(pomS,"FourCC: %s",AVIfourcc);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_CODEC,WM_SETTEXT,0,LPARAM(pomS));
 unsigned int x,y;
 if (deci->get_AVIdimensions(&x,&y)!=S_OK)
  SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_DIMENSIONS,WM_SETTEXT,0,LPARAM("Dimensions:"));
 else
  {
   sprintf(pomS,"Dimensions: %i x %i",x,y);
   SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_DIMENSIONS,WM_SETTEXT,0,LPARAM(pomS));
  } 
 if (deci->get_AVIfps(&x)!=S_OK)
  SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_FPS,WM_SETTEXT,0,LPARAM("FPS:"));
 else
  {
   __asm emms;
   sprintf(pomS,"FPS: %6.2f",float(x/1000.0));
   SendDlgItemMessage(m_hwnd,IDC_LBL_NOW_FPS,WM_SETTEXT,0,LPARAM(pomS));
  } 
}
void TglobalPage::interDlg(void)
{
 char pomS[1024];
 
 setCheck(IDC_CHB_GLOBAL_POSTPROC,cfgGet(IDFF_isPostproc));
 deci->getPostProcDescription(pomS,1023);
 SendDlgItemMessage(m_hwnd,IDC_CHB_GLOBAL_POSTPROC,WM_SETTEXT,0,LPARAM(pomS));
 
 setCheck(IDC_CHB_GLOBAL_PICTPROP,cfgGet(IDFF_isPictProp));
 deci->getPictPropDescription(pomS,1023);
 SendDlgItemMessage(m_hwnd,IDC_CHB_GLOBAL_PICTPROP,WM_SETTEXT,0,LPARAM(pomS));
 
 setCheck(IDC_CHB_GLOBAL_NOISE   ,cfgGet(IDFF_isNoise   ));
 deci->getNoiseDescription(pomS,1023);
 SendDlgItemMessage(m_hwnd,IDC_CHB_GLOBAL_NOISE,WM_SETTEXT,0,LPARAM(pomS));

 setCheck(IDC_CHB_GLOBAL_BLUR    ,cfgGet(IDFF_isBlur    ));
 deci->getBlurDescription(pomS,1023);
 SendDlgItemMessage(m_hwnd,IDC_CHB_GLOBAL_BLUR ,WM_SETTEXT,0,LPARAM(pomS));
 
 setCheck(IDC_CHB_GLOBAL_SHARPEN ,cfgGet(IDFF_isSharpen ));
 deci->getSharpenDescription(pomS,1023);
 SendDlgItemMessage(m_hwnd,IDC_CHB_GLOBAL_SHARPEN,WM_SETTEXT,0,LPARAM(pomS));

 setCheck(IDC_CHB_GLOBAL_CROP,cfgGet(IDFF_isCropNzoom));
 deci->getCropDescription(pomS,1023);
 SendDlgItemMessage(m_hwnd,IDC_CHB_GLOBAL_CROP,WM_SETTEXT,0,LPARAM(pomS));

 setCheck(IDC_CHB_GLOBAL_SUBTITLES,cfgGet(IDFF_isSubtitles));
}
void TglobalPage::cfg2dlg(void)
{
 interDlg();

 setCheck(IDC_CHB_TRAYICON,cfgGet(IDFF_trayIcon));
 
 setCheck(IDC_CHB_XVID,cfgGet(IDFF_xvid));
 setCheck(IDC_CHB_DIV3,cfgGet(IDFF_div3));
 setCheck(IDC_CHB_DIVX,cfgGet(IDFF_divx));
 setCheck(IDC_CHB_DX50,cfgGet(IDFF_dx50));
 setCheck(IDC_CHB_MP43,cfgGet(IDFF_mp43));
 setCheck(IDC_CHB_MP42,cfgGet(IDFF_mp42));
 setCheck(IDC_CHB_H263,cfgGet(IDFF_h263));
 
 setCheck(IDC_CHB_FLIP,cfgGet(IDFF_flip));
 
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_SETCURSEL,cfgGet(IDFF_idct),0);

 setCheck(IDC_CHB_AUTOPRESET,cfgGet(IDFF_autoPreset));
 setCheck(IDC_CHB_AUTOPRESET_FILEFIRST,cfgGet(IDFF_autoPresetFileFirst));
 enableWindow(IDC_CHB_AUTOPRESET_FILEFIRST,getCheck(IDC_CHB_AUTOPRESET));
 char aviname[1024];
 deci->get_AVIname(aviname,1023);
 enableWindow(IDC_BT_PRESET_PUTFLNM,aviname[0]!='\0');
 listPresets();
 
 now2dlg();
}

void TglobalPage::listPresets(void)
{
 //int curPreset=SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_GETCURSEL,0,0);
 SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_RESETCONTENT,0,0);
 unsigned int cnt;
 deci->get_numPresets(&cnt);
 for (unsigned int i=0;i<cnt;i++)
  {
   char pomS[1024];
   deci->get_presetName(i,pomS,1024);
   SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_ADDSTRING,0,LPARAM(pomS));
  }; 
 selectPreset(); 
}

void TglobalPage::selectPreset(void)
{
 char presetName[1024];deci->get_activePresetName(presetName,1023);
 int i=SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_FINDSTRINGEXACT,-1,LPARAM(presetName));
 if (i!=CB_ERR) 
  SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_SETCURSEL,i,0);
}

void TglobalPage::removePreset(void)
{
 char presetName[256];
 SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,WM_GETTEXT,255,LPARAM(presetName));
 bool ok=deci->removePreset(presetName);
 if (ok)
  {
   //int i=SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_FINDSTRINGEXACT,-1,LPARAM(presetName));
   //SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_DELETESTRING,i,0);
   listPresets();
   deci->loadPreset(PRESET_DEFAULT);
   cfg2dlg();
   selectPreset();
  }
}

void TglobalPage::savePreset(void)
{
 parent->applySettings();
 char presetName[256];
 SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,WM_GETTEXT,255,LPARAM(presetName));
 int ok=deci->savePreset(presetName);
 if (!ok) return;
 //int i=SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_FINDSTRINGEXACT,-1,LPARAM(presetName));
 //if (i==CB_ERR) SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_ADDSTRING,0,LPARAM(presetName));
 listPresets();
}

void TglobalPage::loadPreset(void)
{
 char presetName[256];
 if (!cfgGet(IDFF_presetShouldBeSaved)) return;
 SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,WM_GETTEXT,255,LPARAM(presetName));
 int i=SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_FINDSTRINGEXACT,-1,LPARAM(presetName));
 if (i==CB_ERR) return;
 deci->loadPreset(presetName);
 cfg2dlg();
}

void TglobalPage::savePresetToFile(void)
{
 OPENFILENAME ofn;
 memset(&ofn,0,sizeof(ofn));
 char presetFlnm[MAX_PATH];
 char actPresetName[1024];deci->get_activePresetName(actPresetName,1023);
 char actPresetNameName[1024];
 _splitpath(actPresetName,NULL,NULL,actPresetNameName,NULL);
 strcpy(presetFlnm,actPresetNameName);
 ofn.lStructSize    =sizeof(OPENFILENAME);
 ofn.hwndOwner      =m_hwnd;
 ofn.lpstrFilter    ="ffdshow preset (*."FFPRESET")\0*."FFPRESET"\0All files (*.*)\0*.*\0\0";
 ofn.lpstrFile      =presetFlnm;
 ofn.lpstrInitialDir=".";
 ofn.lpstrTitle     ="Save ffdshow preset";
 ofn.lpstrDefExt    =FFPRESET;  
 ofn.nMaxFile       =MAX_PATH;
 ofn.Flags          =OFN_PATHMUSTEXIST|OFN_ENABLESIZING;
 if (GetSaveFileName(&ofn))
  {
   parent->applySettings();
   deci->savePresetToFile(presetFlnm);
  };
}

void TglobalPage::loadPresetFromFile(void)
{
 OPENFILENAME ofn;
 memset(&ofn,0,sizeof(ofn));
 static char presetFlnm[MAX_PATH]="";
 ofn.lStructSize    =sizeof(OPENFILENAME);
 ofn.hwndOwner      =m_hwnd;
 ofn.lpstrFilter    ="ffdshow preset (*."FFPRESET")\0*."FFPRESET"\0All files (*.*)\0*.*\0\0";
 ofn.lpstrInitialDir=".";
 ofn.lpstrFile      =presetFlnm;
 ofn.lpstrTitle     ="Load ffdshow preset";
 ofn.lpstrDefExt    =FFPRESET;  
 ofn.nMaxFile       =MAX_PATH;
 ofn.Flags          =OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ENABLESIZING;
 if (GetOpenFileName(&ofn))
  {
   deci->loadPresetFromFile(presetFlnm);
   deci->savePreset(NULL);
   cfg2dlg();
  } 
}

void TglobalPage::createConfig(void)
{
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_RESETCONTENT,0,0);
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_ADDSTRING,0,LPARAM("simple (16383)"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_ADDSTRING,0,LPARAM("normal"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_ADDSTRING,0,LPARAM("reference"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_ADDSTRING,0,LPARAM("simple (16384)"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_LIMITTEXT,254,0);
 
 cfg2dlg();
}

HRESULT TglobalPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
  //case WM_SHOWWINDOW:
  // if (wParam) cfg2dlg();
  // break;
  case WM_COMMAND:
   switch (LOWORD(wParam))  
    {
    case IDC_CHB_GLOBAL_POSTPROC:
     cfgSet(IDFF_isPostproc,getCheck(IDC_CHB_GLOBAL_POSTPROC));
     return TRUE;
    case IDC_CHB_GLOBAL_PICTPROP:
     cfgSet(IDFF_isPictProp,getCheck(IDC_CHB_GLOBAL_PICTPROP));
     return TRUE;
    case IDC_CHB_GLOBAL_NOISE:
     cfgSet(IDFF_isNoise   ,getCheck(IDC_CHB_GLOBAL_NOISE   ));
     return TRUE;
    case IDC_CHB_GLOBAL_BLUR:
     cfgSet(IDFF_isBlur    ,getCheck(IDC_CHB_GLOBAL_BLUR    ));
     return TRUE;
    case IDC_CHB_GLOBAL_SHARPEN:
     cfgSet(IDFF_isSharpen ,getCheck(IDC_CHB_GLOBAL_SHARPEN ));
     return TRUE;
    case IDC_CHB_GLOBAL_SUBTITLES:
     cfgSet(IDFF_isSubtitles,getCheck(IDC_CHB_GLOBAL_SUBTITLES));
     return TRUE;
    case IDC_CHB_GLOBAL_CROP:
     cfgSet(IDFF_isCropNzoom,getCheck(IDC_CHB_GLOBAL_CROP));
     return TRUE;
    case IDC_CHB_FLIP:
     cfgSet(IDFF_flip,getCheck(IDC_CHB_FLIP));
     return TRUE;
     
    case IDC_CHB_TRAYICON:
     cfgSet(IDFF_trayIcon,getCheck(IDC_CHB_TRAYICON));
     return TRUE;
    
    case IDC_CHB_XVID:
    case IDC_CHB_DIV3:
    case IDC_CHB_DIVX:
    case IDC_CHB_DX50:
    case IDC_CHB_MP43:
    case IDC_CHB_MP42:
    case IDC_CHB_H263:
     cfgSet(IDFF_xvid,getCheck(IDC_CHB_XVID));
     cfgSet(IDFF_div3,getCheck(IDC_CHB_DIV3));
     cfgSet(IDFF_divx,getCheck(IDC_CHB_DIVX));
     cfgSet(IDFF_dx50,getCheck(IDC_CHB_DX50));
     cfgSet(IDFF_mp43,getCheck(IDC_CHB_MP43));
     cfgSet(IDFF_mp42,getCheck(IDC_CHB_MP42));
     cfgSet(IDFF_h263,getCheck(IDC_CHB_H263));
     return TRUE;
     
    case IDC_BT_PRESET_LOAD:
     loadPreset();
     cfgSet(IDFF_autoloadedfromreg,0);
     return TRUE; 
    case IDC_BT_PRESET_SAVE:
     cfgSet(IDFF_presetShouldBeSaved,1);
     savePreset();
     return TRUE; 
    case IDC_BT_PRESET_REMOVE:
     cfgSet(IDFF_presetShouldBeSaved,1);
     removePreset();
     return TRUE; 
    case IDC_BT_PRESET_FILE_SAVE:
     savePresetToFile();
     return TRUE;
    case IDC_BT_PRESET_FILE_LOAD:
     loadPresetFromFile();
     return TRUE; 
    case IDC_BT_PRESET_PUTFLNM:
     {
      char aviname[1024],aviname2[1024];
      deci->get_AVIname(aviname,1023);
      Tconfig::normalizeName(aviname2,aviname);
      SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,WM_SETTEXT,0,LPARAM(aviname2));
      return TRUE;
     };
    case IDC_CHB_AUTOPRESET:
     cfgSet(IDFF_autoPreset,getCheck(IDC_CHB_AUTOPRESET));
     enableWindow(IDC_CHB_AUTOPRESET_FILEFIRST,getCheck(IDC_CHB_AUTOPRESET));
     return TRUE;
    case IDC_CHB_AUTOPRESET_FILEFIRST:
     cfgSet(IDFF_autoPresetFileFirst,getCheck(IDC_CHB_AUTOPRESET_FILEFIRST));
     return TRUE;
    case IDC_CBX_IDCT:
     if (HIWORD(wParam)==CBN_SELCHANGE)
      {
       int i=SendDlgItemMessage(m_hwnd,IDC_CBX_IDCT,CB_GETCURSEL,0,0);
       cfgSet(IDFF_idct,i);
       return TRUE;
      }
     else
      break;
    }; 
   break; 
  };
 return FALSE;
};

TglobalPage::TglobalPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci,int dialogId) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(dialogId);
}
