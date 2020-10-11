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
#include "transac.h"

CTransac::CTransac(CGMServerWait* pServer)
{
	m_trans_count = 1;
	m_pServer = pServer;
}

CTransac::~CTransac() { }

unsigned int CTransac::New(unsigned long to)
{
	/* obtengo un valor incremetal entre 1 y 9999 */
	if(++m_trans_count > 9999) m_trans_count = 1;
	/* le prendo un timer */
	if(CWTimer::Add( &m_trans_count, sizeof(unsigned int),
                         NULL, 0, to) == 0)
	{
		return m_trans_count;
	}
	else
	{
		return 0;
	}
}

int CTransac::Del(unsigned int trans)
{
	if(CWTimer::Del(&trans, sizeof(unsigned int)) == 0)
	{
		return 0;
	}
	return -1;
}

bool CTransac::Exist(unsigned int trans)
{
	return CWTimer::Exist(&trans, sizeof(unsigned int));
}

unsigned int CTransac::Check()
{
	unsigned int trans;

	if(CWTimer::Check(&trans, sizeof(unsigned int), NULL, NULL, 0, NULL))
	{
		return trans;
	}
	else
	{
		return 0;
	}
}

