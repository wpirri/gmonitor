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
#ifndef _GMSBUFFER_H_
#define _GMBSUFFER_H_

#include <vector>
using namespace std;

#include <gmontdb.h>
#include <msgtype.h>
#include <gmsbase.h>
#include <gms.h>

class CGMSBuffer
{
public:
	CGMSBuffer(CGMServer* pServer, CGLog* plog = NULL);
	virtual ~CGMSBuffer();

	int Process(const char *funcion,
				void* in, unsigned long inlen,
				void* out, unsigned long *outlen,
				CGMServerBase::CLIENT_DATA* pClientData);

	unsigned int GetNewId();
	int DeleteId(unsigned int id, CGMServerBase::CLIENT_DATA* pClientData);
	int DeleteTrans(unsigned int trans);

protected:

	typedef struct _ST_BUFFER
	{
		unsigned int id_buffer;
		unsigned int id_trans;
		unsigned int id_timer;
		string user;
		string client;
		string key;
		string group;
		CGMBuffer* buffer;;
	} ST_BUFFER;

	CGMTdb* m_pDB;
	vector <ST_BUFFER> m_buffer_list;
	unsigned int m_last_id;
	CGLog* m_pLog;
	CGMServer* m_pServer;
};
#endif /* _GMSBUFFER_H_ */

