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
#ifndef _CMSG_H_
#define _CMSG_H_

#include "gmbuffer.h"

#define MSG_DEFAULT_PRIORITY	5

class CMsg
{
public:
	CMsg();
	CMsg(int fd);
	virtual ~CMsg();

	int Open(const char *path = NULL);
	int Close();

	/* para el server */
	int Send(int key, CGMBuffer* msg);
	long Receive(CGMBuffer* msg, long to_cs = -1);

	/* para cliente (to en segundos)*/
	int Query(int key, CGMBuffer* qmsg, CGMBuffer* rmsg, long to_cs);

	int GetKey();
	int GetIndex();
	long GetCount();
	long GetRemoteCount(int key);
	int SetPriority(int p);
	int GetRemoteKey(const char* path, int index = 0);

protected:
	int Wait(long to);

	int m_index;
	int m_key;
	int m_fd;
	int m_error;
	long m_priority;
};
#endif /* _CMSG_H_ */

