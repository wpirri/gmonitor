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
  string dato;
  CGMInitData gminit;
  CGMClient *pClient;
  CGMError gmerror;
  CGMBuffer response;
  CGMBuffer query;

  gminit.m_user = "TURNERO";
  gminit.m_client = "PC-TOUCH";
  gminit.m_key = "NO KEY";
  gminit.m_group = "ATM";

  for(i = 1; i < argc; i++)
  {
    if( ! strcmp("-h", argv[i]))
    {
      i++;
      gminit.m_host = argv[i];
    }
    else if( ! strcmp("--consulta", argv[i]))
    {
      i++;
      dato = "consulta|";
      dato += argv[i];
    }
    else if( ! strcmp("--servicio", argv[i]))
    {
      i++;
      dato = "servicio|";
      dato += argv[i];
    }
    else if( ! strcmp("--cuenta", argv[i]))
    {
      i++;
      dato = "cuenta|";
      dato += argv[i];
    }
    else if( ! strcmp("--prestamo", argv[i]))
    {
      i++;
      dato = "prestamo|";
      dato += argv[i];
    }
    else if( ! strcmp("--help", argv[i]))
    {
      cerr << "Use: " << argv[0] << " " << "[-h host] opcion dato" << endl;
      cerr << "     opcion: --consulta" << endl;
      cerr << "     opcion: --servicio" << endl;
      cerr << "     opcion: --cuenta" << endl;
      cerr << "     opcion: --prestamo" << endl;
      exit(1);
    }
  }
  pClient = new CGMClient(&gminit);
  rc = pClient->Enqueue("turnos", dato.c_str(), dato.length());
  if( rc != 0 )
  if(rc != 0)
  {
    cout << "<!> Error al encolar -> " << rc << endl;
    cout << "    ERROR: " << gmerror.Message(rc) << endl;
  }
  delete pClient;
  return 0;
}
