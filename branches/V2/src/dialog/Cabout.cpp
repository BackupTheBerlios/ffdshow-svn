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
#include "Cabout.h"
#include "resource.h"
#include "IffDecoder.h"

void TaboutPage::cfg2dlg(void)
{
}

HRESULT TaboutPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 return FALSE;
}

void TaboutPage::init(void)
{
 char vers[1024];vers[0]='\0';
 deci->getAVcodecVersion(vers,1023);
 SendDlgItemMessage(m_hwnd,IDC_LBL_AVVERSION,WM_SETTEXT,0,LPARAM(vers));
 deci->getXvidVersion(vers,1023);
 SendDlgItemMessage(m_hwnd,IDC_LBL_XVIDVERSION,WM_SETTEXT,0,LPARAM(vers));
 strcpy(vers,"DirectShow filter build date: "__DATE__" , "__TIME__);
 SendDlgItemMessage(m_hwnd,IDC_LBL_FFDSHOWVERSION,WM_SETTEXT,0,LPARAM(vers));
}
