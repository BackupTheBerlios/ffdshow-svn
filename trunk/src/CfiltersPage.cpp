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
#include "CfiltersPage.h"
#include "resource.h"
#include <commctrl.h>
#include <string.h>
#include <assert.h>
#include "IffDecoder.h"
#include "Tpostproc.h"

#define POSTPROC_TIMER 1999

int TfiltersPage::writeLumGain(int x)
{
 char s[256];
 sprintf(s,"Luminance gain:  %i",x);
 if (x==Tconfig::lumGainDef) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_LUMGAIN,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeLumOffset(int x)
{
 char s[256];
 sprintf(s,"Luminance offset:  %i",x-256);
 if (x-256==Tconfig::lumOffsetDef) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_LUMOFFSET,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeGamma(int x)
{
 char s[256];
 __asm emms;
 sprintf(s,"Gamma correction:  %3.2f",float(x/100.0));
 if (x==Tconfig::gammaCorrectionDef) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_GAMMA,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeHue(int x)
{
 char s[256];
 sprintf(s,"Hue:  %i°",x);
 if (x==Tconfig::hueDef) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_HUE,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeSaturation(int x)
{
 char s[256];
 sprintf(s,"Saturation:  %i",x);
 if (x==Tconfig::saturationDef) strcat(s," (off)");
 SendDlgItemMessage(m_hwnd,IDC_LBL_SATURATION,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeSharpenStrength(int x)
{
 char s[256];
 sprintf(s,"Strength:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_XSHARPENSTRENGTH,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeSharpenThreshold(int x)
{
 char s[256];
 sprintf(s,"Threshold:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_XSHARPENTHRESH,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeNoiseStrength(int x)
{
 char s[256];
 sprintf(s,"Luminance noise strength:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOISESTRENGTH,WM_SETTEXT,0,LPARAM(s));
 return x;
}
int TfiltersPage::writeNoiseStrengthChroma(int x)
{
 char s[256];
 sprintf(s,"Chroma noise strength:  %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_NOISESTRENGTH_CHROMA,WM_SETTEXT,0,LPARAM(s));
 return x;
}
void TfiltersPage::setPPchbs(void)
{
 int ppmode;
 if (cfgGet(IDFF_ppIsCustom)) 
  {
   enableWindow(IDC_CHB_DEBLOCKV_LUM  ,true);
   enableWindow(IDC_CHB_DEBLOCKH_LUM  ,true);
   enableWindow(IDC_CHB_DEBLOCKV_CHROM,true);
   enableWindow(IDC_CHB_DEBLOCKH_CHROM,true);
   enableWindow(IDC_CHB_DERING_LUM    ,true);
   enableWindow(IDC_CHB_DERING_CHROM  ,true);
   ppmode=cfgGet(IDFF_ppcustom);
  }
 else
  {
   enableWindow(IDC_CHB_DEBLOCKV_LUM  ,false);
   enableWindow(IDC_CHB_DEBLOCKH_LUM  ,false);
   enableWindow(IDC_CHB_DEBLOCKV_CHROM,false);
   enableWindow(IDC_CHB_DEBLOCKH_CHROM,false);
   enableWindow(IDC_CHB_DERING_LUM    ,false);
   enableWindow(IDC_CHB_DERING_CHROM  ,false);
   deci->getPPmode(&ppmode);
  };
 setCheck(IDC_CHB_DEBLOCKV_LUM  ,ppmode&LUM_V_DEBLOCK);
 setCheck(IDC_CHB_DEBLOCKH_LUM  ,ppmode&LUM_H_DEBLOCK);
 setCheck(IDC_CHB_DEBLOCKV_CHROM,ppmode&CHROM_V_DEBLOCK);
 setCheck(IDC_CHB_DEBLOCKH_CHROM,ppmode&CHROM_H_DEBLOCK);
 setCheck(IDC_CHB_DERING_LUM    ,ppmode&LUM_DERING);
 setCheck(IDC_CHB_DERING_CHROM  ,ppmode&CHROM_DERING);
}

void TfiltersPage::postProc2dlg(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETPOS,TRUE,cfgGet(IDFF_ppqual));
 setCheck(IDC_CHB_AUTOQ    , cfgGet(IDFF_autoq));
 setCheck(IDC_RBT_PPPRESETS,!cfgGet(IDFF_ppIsCustom));
 setCheck(IDC_RBT_PPCUSTOM , cfgGet(IDFF_ppIsCustom));
 setCheck(IDC_CHB_LEVELFIX_LUM  ,cfgGet(IDFF_levelFixLum));
 setCheck(IDC_CHB_LEVELFIX_CHROM,cfgGet(IDFF_levelFixChrom));
 setPPchbs();
}

void TfiltersPage::noise2dlg(void)
{
 setCheck(IDC_RBT_NOISE_MOJ    ,cfgGet(IDFF_noiseMethod)==0);
 setCheck(IDC_RBT_NOISE_AVIH   ,cfgGet(IDFF_noiseMethod)==1);
 setCheck(IDC_CHB_NOISE_UNIFORM,cfgGet(IDFF_uniformNoise));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETPOS,TRUE,writeNoiseStrength(cfgGet(IDFF_noiseStrength)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETPOS,TRUE,writeNoiseStrengthChroma(cfgGet(IDFF_noiseStrengthChroma)));
}

void TfiltersPage::pictProp2dlg(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETPOS,TRUE,writeLumGain(cfgGet(IDFF_lumGain)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETPOS,TRUE,writeLumOffset(cfgGet(IDFF_lumOffset)+256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETPOS,TRUE,writeGamma(cfgGet(IDFF_gammaCorrection)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETPOS,TRUE,writeHue(cfgGet(IDFF_hue)));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETPOS,TRUE,writeSaturation(cfgGet(IDFF_saturation)));
}

void TfiltersPage::sharpen2dlg(void)
{
 int sharpenMethod=cfgGet(IDFF_sharpenMethod);
 setCheck(IDC_RBT_SHARPEN_XSHARPEN   ,(sharpenMethod==0));
 setCheck(IDC_RBT_SHARPEN_UNSHARPMASK,(sharpenMethod==1));
 if (cfgGet(IDFF_xsharp_strength)==0) cfgSet(IDFF_xsharp_strength,1);
 switch  (sharpenMethod)
  {
   case 0:SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETPOS,TRUE,writeSharpenStrength(cfgGet(IDFF_xsharp_strength)));
          enableWindow(IDC_TBR_XSHARPENTHRESH,true);
          enableWindow(IDC_LBL_XSHARPENTHRESH,true);
          SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETPOS,TRUE,writeSharpenThreshold(cfgGet(IDFF_xsharp_threshold)));
          break;
   case 1:SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETPOS,TRUE,writeSharpenStrength(cfgGet(IDFF_unsharp_strength)));
          enableWindow(IDC_TBR_XSHARPENTHRESH,false);
          enableWindow(IDC_LBL_XSHARPENTHRESH,false);
          SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETPOS,TRUE,writeSharpenThreshold(cfgGet(IDFF_unsharp_threshold)));
          break;
  };        
}
void TfiltersPage::blur2dlg(void)
{
 int x=cfgGet(IDFF_blurStrength);
 char s[256];
 sprintf(s,"Strength: %i",x);
 SendDlgItemMessage(m_hwnd,IDC_LBL_BLUR_STRENGTH,WM_SETTEXT,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETPOS,TRUE,x);
}
void TfiltersPage::cfg2dlg(void)
{
 interDlg();
 postProc2dlg();
 pictProp2dlg();
 noise2dlg();
 blur2dlg();
 sharpen2dlg(); 
}

void TfiltersPage::interDlg(void)
{
 setCheck(IDC_CHB_POSTPROC,cfgGet(IDFF_isPostproc));
 setCheck(IDC_CHB_PICTPROP,cfgGet(IDFF_isPictProp));
 setCheck(IDC_CHB_NOISE   ,cfgGet(IDFF_isNoise   ));
 setCheck(IDC_CHB_BLUR    ,cfgGet(IDFF_isBlur    ));
 setCheck(IDC_CHB_SHARPEN ,cfgGet(IDFF_isSharpen ));
}

void TfiltersPage::createConfig(void)
{
 SetTimer(m_hwnd,POSTPROC_TIMER,100,NULL);
 
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETRANGE,TRUE,MAKELPARAM(0,256));
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_SETPAGESIZE,0,32); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETRANGE,TRUE,MAKELPARAM(0,512));
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_SETPAGESIZE,0,64); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETRANGE,TRUE,MAKELPARAM(1,400));
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_SETPAGESIZE,0,40); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETRANGE,TRUE,MAKELPARAM(-180,180));
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_SETPAGESIZE,0,20); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETRANGE,TRUE,MAKELPARAM(0,128));
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETPAGESIZE,0,8); 

 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETRANGE,TRUE,MAKELONG(0,6));
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSEL  ,TRUE,MAKELONG(0,6));
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELSTART,TRUE,0);
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELEND,TRUE,0);

 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(1,127));
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_SETPAGESIZE,0,16); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETRANGE,TRUE,MAKELPARAM(0,255));
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_SETPAGESIZE,0,16); 

 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(0,255));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_SETPAGESIZE,0,16); 
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETRANGE,TRUE,MAKELPARAM(0,127));
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_SETPAGESIZE,0,16); 

 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(1,255));
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_SETPAGESIZE,0,16); 

 cfg2dlg();
}

