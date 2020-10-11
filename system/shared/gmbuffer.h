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
#ifndef _GMBUFFER_H_
#define _GMBUFFER_H_

#include <string>
using namespace std;

class CGMBuffer
{
public:
	CGMBuffer();
	CGMBuffer(const char* str);
	CGMBuffer(const void* ptr, unsigned long len);
	CGMBuffer(string s);

	virtual ~CGMBuffer();

	void Clear();

	const char* operator=(const char *str);
	const char* operator=(CGMBuffer buffer);
	const char* operator=(string s);

	const char* operator+=(const char *str);
	const char* operator+=(CGMBuffer buffer);
	const char* operator+=(string s);

	const char* Set(const char *str);
	const char* Set(const void *ptr, unsigned long len);
	const char* Set(CGMBuffer buffer);
	const char* Set(string s);

	const char* Add(const char *str);
	const char* Add(const void *ptr, unsigned long len);
	const char* Add(CGMBuffer buffer);
	const char* Add(string s);

	const char* Format(const char* fmt, ...);
	const char* AddFormat(const char* fmt, ...);

	unsigned long Length();
	const char* Data();
	const char* C_Str();
	string String();
	const char* At(unsigned long idx);

protected:
	char *m_buffer;
	unsigned long m_len;

};
#endif /* _GMBUFFER_H_ */

