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
#include "CpostProc.h"
#include "resource.h"
#include "Tpostproc.h"

#define POSTPROC_TIMER 1999

void TpostProcPage::init(void)
{
 if (cfgGet(IDFF_inPlayer)) SetTimer(m_hwnd,POSTPROC_TIMER,100,NULL);
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETRANGE,TRUE,MAKELONG(0,6));
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSEL  ,TRUE,MAKELONG(0,6));
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELSTART,TRUE,0);
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELEND,TRUE,0);
 SendDlgItemMessage(m_hwnd,IDC_TBR_DEBLOCKSTRENGTH,TBM_SETRANGE,TRUE,MAKELPARAM(0,512));
 SendDlgItemMessage(m_hwnd,IDC_TBR_DEBLOCKSTRENGTH,TBM_SETLINESIZE,0,8);
 SendDlgItemMessage(m_hwnd,IDC_TBR_DEBLOCKSTRENGTH,TBM_SETPAGESIZE,0,32); 
 cfg2dlg();
}

void TpostProcPage::cfg2dlg(void)
{
 postProc2dlg();
}

void TpostProcPage::postProc2dlg(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETPOS,TRUE,cfgGet(IDFF_ppqual));
 setCheck(IDC_CHB_AUTOQ    , cfgGet(IDFF_autoq));
 setCheck(IDC_RBT_PPPRESETS,!cfgGet(IDFF_ppIsCustom));
 setCheck(IDC_RBT_PPCUSTOM , cfgGet(IDFF_ppIsCustom));
 setCheck(IDC_CHB_LEVELFIX_LUM  ,cfgGet(IDFF_levelFixLum));
 setCheck(IDC_CHB_LEVELFIX_CHROM,cfgGet(IDFF_levelFixChrom));
 SendDlgItemMessage(m_hwnd,IDC_TBR_DEBLOCKSTRENGTH,TBM_SETPOS,TRUE,cfgGet(IDFF_deblockStrength));
 char pomS[256];
 sprintf(pomS,"Processing strength: %i%%",100*cfgGet(IDFF_deblockStrength)/256);
 SendDlgItemMessage(m_hwnd,IDC_LBL_DEBLOCKSTRENGTH,WM_SETTEXT,0,LPARAM(pomS));
 setPPchbs();
}

void TpostProcPage::setPPchbs(void)
{
 unsigned int ppmode;
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
  }
 setCheck(IDC_CHB_DEBLOCKV_LUM  ,ppmode&LUM_V_DEBLOCK);
 setCheck(IDC_CHB_DEBLOCKH_LUM  ,ppmode&LUM_H_DEBLOCK);
 setCheck(IDC_CHB_DEBLOCKV_CHROM,ppmode&CHROM_V_DEBLOCK);
 setCheck(IDC_CHB_DEBLOCKH_CHROM,ppmode&CHROM_H_DEBLOCK);
 setCheck(IDC_CHB_DERING_LUM    ,ppmode&LUM_DERING);
 setCheck(IDC_CHB_DERING_CHROM  ,ppmode&CHROM_DERING);
}

HRESULT TpostProcPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_PPQUAL))
     {
      cfgSet(IDFF_ppqual,SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_GETPOS,0,0));
      cfgSet(IDFF_currentq,cfgGet(IDFF_ppqual));
      setPPchbs();
      return TRUE;
     }
    else if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_DEBLOCKSTRENGTH))
     {
      cfgSet(IDFF_deblockStrength,SendDlgItemMessage(m_hwnd,IDC_TBR_DEBLOCKSTRENGTH,TBM_GETPOS,0,0));
      postProc2dlg();
      return TRUE;
     }
    break;  
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_POSTPROC:
       cfgSet(IDFF_isPostproc,getCheck(IDC_CHB_POSTPROC));
       parent->drawInter();
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
     }  
    break; 
   case WM_TIMER:
    if (wParam==POSTPROC_TIMER/* && !cfgGet(IDFF_ppIsCustom)*/)
     {
      //if (!cfgGet(IDFF_autoq)) cfgSet(IDFF_currentq,cfgGet(IDFF_ppqual));
      SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELEND,TRUE,cfgGet(IDFF_currentq));
      setPPchbs();
      return TRUE;
     }
    break;
   case WM_DESTROY:
    KillTimer(m_hwnd,POSTPROC_TIMER);
    break;
  }
 return FALSE;
}

void TpostProcPage::interDlg(void)
{
 setCheck(IDC_CHB_POSTPROC,cfgGet(IDFF_isPostproc));
}
void TpostProcPage::getTip(char *tipS,int len)
{
 char pomS[256];
 tipS[0]='\0';
 if (cfgGet(IDFF_ppIsCustom))
  {
   strcat(tipS,"custom");
  }
 else
  {
   strcat(tipS,"presets: ");
   sprintf(pomS,"strength %i",cfgGet(IDFF_ppqual));strcat(tipS,pomS);
   if (cfgGet(IDFF_autoq)) strcat(tipS," automatic");
  } 
 if (cfgGet(IDFF_levelFixChrom) || cfgGet(IDFF_levelFixLum)) strcat(tipS,", level fix"); 
}
TpostProcPage::TpostProcPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci) :TconfPage(Iparent,IhwndParent,Ideci)
{
 createWindow(IDD_POSTPROC);
}
