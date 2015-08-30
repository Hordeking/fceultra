/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2015 CaH4e3
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "mapinc.h"

static uint16 addrlatch;
static uint8 datalatch;

static SFORMAT StateRegs[] =
{
	{ &addrlatch, 2, "ADRL" },
	{ &datalatch, 1, "DATL" },
	{ 0 }
};

static void Sync(void) {
	uint8 prg = (addrlatch & 7) | ((addrlatch & 0x40) >> 3);
	setchr8(datalatch);
	if(addrlatch & 0x80) {
		setprg16(0x8000,prg);
		setprg16(0xC000,prg);
	} else {
		setprg32(0x8000,prg >> 1);
	}
	setmirror(MI_V);
}

static DECLFW(EH8813AWrite) {
	if((addrlatch & 0x100) == 0) {
		addrlatch = A & 0x1FF;
		datalatch = V & 0xF;
	}
	Sync();
}

static void EH8813APower(void) {
	addrlatch = datalatch = 0;
	Sync();
	SetReadHandler(0x8000, 0xFFFF, CartBR);
	SetWriteHandler(0x8000, 0xFFFF, EH8813AWrite);
}

static void EH8813AReset(void) {
	addrlatch = datalatch = 0;
	Sync();
}

static void StateRestore(int version) {
	Sync();
}

void UNLEH8813A_Init(CartInfo *info) {
	info->Reset = EH8813AReset;
	info->Power = EH8813APower;
	GameStateRestore = StateRestore;
	AddExState(&StateRegs, ~0, 0, 0);
}
