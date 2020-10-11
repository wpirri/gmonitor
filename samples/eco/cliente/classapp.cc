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

#include <gmonitor/gmc.h>

#include <iostream>
using namespace std;

#include <stdlib.h>
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
	string servicio;
	string msg;
	CGMBuffer response;
	CGMBuffer query;

	gminit.m_user = "GMCTEST";
	gminit.m_client = "CLIENT";
	gminit.m_key = "KEY";
	gminit.m_group = "GROUP";

	servicio = ".eco";
	msg = "la verdad de la milanesa";

	for(i = 1; i < argc; i++)
	{
		if( ! strcmp("-h", argv[i]))
		{
			i++;
			gminit.m_host = argv[i];
		}
		else if( ! strcmp("-s", argv[i]))
		{
			i++;
			servicio = argv[i];
			msg = "";
		}
		else if( ! strcmp("-d", argv[i]))
		{
			i++;
			msg = argv[i];
		}
		else
		{
			cerr << "Use: " << argv[0] << " " << "[-h host] [-s servicio] [-d dato]" << endl;
			exit(1);
		}
	}

	/* para test de server gm_default */
	pClient = new CGMClient(&gminit);

	for(i=0; i<10; i++) {

	rc = pClient->Begin(15);
	if(rc != 0)
	{
		cout << "<!> Error en Begin -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
		delete pClient;
		return 0;
	}

	response.Clear();
	query.Format("<%s> (%05i) [%03i]\n", msg.c_str(), getpid(), i);
	rc = pClient->Call(servicio, query, response, 3000);
	if(rc != 0)
	{
		cout << "<!> Error en Call -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
	}
	else
	{
		cout << "<i> Ok [" << response.Length() << "] " << response.C_Str() << endl;
	}

	if(rc == 0)
	{
		rc = pClient->Commit();
	}
	else
	{
		rc = pClient->Abort();
	}
	if(rc != 0)
	{
		cout << "<!> Error en Commit/Abort -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
		delete pClient;
		return 0;
	}

	} /* del for(... */

	/* para test de server gm_timer */
	/*
	rc = pClient->Post(".set_timer", "sabadaba", 8);
	if(rc != 0)
	{
		cout << "<!> Error -> " << rc << endl;
	}
	else
	{
		cout << "<i> Ok" << endl;
	}
	*/

	delete pClient;
	return 0;
}
