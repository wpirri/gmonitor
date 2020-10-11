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
#include "classapp.h"

#include <gmc.h>

#include <iostream>
using namespace std;

#include <string.h>

CClassApp::CClassApp(){ }
CClassApp::~CClassApp(){ }

int CClassApp::Main(int argc, char** argv, char** env)
{
	int rc = 0;
	int i;
	CGMInitData gminit;
	CGMClient *pClient;
	CGMError gmerror;
  string loglevel;

	gminit.m_user = "GMLOGLEVEL";
	gminit.m_client = "CLIENT";
	gminit.m_key = "KEY";
	gminit.m_group = "GROUP";
  loglevel = "50";

	for(i = 1; i < argc; i++)
	{
		if( ! strcmp("-h", argv[i]))
		{
			i++;
			gminit.m_host = argv[i];
		}
		else
		{
      loglevel = argv[i];
		}
	}
  pClient = new CGMClient(&gminit);
	rc = pClient->Post(".log-level", loglevel.c_str(), loglevel.length());
	if(rc != 0)
	{
		cout << "<!> Error -> " << rc << endl;
	}
	else
	{
		cout << "<i> Ok" << endl;
	}

	delete pClient;
	return 0;
}
