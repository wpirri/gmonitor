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
#ifndef _GMSWAITED_H_
#define _GMSWAITED_H_

#include "gmsbase.h"
#include "gmconst.h"
#include "gmbuffer.h"
#include "cmsg.h"
#include "gmisc.h"
#include "gmerror.h"
#include "gmessage.h"
#include "glog.h"

#include <iostream>
#include <string>
using namespace std;

#define GME_WAIT_ERROR   -1
#define GME_WAIT_TIMEOUT 0
#define GME_WAIT_OK      1

class CGMServerWait : public CGMServerBase
{
public:
	CGMServerWait();
	~CGMServerWait();

	int Init(const char *server_name);
	int Init(string &server_name);
	int Wait(char *fn, char *typ, void *data, unsigned long maxlen, unsigned long *datalen, long to_cs=-1);
	int Resp(const void *data, unsigned long datalen, int rc);
	int Exit();
	unsigned int GetLogLevel();
	void SetLogLevel(unsigned int level);
	CGLog*	m_pLog;
protected:
	CGMTdb::CSrvTab	m_server_params;
	//int	m_log_level;
private:
	CMsg*	m_pMsg;
	/*CGMTdb*	m_pConfig; lo paso a CGMServerBase*/
	CGMessage m_outMessage;
	long      m_srcMessage;
};
#endif /* _GMSWAITED_H_ */
