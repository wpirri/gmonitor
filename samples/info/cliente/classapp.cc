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

#include <gmonitor/svcstru.h>

#include <iostream>
using namespace std;

#include <stdio.h>

CClassApp::CClassApp(){ }
CClassApp::~CClassApp(){ }

int CClassApp::Main(int argc, char** argv, char** env)
{
	int rc;
	CGMInitData gminit;
	CGMClient *pClient;
	CGMError gmerror;
	char *pBuffer;
	unsigned long buffer_len;

	ST_SBUFFER* st_ibuffer;
	ST_SBUFFER* st_obuffer;

	gminit.m_user = "GMINFO.CGI";
	gminit.m_client = "CLIENT";
	gminit.m_key = "KEY";
	gminit.m_group = "GROUP";

	pClient = new CGMClient(&gminit);

	/* Para probar server de buffers
	st_ibuffer = (ST_SBUFFER*)calloc(2048,1);

	st_ibuffer->new_buffer.len = 16;
	memcpy(&st_ibuffer->new_buffer.data[0], "sabadabaSABADABA", st_ibuffer->new_buffer.len);

	rc = pClient->Call(".new_buffer",
					   (const char*)st_ibuffer, st_ibuffer->new_buffer.len + sizeof(ST_SBUFFER),
					   (char**)&st_obuffer, &buffer_len, 3000);
	if(rc == 0)
	{
		cout << "<i> Buffer ID -> " << st_obuffer->new_buffer.id << endl;
		free(st_ibuffer);
	}
	else
	{
		cout << "<!> Error en Call -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
	}
	*/

	/* Para probar server de buffers
	st_ibuffer = (ST_SBUFFER*)calloc(sizeof(ST_SBUFFER),1);

	st_ibuffer->get_buffer.id = 2;
	st_ibuffer->get_buffer.offset = 0;
	st_ibuffer->get_buffer.maxlen = 1024;

	rc = pClient->Call(".get_buffer",
					   (const char*)st_ibuffer, st_ibuffer->get_buffer.len + sizeof(ST_SBUFFER),
					   (char**)&st_obuffer, &buffer_len, 3000);
	if(rc == 0)
	{
		cout << "<i> Buffer 2 -> " << ((const char*)&st_obuffer->get_buffer.data[0]) << endl;
		cout << "<i> Buffer 2 len -> " << st_obuffer->get_buffer.len << endl;
		free(st_ibuffer);
	}
	else
	{
		cout << "<!> Error en Call -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
	}
	*/

	if((rc = pClient->Connect(".get_server_list", 64)) != GME_OK)
	{
		cout << "<!> Error en Connect -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
		delete pClient;
		return 0;
	}
	if((rc = pClient->Send("", 0)) != GME_OK)
	{
		cout << "<!> Error en Send -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
		pClient->Discon();
		delete pClient;
		return 0;
	}
	fwrite("<?xml version=\"1.0\"?>\n", 22, sizeof(char), stdout);
	fwrite("<info>\n", 7, sizeof(char), stdout);
	while((rc = pClient->Recv(&pBuffer, &buffer_len, 300)) == GME_MORE_DATA)
	{
		fwrite(pBuffer, buffer_len, sizeof(char), stdout);
		pClient->Free(pBuffer);
	}
	if(rc == GME_OK)
	{
		fwrite(pBuffer, buffer_len, sizeof(char), stdout);
		pClient->Free(pBuffer);
	}
	else
	{
		cout << "<!> Error en Recv -> " << rc << endl;
		cout << "    ERROR: " << gmerror.Message(rc) << endl;
	}
	pClient->Discon();
	fwrite("</info>\n", 8, sizeof(char), stdout);

	delete pClient;
	return 0;
}
