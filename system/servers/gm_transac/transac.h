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
#ifndef _TRANSAC_H_
#define _TRANSAC_H_

#include "wtimer.h"
#include "gmswaited.h"

class CTransac : public CWTimer
{
public:
	CTransac(CGMServerWait* pServer);
	virtual ~CTransac();

	unsigned int New(unsigned long to);
	int Del(unsigned int trans);
	bool Exist(unsigned int trans);
	unsigned int Check();
private:
	unsigned int m_trans_count;
	CGMServerWait* m_pServer;
};

#endif /* _TRANSAC_H_ */
