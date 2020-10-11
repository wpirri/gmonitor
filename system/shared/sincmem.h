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
#ifndef _SINCMEM_H_
#define _SINCMEM_H_

#include "sincro.h"
#include "shmem.h"

class CSincMem : public CSincro,CShMem
{
public:
	CSincMem();
	CSincMem(int key);
	virtual ~CSincMem();
	int Key(int key);
	int Create(int length);
	void Close();
	int Open(int length);
	int SetAt(int pos, const void* data, int length);
	int GetAt(int pos, void* data, int length);
	bool IsOwner();
private:
	int WriteBloq(int in_out);
	int m_key;
	int m_length;
};
#endif /*_SINCMEM_H_*/
