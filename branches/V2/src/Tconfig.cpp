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
#include "Tconfig.h"
#include "xvid\xvid.h"
#include "reg.h"
#include "TmovieSourceLibavcodec.h"
#include "TmovieSourceXviD.h"

Tconfig config;

int Tconfig::cpu_flags=0;

void Tconfig::init(void)
{
 char sysdir[256];
 GetSystemDirectory(sysdir,255);
 TregOpRegRead t(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD);
 t._REG_OP_S("pth",pth,sysdir);

 if (strlen(pth) && pth[strlen(pth)-1]!='\\') strcat(pth,"\\");
 XVID_INIT_PARAM xip;
 xip.cpu_flags=0;
 xvid_init(NULL, 0, &xip, NULL);
 cpu_flags=xip.cpu_flags;

 isLibavcodec=TmovieSourceLibavcodec::getVersion(NULL);
 isXviD=TmovieSourceXviD::getVersion(NULL);
}
