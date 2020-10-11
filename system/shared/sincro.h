/***************************************************************************
 * Copyright (C) 2003 Walter Pirri
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 ***************************************************************************/
#ifndef _SINCRO_H_
#define _SINCRO_H_

#ifndef __USE_GNU
#include <time.h>
#endif /* __USE_GNU */
#include <sys/sem.h>

typedef sembuf sembuff;

class CSincro
{
public:
	CSincro();
	CSincro(int key);
	virtual ~CSincro();
	int Key(int key);
	int Create(int count, int init_val = 0);
	int Open(int count);
	void Close();
	int Wait(int sem = 0);
	int Signal(int sem = 0);
	int Set(int sem, int val);
private:
	int m_SemCount;
	int m_SemId;
	bool m_own;
	int m_key;
	sembuff* SemBuff(int sem, int opt);
};
#endif /*_SINCRO_H_*/
