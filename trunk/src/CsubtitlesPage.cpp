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
#include "CsubtitlesPage.h"
#include "resource.h"
#include <commctrl.h>
#include <string.h>
#include <assert.h>
#include "IffDecoder.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include "colordlg.h"

using namespace std;

static int CALLBACK EnumFamCallBackCharsets(CONST LOGFONT *lpelf,CONST TEXTMETRIC *lpntm,DWORD FontType,LPARAM lParam)
{
 if (FontType&TRUETYPE_FONTTYPE)
  {
   vector<int> *sl=(vector<int>*)lParam;
   vector<int>::iterator i=find(sl->begin(),sl->end(),lpelf->lfCharSet);
   if (i==sl->end())
    sl->push_back(lpelf->lfCharSet);
  }; 
 return 1;
};

void TsubtitlesPage::selectCharset(int ii)
{
 int cnt=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETCOUNT,0,0);
 for (int i=0;i<cnt;i++)
  {
   int iii=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETITEMDATA,i,0);
   if (ii==iii)
    {
     SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_SETCURSEL,i,0);
     return;
    }
  }
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_SETCURSEL,0,0);
 cfgSet(IDFF_fontCharset,SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETITEMDATA,0,0));
}
void TsubtitlesPage::fillCharsets(void)
{
 int oldi=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETCURSEL,0,0);
 int oldii=(oldi!=CB_ERR)?SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETITEMDATA,oldi,0):ANSI_CHARSET;
 
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_RESETCONTENT,0,0);
 vector<int> sl;
 LOGFONT lf;
 deci->getFontName(lf.lfFaceName,LF_FACESIZE);
 lf.lfCharSet=DEFAULT_CHARSET;lf.lfPitchAndFamily=0;
 HDC dc=GetDC(m_hwnd);
 EnumFontFamiliesEx(dc,&lf,EnumFamCallBackCharsets,LPARAM(&sl),0); 
 ReleaseDC(m_hwnd,dc);
 for (unsigned int i=0;i<sl.size();i++)
  {
   char *name;
   switch (sl[i])
    {
     case ANSI_CHARSET       :name="Ansi";break;
     case DEFAULT_CHARSET    :name="Default";break;
     case SYMBOL_CHARSET     :name="Symbol";break;
     case SHIFTJIS_CHARSET   :name="Shiftjis";break;
     case HANGUL_CHARSET     :name="Hangul";break;
     case GB2312_CHARSET     :name="Gb2312";break;
     case CHINESEBIG5_CHARSET:name="Chinese";break;
     case OEM_CHARSET        :name="OEM";break;
     case JOHAB_CHARSET      :name="Johab";break;
     case HEBREW_CHARSET     :name="Hebrew";break;
     case ARABIC_CHARSET     :name="Arabic";break;
     case GREEK_CHARSET      :name="Greek";break;
     case TURKISH_CHARSET    :name="Turkish";break;
     case VIETNAMESE_CHARSET :name="Vietnamese";break;
     case THAI_CHARSET       :name="Thai";break;
     case EASTEUROPE_CHARSET :name="Easteurope";break;
     case RUSSIAN_CHARSET    :name="Russian";break;
     case MAC_CHARSET        :name="Mac";break;
     case BALTIC_CHARSET     :name="Baltic";break;
     default                 :name="unknown";break;
    };
   int ii=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_ADDSTRING,0,LPARAM(name));
   SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_SETITEMDATA,ii,sl[i]);
  };
 selectCharset(oldii);
}
void TsubtitlesPage::font2dlg(void)
{
 setCheck(IDC_CHB_FONT_AUTOSIZE,cfgGet(IDFF_fontAutosize));
 char s[256];int x;
 x=cfgGet(IDFF_fontSize);
 sprintf(s,"Size:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SIZE,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_fontSpacing);
 sprintf(s,"Spacing:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SPACING,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_fontShadow);
 sprintf(s,"Shadow intensity:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SHADOW,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW,TBM_SETPOS,TRUE,x);

 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_SETCURSEL,cfgGet(IDFF_fontWeight)/100-1,0);
 char fontname[256];deci->getFontName(fontname,255);
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_NAME,CB_SELECTSTRING,-1,LPARAM(fontname));
 fillCharsets();
 selectCharset(cfgGet(IDFF_fontCharset));
}
void TsubtitlesPage::sub2dlg(void)
{
 char s[256];int x;
 x=cfgGet(IDFF_subPosX);
 char *posS;
 if (x<40) posS="left";
 else if (x>60) posS="right";
 else posS="center";
 sprintf(s,"Horizontal position:  %3i%% (%s)",x,posS);
 SendDlgItemMessage(m_hwnd,IDC_LBL_SUB_POSX,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETPOS,TRUE,x);
 x=cfgGet(IDFF_subPosY);
 if (x<40) posS="top";
 else if (x>60) posS="bottom";
 else posS="center";
 sprintf(s,"Vertical position:  %3i%% (%s)",x,posS);
 SendDlgItemMessage(m_hwnd,IDC_LBL_SUB_POSY,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETPOS,TRUE,x);
}
void TsubtitlesPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_SUBTITLES,cfgGet(IDFF_isSubtitles));
 font2dlg();
 sub2dlg();
}                  

