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
#include "reg.h"
#include "TdialogSettings.h"

void TdialogSettings::reg_op(TregOp &t)
{
 t._REG_OP_N("lastPage",lastPage,0);
 t._REG_OP_N("dlgRestorePos",dlgRestorePos,0);
 t._REG_OP_N("dlgPosX",dlgPosX,0);
 t._REG_OP_N("dlgPosY",dlgPosY,0);
 t._REG_OP_N("lvWidth0",lvWidth0,300);
 t._REG_OP_N("showHints",showHints,1);
}

void TdialogSettings::load(void)
{
 TregOpRegRead t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD);
 reg_op(t);
}

void TdialogSettings::save(void)
{
 TregOpRegWrite t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD);
 reg_op(t);
}
