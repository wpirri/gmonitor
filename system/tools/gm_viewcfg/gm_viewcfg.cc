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

int RemoteInfo(CGMInitData* pGmInit);
int LocalInfo(void);

int main(int argc, char** argv)
{
  int i;
  int help, remote;
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
    else help = 1;
  }
  if(help)
  {
    fprintf(stderr, "Gnu-Monitor - Sistema Monitor Transaccional\n");
    fprintf(stderr, "Herramienta de consulta de configuracion de servers y servicios.\n");
    fprintf(stderr, "Use: %s [-h][-m|-r]\n", argv[0]);
    fprintf(stderr, "     -m|--monitor: nombre de host donde corre Gnu-Monitor(*).\n");
    fprintf(stderr, "     -r|--remote:  evita consultar en forma local.\n");
    fprintf(stderr, "     -h|--help:    muestra esta ayuda.\n");
    fprintf(stderr, "  (*) Si no se espesifica el parametro -m la herramienta intentara"
                    " acceder al area de memoria compartida local para obtener los datos"
                    " salvo que se especifique el parametro -r.\n");
    exit(1);
  }
  if(remote) return RemoteInfo(&gminit);
  else return LocalInfo();
}

int RemoteInfo(CGMInitData* pGmInit)
{
  int rc;
  CGMClient *pClient;
  CGMError gmerror;
  CGMClient::_GMIOS gmio;

  pClient = new CGMClient(pGmInit);

  fprintf(stdout, "<?xml version=\"1.0\" standalone=\"yes\"?>\n");
  fprintf(stdout, "<gm_viewcfg>\n");
  if((rc = pClient->Connect(".get_server_list", 1024)) != GME_OK)
  {
    fprintf(stderr, "ERROR: Connect() - %s\n", gmerror.Message(rc).c_str());
    fprintf(stdout, "</gm_viewcfg>\n");
    delete pClient;
    return rc;
  }
  if((rc = pClient->Send(NULL, 0)) != GME_OK)
  {
    fprintf(stderr, "ERROR: Send() - %s\n", gmerror.Message(rc).c_str());
    fprintf(stdout, "</gm_viewcfg>\n");
    pClient->Discon();
    delete pClient;
    return rc;
  }
  while((rc = pClient->Recv(&gmio, 300)) == GME_MORE_DATA)
  {
    fprintf(stdout, "%*.*s", (int)gmio.len, (int)gmio.len, (char*)gmio.data);
    pClient->Free(gmio);
  }
  if(rc == GME_OK)
  {
    fprintf(stdout, "%*.*s", (int)gmio.len, (int)gmio.len, (char*)gmio.data);
    pClient->Free(gmio);
  }
  else
  {
    fprintf(stderr, "ERROR: Recv() - %s\n", gmerror.Message(rc).c_str());
    fprintf(stdout, "</gm_viewcfg>\n");
    pClient->Discon();
    delete pClient;
    return rc;
  }
  pClient->Discon();
  if((rc = pClient->Connect(".get_service_list", 1024)) != GME_OK)
  {
    fprintf(stderr, "ERROR: Connect() - %s\n", gmerror.Message(rc).c_str());
    fprintf(stdout, "</gm_viewcfg>\n");
    delete pClient;
    return rc;
  }
  if((rc = pClient->Send(NULL, 0)) != GME_OK)
  {
    fprintf(stderr, "ERROR: Send() - %s\n", gmerror.Message(rc).c_str());
    fprintf(stdout, "</gm_viewcfg>\n");
    pClient->Discon();
    delete pClient;
    return rc;
  }
  while((rc = pClient->Recv(&gmio, 300)) == GME_MORE_DATA)
  {
    fprintf(stdout, "%*.*s", (int)gmio.len, (int)gmio.len, (char*)gmio.data);
    pClient->Free(gmio);
  }
  if(rc == GME_OK)
  {
    fprintf(stdout, "%*.*s", (int)gmio.len, (int)gmio.len, (char*)gmio.data);
    pClient->Free(gmio);
  }
  else
  {
    fprintf(stderr, "ERROR: Recv() - %s\n", gmerror.Message(rc).c_str());
  }
  pClient->Discon();
  fprintf(stdout, "</gm_viewcfg>\n");
  delete pClient;
  return rc;
}

int LocalInfo(void)
{
  CGMTdb config(MONITOR_CONFIG_PATH, MAX_SERVERS, MAX_SERVICES/*, pLog*/);

  if(config.Open() != 0)
  {
    fprintf(stderr, "ERROR: area de memoria de configuracion no existe\n");
    return 1;
  }
  fprintf(stdout, "<?xml version=\"1.0\" standalone=\"yes\"?>\n");
  return config.Dump(stdout, stderr);
}
