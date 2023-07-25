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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <gmcomm.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <csignal>
using namespace std;

CGMComm::CGMComm(CTcp* sock)
{
	m_socket = sock;
	if(!sock)
	{
		m_fdin = STDIN_FILENO;
		m_fdout = STDOUT_FILENO;
		m_fderr = STDERR_FILENO;
	}
}

CGMComm::CGMComm(int in, int out, int err)
{
	m_fdin = in;
	m_fdout = out;
	m_fderr = err;
	m_socket = NULL;
}

CGMComm::~CGMComm()
{

}

CGMBuffer* CGMComm::Read(unsigned int len, int to_cs)
{
	if(m_socket)
	{
		return SockRead(len, to_cs);
	}
	else
	{
		return FDRead(len, (int)(to_cs/100));
	}
}

int CGMComm::Write(CGMBuffer* data)
{
	if(!data) return 0;
	return Write(data->Data(), data->Length());
}

int CGMComm::Write(const void *data, unsigned int len)
{
	if(!data || !len) return 0;
	if(m_socket)
	{
		return SockWrite(data, len);
	}
	else
	{
		return FDWrite(data, len);
	}
}

CGMBuffer* CGMComm::FDRead(unsigned int len, int to_s)
{
	char ch[1];
	int rc;
  unsigned int remain;
	CGMBuffer *buff = new CGMBuffer("");

	signal(SIGALRM, SIG_IGN);
	if(to_s <= 0) to_s = 1;
	alarm(to_s);
	while(buff->Length() < len)
	{
		rc = read(m_fdin, ch, 1);
		if(rc > 0)                         /* Si leyó algo */
    {
      buff->Add(ch,1);
    }
		else if((remain = alarm(0)) > 0)   /* si no leyó nada pero todavía queda tiempo */
    {
      alarm(remain);
    }
    else
    {
      break;
    }
	}
	alarm(0);
	if(buff->Length() > 0)
	{
		return buff;
	}
	else
	{
		delete buff;
		return NULL;
	}
}

CGMBuffer* CGMComm::SockRead(unsigned int len, int to_cs)
{
	char *ch;
	int rc;
	CGMBuffer *buff = NULL;

	ch = (char*)malloc(len);
	rc = m_socket->Receive(ch, len, to_cs * 10);
	if(rc > 0)
	{
		buff = new CGMBuffer(ch, rc);
	}
	free(ch);
	return buff;
}

int CGMComm::FDWrite(const void* data, unsigned int datalen)
{
	unsigned int len = 0;
	int rc;
	if(!data || !datalen) return -1;
	while(len < datalen)
	{
		if((rc = write(m_fdout,
			((char*)data + len),
			datalen - len)) >= 0)
		{
			len+=rc;
		}
	}
	return len;
}

int CGMComm::SockWrite(const void* data, unsigned int datalen)
{
	if(m_socket->Send(data, datalen) == 0)
	{
		return datalen;
	}
	else
	{
		return 0;
	}
}

int CGMComm::FDError(const void* data, unsigned int datalen)
{
	unsigned int len = 0;
	int rc;
	if(!data || !datalen) return -1;
	while(len < datalen)
	{
		if((rc = write(m_fderr,
			((char*)data + len),
			datalen - len)) >= 0)
		{
			len+=rc;
		}
	}
	return len;
}