HRESULT TfiltersPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
  //case WM_SHOWWINDOW:
  // if (wParam) cfg2dlg();
  // break;
  case WM_DESTROY:
   KillTimer(m_hwnd,POSTPROC_TIMER);
   break;
  case WM_HSCROLL:
   if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_PPQUAL))
    {
     cfgSet(IDFF_ppqual,SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_GETPOS,0,0));
     cfgSet(IDFF_currentq,cfgGet(IDFF_ppqual));
     setPPchbs();
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_LUMGAIN) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_LUMOFFSET) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_GAMMA))
    {
     cfgSet(IDFF_lumGain,writeLumGain(SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN,TBM_GETPOS,0,0)));
     cfgSet(IDFF_lumOffset,writeLumOffset(SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET,TBM_GETPOS,0,0))-256);
     cfgSet(IDFF_gammaCorrection,writeGamma(SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA,TBM_GETPOS,0,0)));
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_HUE) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_SATURATION))
    {
     cfgSet(IDFF_hue,writeHue(SendDlgItemMessage(m_hwnd,IDC_TBR_HUE,TBM_GETPOS,0,0)));
     cfgSet(IDFF_saturation,writeSaturation(SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_GETPOS,0,0)));
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_XSHARPENSTRENGTH) || HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_XSHARPENTHRESH))
    {
     switch (cfgGet(IDFF_sharpenMethod))
      {
       case 0:cfgSet(IDFF_xsharp_strength,writeSharpenStrength(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_GETPOS,0,0)));
              cfgSet(IDFF_xsharp_threshold,writeSharpenThreshold(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_GETPOS,0,0)));
              break;
       case 1:cfgSet(IDFF_unsharp_strength,writeSharpenStrength(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENSTRENGTH,TBM_GETPOS,0,0)));
              cfgSet(IDFF_unsharp_threshold,writeSharpenThreshold(SendDlgItemMessage(m_hwnd,IDC_TBR_XSHARPENTHRESH,TBM_GETPOS,0,0)));
              break;
      };        
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_NOISESTRENGTH))
    {
     cfgSet(IDFF_noiseStrength,writeNoiseStrength(SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH,TBM_GETPOS,0,0)));
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA))
    {
     cfgSet(IDFF_noiseStrengthChroma,writeNoiseStrengthChroma(SendDlgItemMessage(m_hwnd,IDC_TBR_NOISESTRENGTH_CHROMA,TBM_GETPOS,0,0)));
     return TRUE;
    }
   else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_BLUR_STRENGTH))
    {
     cfgSet(IDFF_blurStrength,SendDlgItemMessage(m_hwnd,IDC_TBR_BLUR_STRENGTH,TBM_GETPOS,0,0));
     blur2dlg();
     return TRUE;
    }
   break;
   
  case WM_COMMAND:
   switch (LOWORD(wParam))  
    {
    case IDC_CHB_POSTPROC:
     cfgSet(IDFF_isPostproc,getCheck(IDC_CHB_POSTPROC));
     return TRUE;
    case IDC_CHB_PICTPROP:
     cfgSet(IDFF_isPictProp,getCheck(IDC_CHB_PICTPROP));
     return TRUE;
    case IDC_CHB_NOISE:
     cfgSet(IDFF_isNoise,getCheck(IDC_CHB_NOISE));
     return TRUE;
    case IDC_CHB_SHARPEN:
     cfgSet(IDFF_isSharpen,getCheck(IDC_CHB_SHARPEN));
     return TRUE;
    case IDC_CHB_BLUR:
     cfgSet(IDFF_isBlur,getCheck(IDC_CHB_BLUR));
     return TRUE;
    
    case IDC_RBT_PPPRESETS:
    case IDC_RBT_PPCUSTOM:
     cfgSet(IDFF_ppIsCustom,getCheck(IDC_RBT_PPCUSTOM));
     setPPchbs();
     return TRUE;
    case IDC_CHB_AUTOQ:
     cfgSet(IDFF_autoq,getCheck(IDC_CHB_AUTOQ));
     cfgSet(IDFF_currentq,cfgGet(IDFF_ppqual));
     setPPchbs();
     return TRUE;
    case IDC_CHB_DEBLOCKV_LUM:
    case IDC_CHB_DEBLOCKH_LUM:
    case IDC_CHB_DEBLOCKV_CHROM:
    case IDC_CHB_DEBLOCKH_CHROM:
    case IDC_CHB_DERING_LUM:
    case IDC_CHB_DERING_CHROM:
     {
      int ppcustom=0;
      ppcustom|=getCheck(IDC_CHB_DEBLOCKV_LUM  )?LUM_V_DEBLOCK:0;
      ppcustom|=getCheck(IDC_CHB_DEBLOCKH_LUM  )?LUM_H_DEBLOCK:0;
      ppcustom|=getCheck(IDC_CHB_DEBLOCKV_CHROM)?CHROM_V_DEBLOCK:0;
      ppcustom|=getCheck(IDC_CHB_DEBLOCKH_CHROM)?CHROM_H_DEBLOCK:0;
      ppcustom|=getCheck(IDC_CHB_DERING_LUM    )?LUM_DERING:0;
      ppcustom|=getCheck(IDC_CHB_DERING_CHROM  )?CHROM_DERING:0;
      cfgSet(IDFF_ppcustom,ppcustom);
      setPPchbs();
      return TRUE;
     }
    case IDC_CHB_LEVELFIX_LUM:
    case IDC_CHB_LEVELFIX_CHROM:
     {
      cfgSet(IDFF_levelFixLum  ,getCheck(IDC_CHB_LEVELFIX_LUM  ));
      cfgSet(IDFF_levelFixChrom,getCheck(IDC_CHB_LEVELFIX_CHROM));
      return TRUE;
     }
     
    case IDC_BT_LUMRESET:
     SendDlgItemMessage(m_hwnd,IDC_TBR_LUMGAIN   ,TBM_SETPOS,TRUE,writeLumGain(cfgSet(IDFF_lumGain,Tconfig::lumGainDef)));
     SendDlgItemMessage(m_hwnd,IDC_TBR_LUMOFFSET ,TBM_SETPOS,TRUE,writeLumOffset(256+(cfgSet(IDFF_lumOffset,Tconfig::lumOffsetDef))));
     SendDlgItemMessage(m_hwnd,IDC_TBR_GAMMA     ,TBM_SETPOS,TRUE,writeGamma(cfgSet(IDFF_gammaCorrection,Tconfig::gammaCorrectionDef)));
     SendDlgItemMessage(m_hwnd,IDC_TBR_HUE       ,TBM_SETPOS,TRUE,writeHue(cfgSet(IDFF_hue,Tconfig::hueDef)));
     SendDlgItemMessage(m_hwnd,IDC_TBR_SATURATION,TBM_SETPOS,TRUE,writeSaturation(cfgSet(IDFF_saturation,Tconfig::saturationDef)));
     return TRUE;
     
    case IDC_CHB_XSHARPEN:
     cfgSet(IDFF_isSharpen,getCheck(IDC_CHB_XSHARPEN));
     return TRUE;
    case IDC_RBT_SHARPEN_XSHARPEN:
    case IDC_RBT_SHARPEN_UNSHARPMASK:
     cfgSet(IDFF_sharpenMethod,getCheck(IDC_RBT_SHARPEN_XSHARPEN)?0:1);
     sharpen2dlg();
     return TRUE; 
     
    case IDC_RBT_NOISE_MOJ:
    case IDC_RBT_NOISE_AVIH:
     cfgSet(IDFF_noiseMethod,getCheck(IDC_RBT_NOISE_AVIH)?1:0);
     return TRUE;
    case IDC_CHB_NOISE_UNIFORM:
     cfgSet(IDFF_uniformNoise,getCheck(IDC_CHB_NOISE_UNIFORM));
     return TRUE;
   };
   break;
  case WM_TIMER:
   if (wParam==POSTPROC_TIMER/* && !cfgGet(IDFF_ppIsCustom)*/)
    {
     if (!cfgGet(IDFF_autoq)) cfgSet(IDFF_currentq,cfgGet(IDFF_ppqual));
     SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELEND,TRUE,cfgGet(IDFF_currentq));
     setPPchbs();
     return TRUE;
    };   
   break;
  };
 return FALSE;
};

TfiltersPage::TfiltersPage(HWND IhwndParent,IffDecoder *Ideci,int dialogId) :TconfPage(IhwndParent,Ideci)
{
 createWindow(dialogId);
}
