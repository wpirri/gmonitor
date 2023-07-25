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
#ifndef _GMCOMM_H_
#define _GMCOMM_H_

#include <ctcp.h>
#include <gmbuffer.h>

#include <unistd.h>
using namespace std;

class CGMComm
{
public:
	CGMComm(CTcp* sock = NULL);
	CGMComm(int in, int out, int err = STDERR_FILENO);
	virtual ~CGMComm();

	CGMBuffer* Read(unsigned int len, int to_cs = 0);
	int Write(CGMBuffer* data);
	int Write(const void* data, unsigned int len);
protected:
	CGMBuffer* FDRead(unsigned int len, int to_s);
	CGMBuffer* SockRead(unsigned int len, int to_cs);
	int FDWrite(const void* data, unsigned int datalen);
	int SockWrite(const void* data, unsigned int datalen);
	int FDError(const void* data, unsigned int datalen);
private:
	CTcp* m_socket;
	int m_fdin;
	int m_fdout;
	int m_fderr;

};
#endif /*_GMCOMM_H_*/

