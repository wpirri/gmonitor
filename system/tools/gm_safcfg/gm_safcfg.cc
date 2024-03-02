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

#include <gmc.h>
#include <svcstru.h>

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

CGMClient *g_pClient;

typedef struct _saf_info
{
  char filename[FILENAME_MAX];
  char name[32];
  unsigned long record_count;
  unsigned long data_count;
} saf_info;

int ListSaf()
{
  int rc;
  CGMClient::GMIOS gmio;

  rc = g_pClient->Call(".list-queue", NULL, 0, &gmio, 3000);
  if(rc == GME_OK)
  {
    if(gmio.len)
    {
      fprintf(stdout, "<?xml version=\"1.0\" standalone=\"yes\"?>\n");
      fprintf(stdout, "%s\n", (char*)gmio.data);
      //g_pClient->Free(gmio);
    }
    else
    {
      fprintf(stdout, "Respuesta sin datos\n");
    }
  }
  return rc;
}

int CrearSaf(const char* saf_name)
{
  return g_pClient->Notify(".create-queue", saf_name, strlen(saf_name)+1);
}

int DropSaf(const char* saf_name)
{
  return g_pClient->Notify(".drop-queue", saf_name, strlen(saf_name)+1);
}

int InfoSaf(const char* saf_name)
{
  saf_info *psi;
  int rc;
  CGMClient::GMIOS gmio;

  rc = g_pClient->Call(".info-queue", saf_name, strlen(saf_name)+1, &gmio, 3000);
  if(rc == GME_OK)
  {
    if(gmio.len)
    {
      psi = (saf_info*)gmio.data;
      fprintf(stdout, "Cola: %s\n", psi->name);
      fprintf(stdout, "Ubicaci�n: %s\n", psi->filename);
      fprintf(stdout, "Registros totales: %lu\n", psi->record_count);
      fprintf(stdout, "Registros utiles: %lu\n", psi->data_count);
      //g_pClient->Free(gmio);
    }
    else
    {
      fprintf(stdout, "Respuesta sin datos\n");
    }
  }
  return rc;
}

int main(int argc, char** argv)
{
  int i;
  int help;
  int rc;
  string last_cmd;
  CGMInitData gminit;
  CGMError gmerror;

  help = 0;
  rc = 0;
  gminit.m_user = "SAFCFG";
  gminit.m_client = "CLIENT";
  gminit.m_key = "****************";
  gminit.m_group = "TOOLS";

  g_pClient = new CGMClient(&gminit);

  if(argc <= 1) help = 1;

  /* recorro los parametros */
  for(i = 1; i < argc; i++)
  {
    if( !strcmp(argv[i], "-m") || !strcmp(argv[i], "--monitor"))
    {
      i++;
      gminit.m_host = argv[i];
    }
    else if( !strcmp(argv[i], "-l") || !strcmp(argv[i], "--list"))
    {
      rc = ListSaf();
      last_cmd = "listar receptores de SAF";
    }
    else if( !strcmp(argv[i], "-c") || !strcmp(argv[i], "--create"))
    {
      i++;
      rc = CrearSaf(argv[i]);
      last_cmd = "crear archivo de saf";
    }
    else if( !strcmp(argv[i], "-d") || !strcmp(argv[i], "--drop"))
    {
      i++;
      rc = DropSaf(argv[i]);
      last_cmd = "eliminar archivo de saf";
    }
    else if( !strcmp(argv[i], "-i") || !strcmp(argv[i], "--info"))
    {
      i++;
      rc = InfoSaf(argv[i]);
      last_cmd = "traer informacion de archivo de saf";
    }
    else
    {
      help = 1;
      break;
    }
    if(rc != 0)
    {
      fprintf(stderr, "Error al %s. %s\n", last_cmd.c_str(), gmerror.Message(rc).c_str());
    }
  }
  if(help)
  {
    fprintf(stderr, "Gnu-Monitor - Sistema Monitor Transaccional\n");
    fprintf(stderr, "Herramienta de administraci�n de archivos de SAF para servicios de Store & Forward\n");
    fprintf(stderr, "Use: %s [-m host] [-l|-c|-d|-i|-h] saf-name\n", argv[0]);
    fprintf(stderr, "     -m|--monitor: nombre de host donde corre Gnu-Monitor (default localhost).\n");
    fprintf(stderr, "     -l|--list:    lista receptores de SAF.\n");
    fprintf(stderr, "     -c|--create:  crear saf-name.\n");
    fprintf(stderr, "     -d|--drop:    eliminar saf-name y todo su contenido.\n");
    fprintf(stderr, "     -i|--info:    informacion acerca de saf-name.\n");
    fprintf(stderr, "     -h|--help:    muestra esta ayuda.\n");
    exit(1);
  }

  delete g_pClient;
  return 0;
}

