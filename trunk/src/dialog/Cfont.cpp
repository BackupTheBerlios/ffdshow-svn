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
#include "Cfont.h"
#include "resource.h"
#include "IffDecoder.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

const char *TfontPage::fontWeights[]=
{
 "thin",
 "extralight",
 "light",
 "normal",
 "medium",
 "semibold",
 "bold",
 "extrabold",
 "heavy",
};

static int CALLBACK EnumFamCallBackCharsets(CONST LOGFONT *lpelf,CONST TEXTMETRIC *lpntm,DWORD FontType,LPARAM lParam)
{
 if (FontType&TRUETYPE_FONTTYPE)
  {
   vector<int> *sl=(vector<int>*)lParam;
   vector<int>::iterator i=find(sl->begin(),sl->end(),lpelf->lfCharSet);
   if (i==sl->end())
    sl->push_back(lpelf->lfCharSet);
  }
 return 1;
}

static int CALLBACK EnumFamCallBackFonts(CONST LOGFONT *lpelf,CONST TEXTMETRIC *lpntm,DWORD FontType,LPARAM lParam)
{
 if (FontType&TRUETYPE_FONTTYPE/* && lpelf->lfCharSet==0*/)
  {
   vector<string> *sl=(vector<string>*)lParam;
   vector<string>::iterator i=find(sl->begin(),sl->end(),lpelf->lfFaceName);
   if (i==sl->end())
    sl->push_back(lpelf->lfFaceName);
  }
 return 1;
}

void TfontPage::init(void)
{
 unsigned int i;
 for (i=0;i<16;i++) custColors[i]=RGB(i*16,i*16,i*16);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETRANGE,TRUE,MAKELPARAM(3,127));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETPAGESIZE,0,6); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETRANGE,TRUE,MAKELPARAM(-10,10));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETPAGESIZE,0,2); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_STRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(0,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_STRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_STRENGTH,TBM_SETPAGESIZE,0,10); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_RADIUS,TBM_SETRANGE,TRUE,MAKELPARAM(1,100));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_RADIUS,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_RADIUS,TBM_SETPAGESIZE,0,10); 
 
 for (i=0;i<sizeof(fontWeights)/sizeof(char*);i++)
  SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_ADDSTRING,0,LPARAM(fontWeights[i]));
 vector<string> sl;
 LOGFONT lf;lf.lfCharSet=DEFAULT_CHARSET;lf.lfPitchAndFamily=0;lf.lfFaceName[0]='\0';
 HDC dc=GetDC(m_hwnd);
 EnumFontFamiliesEx(dc,&lf,EnumFamCallBackFonts,LPARAM(&sl),0); 
 ReleaseDC(m_hwnd,dc);
 for (i=0;i<sl.size();i++)
  SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_NAME,CB_ADDSTRING,0,LPARAM(sl[i].c_str()));
 
 cfg2dlg();
}

void TfontPage::selectCharset(int ii)
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
 cfgSet(osd?IDFF_OSDfontCharset:IDFF_fontCharset,SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETITEMDATA,0,0));
}

void TfontPage::getCharset(int i,char **name)
{
 switch (i)
  {
   case ANSI_CHARSET       :*name="Ansi";return;
   case DEFAULT_CHARSET    :*name="Default";return;
   case SYMBOL_CHARSET     :*name="Symbol";return;
   case SHIFTJIS_CHARSET   :*name="Shiftjis";return;
   case HANGUL_CHARSET     :*name="Hangul";return;
   case GB2312_CHARSET     :*name="Gb2312";return;
   case CHINESEBIG5_CHARSET:*name="Chinese";return;
   case OEM_CHARSET        :*name="OEM";return;
   case JOHAB_CHARSET      :*name="Johab";return;
   case HEBREW_CHARSET     :*name="Hebrew";return;
   case ARABIC_CHARSET     :*name="Arabic";return;
   case GREEK_CHARSET      :*name="Greek";return;
   case TURKISH_CHARSET    :*name="Turkish";return;
   case VIETNAMESE_CHARSET :*name="Vietnamese";return;
   case THAI_CHARSET       :*name="Thai";return;
   case EASTEUROPE_CHARSET :*name="Easteurope";return;
   case RUSSIAN_CHARSET    :*name="Russian";return;
   case MAC_CHARSET        :*name="Mac";return;
   case BALTIC_CHARSET     :*name="Baltic";return;
   default                 :*name="unknown";return;
  }
}

void TfontPage::fillCharsets(void)
{
 int oldi=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETCURSEL,0,0);
 int oldii=(oldi!=CB_ERR)?SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_GETITEMDATA,oldi,0):ANSI_CHARSET;
 
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_RESETCONTENT,0,0);
 vector<int> sl;
 LOGFONT lf;
 if (osd)
  deci->getOSDfontName(lf.lfFaceName,LF_FACESIZE);
 else
  deci->getFontName(lf.lfFaceName,LF_FACESIZE);
 lf.lfCharSet=DEFAULT_CHARSET;lf.lfPitchAndFamily=0;
 HDC dc=GetDC(m_hwnd);
 EnumFontFamiliesEx(dc,&lf,EnumFamCallBackCharsets,LPARAM(&sl),0); 
 ReleaseDC(m_hwnd,dc);
 for (unsigned int i=0;i<sl.size();i++)
  {
   char *name;
   getCharset(sl[i],&name);
   int ii=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_ADDSTRING,0,LPARAM(name));
   SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_CHARSET,CB_SETITEMDATA,ii,sl[i]);
  }
 selectCharset(oldii);
}

