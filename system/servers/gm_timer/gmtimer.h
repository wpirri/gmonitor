/***************************************************************************
    Copyright (C) 2003   Walter Pirri

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/
#ifndef _GMTIMER_H_
#define _GMTIMER_H_

#include "wtimer.h"

#include <vector>
using namespace std;

#include <gmswaited.h>
#include <msgtype.h>
#include <gmsbase.h>
#include <glog.h>

class CGMTimer
{
public:
	CGMTimer(CGMServerWait *pServer, CGLog* plog = NULL);
	virtual ~CGMTimer();

	int Message(const char *funcion,
				void* in, unsigned long inlen,
				void** out, unsigned long* outlen,
				CGMServerBase::CLIENT_DATA* pClientData);
	long TimeOut();

private:
	CWTimer *m_pTimer;
	CGLog* m_pLog;
	CGMServerWait *m_pServer;
};
#endif /*_GMTIMER_H_*/
