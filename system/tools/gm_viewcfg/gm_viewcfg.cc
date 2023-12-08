/***************************************************************************
    Copyright (C) 2004   Walter Pirri

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

/* para acceder por monitor */
#include <gmc.h>
#include <svcstru.h>

/* para acceder por memoria */
#include <gmontdb.h>

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>

int RemoteSrv(CGMInitData* pGmInit);
int LocalSrv(void);
int RemoteSvc(CGMInitData* pGmInit);
int LocalSvc(void);

int main(int argc, char** argv)
{
  int i;
  int help, remote;
  int get_srv = 0;
  int get_svc = 0;
  CGMInitData gminit;

  help = 0;
  remote = 0;
  gminit.m_user = "GNU-MONITOR";
  gminit.m_client = "VIEWCFG";
  gminit.m_key = "****************";
  gminit.m_group = "TOOLS";
  /*gminit.m_flags = GMFLAG_VERBOSE;*/

  /*pLog = NULL;*/
  /* recorro los parametros */
  for(i = 1; i < argc; i++)
  {
    if(!strcmp("-h", argv[i]) || !strcmp("--help", argv[i]))
    {
      help = 1;
    }
    else if(!strcmp("-m", argv[i]) || !strcmp("--monitor", argv[i]))
    {
      i++;
      gminit.m_host = argv[i];
      remote = 1;
    }
    else if(!strcmp("-r", argv[i]) || !strcmp("--remote", argv[i]))
    {
      remote = 1;
    }
    else if(!strcmp("--srv", argv[i]))
    {
      get_srv = 1;
    }
    else if(!strcmp("--svc", argv[i]))
    {
      get_svc = 1;
    }
    else help = 1;
  }
  if(help)
  {
    fprintf(stderr, "Gnu-Monitor - Sistema Monitor Transaccional\n");
    fprintf(stderr, "Herramienta de consulta de configuracion de servers y servicios.\n");
    fprintf(stderr, "Use: %s [-h][-m|-r]\n", argv[0]);
    fprintf(stderr, "     -m|--monitor: nombre de host donde corre Gnu-Monitor(*).\n");
    fprintf(stderr, "     -r|--remote:  evita consultar en forma local.\n");
    fprintf(stderr, "     --srv:        trae informacion de los servers.\n");
    fprintf(stderr, "     --svc:        trae informacion de los servicios.\n");
    fprintf(stderr, "     -h|--help:    muestra esta ayuda.\n");
    fprintf(stderr, "  (*) Si no se espesifica el parametro -m la herramienta intentara"
                    " acceder al area de memoria compartida local para obtener los datos"
                    " salvo que se especifique el parametro -r.\n");
    fprintf(stderr, "      Si no se espesifica --srv o --svc trae ambos.\n");
    exit(1);
  }
  if(get_srv == 0 && get_svc == 0)
  {
    get_srv = 1;
    get_svc = 1;
  }
  if(remote)
  {
    if(get_srv) RemoteSrv(&gminit);
    if(get_svc) RemoteSvc(&gminit);
  }
  else
  {
    if(get_srv) LocalSrv();
    if(get_svc) LocalSvc();
  }
  return 0;
}

int RemoteSrv(CGMInitData* pGmInit)
{
  int rc;
  CGMClient *pClient;
  CGMError gmerror;
  CGMBuffer response;

  pClient = new CGMClient(pGmInit);

  response.Clear();
  rc = pClient->Call(".get_server_list", response, response, 300);
  if(rc == GME_OK)
  {
    fprintf(stdout, "%s", response.C_Str());
  }
  else
  {
    fprintf(stderr, "ERROR: Recv() - %s\n", gmerror.Message(rc).c_str());
  }
  delete pClient;
  return rc;
}

int RemoteSvc(CGMInitData* pGmInit)
{
  int rc;
  CGMClient *pClient;
  CGMError gmerror;
  CGMBuffer response;

  pClient = new CGMClient(pGmInit);

  response.Clear();
  rc = pClient->Call(".get_service_list", response, response, 300);
  if(rc == GME_OK)
  {
    fprintf(stdout, "%s", response.C_Str());
  }
  else
  {
    fprintf(stderr, "ERROR: Recv() - %s\n", gmerror.Message(rc).c_str());
  }
  delete pClient;
  return rc;
}

int LocalSrv(void)
{
  CGMTdb config(MONITOR_CONFIG_PATH, MAX_SERVERS, MAX_SERVICES/*, pLog*/);

  if(config.Open() != 0)
  {
    fprintf(stderr, "ERROR: area de memoria de configuracion no existe\n");
    return 1;
  }
  return config.DumpSrv(stdout, stderr);
}

int LocalSvc(void)
{
  CGMTdb config(MONITOR_CONFIG_PATH, MAX_SERVERS, MAX_SERVICES/*, pLog*/);

  if(config.Open() != 0)
  {
    fprintf(stderr, "ERROR: area de memoria de configuracion no existe\n");
    return 1;
  }
  return config.DumpSvc(stdout, stderr);
}