static int CALLBACK EnumFamCallBackFonts(CONST LOGFONT *lpelf,CONST TEXTMETRIC *lpntm,DWORD FontType,LPARAM lParam)
{
 if (FontType&TRUETYPE_FONTTYPE/* && lpelf->lfCharSet==0*/)
  {
   vector<string> *sl=(vector<string>*)lParam;
   vector<string>::iterator i=find(sl->begin(),sl->end(),lpelf->lfFaceName);
   if (i==sl->end())
    sl->push_back(lpelf->lfFaceName);
  }; 
 return 1;
};

void TsubtitlesPage::createConfig(void)
{
 unsigned int i;
 for (i=0;i<16;i++) custColors[i]=RGB(i*16,i*16,i*16);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETRANGE,TRUE,MAKELPARAM(3,127));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETPAGESIZE,0,6); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETRANGE,TRUE,MAKELPARAM(-10,10));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETPAGESIZE,0,2); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW,TBM_SETPAGESIZE,0,2); 
 
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("thin"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("extralight"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("ultralight"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("light"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("normal"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("regular"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("medium"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("semibold"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("demibold"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("bold"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("extrabold"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("ultrabold"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("heavy"));
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM("black"));

 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_SETPAGESIZE,0,10); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_SETPAGESIZE,0,10); 

 vector<string> sl;
 LOGFONT lf;lf.lfCharSet=DEFAULT_CHARSET;lf.lfPitchAndFamily=0;lf.lfFaceName[0]='\0';
 HDC dc=GetDC(m_hwnd);
 EnumFontFamiliesEx(dc,&lf,EnumFamCallBackFonts,LPARAM(&sl),0); 
 ReleaseDC(m_hwnd,dc);
 for (i=0;i<sl.size();i++)
  SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_NAME,CB_ADDSTRING,0,LPARAM(sl[i].c_str()));
 
 cfg2dlg();
}

HRESULT TsubtitlesPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
  case WM_SHOWWINDOW:
   if (wParam) cfg2dlg();
   break;

  case WM_HSCROLL:
   if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SIZE))
    {
     cfgSet(IDFF_fontSize,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_GETPOS,0,0));
     font2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SPACING))
    {
     cfgSet(IDFF_fontSpacing,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_GETPOS,0,0));
     font2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SHADOW))
    {
     cfgSet(IDFF_fontShadow,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW,TBM_GETPOS,0,0));
     font2dlg();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_SUB_POSX) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_SUB_POSY))
    {
     cfgSet(IDFF_subPosX,SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSX,TBM_GETPOS,0,0));
     cfgSet(IDFF_subPosY,SendDlgItemMessage(m_hwnd,IDC_TBR_SUB_POSY,TBM_GETPOS,0,0));
     sub2dlg();
     return TRUE;
    }
   else break;
    
  case WM_COMMAND:
   switch (LOWORD(wParam))  
    {
     case IDC_CHB_SUBTITLES:
      cfgSet(IDFF_isSubtitles,getCheck(IDC_CHB_SUBTITLES));
      return TRUE; 
     case IDC_CHB_FONT_AUTOSIZE:
      cfgSet(IDFF_fontAutosize,getCheck(IDC_CHB_FONT_AUTOSIZE));
      return TRUE;
     case IDC_CBX_FONT_WEIGHT:
      if (HIWORD(wParam)==CBN_SELCHANGE)
       {
        int i=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_GETCURSEL,0,0);
        cfgSet(IDFF_fontWeight,(i+1)*100);
        return TRUE;
       }
      else
       break;
     case IDC_CBX_FONT_NAME:
      if (HIWORD(wParam)==CBN_SELCHANGE)
       {
        int i=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_NAME,CB_GETCURSEL,0,0);
        char pomS[256];
        SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_NAME,CB_GETLBTEXT,i,LPARAM(pomS));
        deci->setFontName(pomS);
        fillCharsets();
        return TRUE;
       }
      else
       break;
     case IDC_CBX_FONT_CHARSET:
      if (HIWORD(wParam)==CBN_SELCHANGE)
       {
        int i=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETCURSEL,0,0);
        int ii=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETITEMDATA,i,0);
        cfgSet(IDFF_fontCharset,ii);
        return TRUE;
       }
      else break;
     case IDC_IMG_FONT_COLOR :
      if (HIWORD(wParam)==STN_CLICKED)
       {
        CHOOSECOLOR cc;
        memset(&cc,0,sizeof(cc));
        cc.lStructSize=sizeof(cc);
        cc.hwndOwner=m_hwnd;
        cc.rgbResult=cfgGet(IDFF_fontColor);
        //TODO: store custom colors
        cc.lpCustColors=custColors;
        cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT;
        if (ChooseColor(&cc))
         {
          cfgSet(IDFF_fontColor,cc.rgbResult);
          font2dlg();
          InvalidateRect(GetDlgItem(m_hwnd,IDC_IMG_FONT_COLOR),NULL,FALSE);
         }
        return TRUE; 
       }
      else break; 
    };
   break;
  case WM_DRAWITEM:
   if (wParam==IDC_IMG_FONT_COLOR)
    {
     LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
     LOGBRUSH lb;
     lb.lbColor=cfgGet(IDFF_fontColor);
     lb.lbStyle=BS_SOLID;
     HBRUSH br=CreateBrushIndirect(&lb);
     FillRect(dis->hDC,&dis->rcItem,br);
     DeleteObject(br);
     return TRUE;
    };
   break; 
  };
 return FALSE;
};

TsubtitlesPage::TsubtitlesPage(HWND IhwndParent,IffDecoder *Ideci,int dialogId) :TconfPage(IhwndParent,Ideci)
{
 createWindow(dialogId);
}