void TfontPage::cfg2dlg(void)
{
 font2dlg();
}

void TfontPage::font2dlg(void)
{
 char s[256];int x;
 x=cfgGet(osd?IDFF_OSDfontSize:IDFF_fontSize);
 sprintf(s,"Size: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SIZE,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_SETPOS,TRUE,x);
 x=cfgGet(osd?IDFF_OSDfontSpacing:IDFF_fontSpacing);
 sprintf(s,"Spacing: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SPACING,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_SETPOS,TRUE,x);
 x=cfgGet(osd?IDFF_OSDfontShadowStrength:IDFF_fontShadowStrength);
 sprintf(s,"Shadow intensity: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SHADOW_STRENGTH,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_STRENGTH,TBM_SETPOS,TRUE,x);
 x=cfgGet(osd?IDFF_OSDfontShadowRadius:IDFF_fontShadowRadius);
 sprintf(s,"Shadow radius: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FONT_SHADOW_RADIUS,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_RADIUS,TBM_SETPOS,TRUE,x);

 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_SETCURSEL,cfgGet(osd?IDFF_OSDfontWeight:IDFF_fontWeight)/100-1,0);
 char fontname[256];
 if (osd)
  deci->getOSDfontName(fontname,255);
 else
  deci->getFontName(fontname,255);
 SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_NAME,CB_SELECTSTRING,-1,LPARAM(fontname));
 fillCharsets();
 selectCharset(cfgGet(osd?IDFF_OSDfontCharset:IDFF_fontCharset));
}

HRESULT TfontPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SIZE))
     {
      cfgSet(osd?IDFF_OSDfontSize:IDFF_fontSize,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SIZE,TBM_GETPOS,0,0));
      font2dlg();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SPACING))
     {
      cfgSet(osd?IDFF_OSDfontSpacing:IDFF_fontSpacing,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SPACING,TBM_GETPOS,0,0));
      font2dlg();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SHADOW_STRENGTH) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_FONT_SHADOW_RADIUS))
     {
      cfgSet(osd?IDFF_OSDfontShadowStrength:IDFF_fontShadowStrength,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_STRENGTH,TBM_GETPOS,0,0));
      cfgSet(osd?IDFF_OSDfontShadowRadius  :IDFF_fontShadowRadius  ,SendDlgItemMessage(m_hwnd,IDC_TBR_FONT_SHADOW_RADIUS  ,TBM_GETPOS,0,0));
      font2dlg();
      return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CBX_FONT_WEIGHT:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         int i=SendDlgItemMessage(m_hwnd,IDC_CBX_FONT_WEIGHT,CB_GETCURSEL,0,0);
         cfgSet(osd?IDFF_OSDfontWeight:IDFF_fontWeight,(i+1)*100);
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
         if (osd)
          deci->setOSDfontName(pomS);
         else
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
         cfgSet(osd?IDFF_OSDfontCharset:IDFF_fontCharset,ii);
         return TRUE;
        }
       else
        break;
      case IDC_IMG_FONT_COLOR :
       if (HIWORD(wParam)==STN_CLICKED)
        {
         CHOOSECOLOR cc;
         memset(&cc,0,sizeof(cc));
         cc.lStructSize=sizeof(cc);
         cc.hwndOwner=m_hwnd;
         cc.rgbResult=cfgGet(osd?IDFF_OSDfontColor:IDFF_fontColor);
         //TODO: store custom colors
         cc.lpCustColors=custColors;
         cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT;
         if (ChooseColor(&cc))
          {
           cfgSet(osd?IDFF_OSDfontColor:IDFF_fontColor,cc.rgbResult);
           font2dlg();
           InvalidateRect(GetDlgItem(m_hwnd,IDC_IMG_FONT_COLOR),NULL,FALSE);
          }
         return TRUE; 
        }
       else
        break; 
     }
    break; 
   case WM_DRAWITEM:
    if (wParam==IDC_IMG_FONT_COLOR)
     {
      LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
      LOGBRUSH lb;
      lb.lbColor=cfgGet(osd?IDFF_OSDfontColor:IDFF_fontColor);
      lb.lbStyle=BS_SOLID;
      HBRUSH br=CreateBrushIndirect(&lb);
      FillRect(dis->hDC,&dis->rcItem,br);
      DeleteObject(br);
      return TRUE;
     }
    else 
     break; 
  }   
 return FALSE;
}

void TfontPage::getTip(char *tipS,int len)
{
 char font[256];
 if (osd)
  deci->getOSDfontName(font,255);
 else 
  deci->getFontName(font,255);
 char *charset;getCharset(cfgGet(osd?IDFF_OSDfontCharset:IDFF_fontCharset),&charset);
 sprintf(tipS,"Font: %s, %s charset, size:%i, %s, spacing:%i\nShadow intensity: %i, shadow radius:%i",font,charset,cfgGet(osd?IDFF_OSDfontSize:IDFF_fontSize),fontWeights[cfgGet(osd?IDFF_OSDfontWeight:IDFF_fontWeight)/100-1],cfgGet(osd?IDFF_OSDfontSpacing:IDFF_fontSpacing),cfgGet(osd?IDFF_OSDfontShadowStrength:IDFF_fontShadowStrength),cfgGet(osd?IDFF_OSDfontShadowRadius:IDFF_fontShadowRadius));
}

TfontPage::TfontPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 osd=false;
 dialogId=IDD_FONT;
 inPreset=1;
 createWindow();
}
