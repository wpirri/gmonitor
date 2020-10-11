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

CClassApp::CClassApp(){ }
CClassApp::~CClassApp(){ }

int CClassApp::Main(int argc, char** argv, char** env)
{
  int rc = 0;
  int i;
  CGMInitData gminit;
  CGMClient *pClient;
  CGMError gmerror;
  CGMClient::GMIOS data;

  gminit.m_user = "CAJERO";
  gminit.m_client = "PC-TOUCH";
  gminit.m_key = "NO KEY";
  gminit.m_group = "ATM";

  pClient = new CGMClient(&gminit);
  rc = pClient->Dequeue("turnos", &data);
  if(rc == 0)
  {
    cout << "<" << data.len << "> " << string((char*)data.data) << endl;
    pClient->Free(data);
  }
  else
  {
    cout << "<!> Error al desencolar -> " << rc << endl;
    cout << "    ERROR: " << gmerror.Message(rc) << endl;
  }
  delete pClient;
  return 0;
}
