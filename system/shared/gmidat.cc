/***************************************************************************
    Copyright (C) 2003  Walter Pirri

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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/*
    Esta clase implementa el objeto necesario para inicializar al cliente
*/
#include "gmidat.h"

#include <string>
#include <iostream>
using namespace std;

CGMInitData::CGMInitData(CGMInitData *init_data)
{
	if(!init_data)
	{
		m_host = "localhost";
		m_port = 5533;
		m_user = "";
		m_client = "";
		m_key = "";
		m_group = "";
		m_flags = 0;
	}
	else
	{
		m_host = init_data->m_host;
		m_port = init_data->m_port;
		m_user = init_data->m_user;
		m_client = init_data->m_client;
		m_key = init_data->m_key;
		m_group = init_data->m_group;
		m_flags = init_data->m_flags;
	}
}

CGMInitData::~CGMInitData()
{
}

bool CGMInitData::IsVerbose()
{
  return ((m_flags & GMFLAG_VERBOSE) != 0);
}
