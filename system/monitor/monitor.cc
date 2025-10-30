/***************************************************************************
 * Copyright (C) 2024 Walter Pirri
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

//#include <gmerror.h>
#include <gmontdb.h>
#include <gmstring.h>
#include <sincmem.h>
#include <glog.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/msg.h>

#define INDEX_SERVER( a ) (sizeof(CGMTdb::CSystemInfo) + ( a * sizeof(SH_SERVER)))
#define INDEX_FUNCTION( a ) (sizeof(CGMTdb::CSystemInfo) + ( a * sizeof(SH_FUNCION)) + (MAX_SERVERS * sizeof(SH_SERVER) ))

typedef struct _SH_SERVER
{
  char	nombre[33];
  char	descripcion[256];
  int		modo;
  char	path[256];
  int cola[MAX_SERVER_INSTANCES]; /*para guardar el key de la cola*/
  int qid[MAX_SERVER_INSTANCES];  /*para guardar el msqid por si hay que eliminarla desde afuera*/
  int pid[MAX_SERVER_INSTANCES]; /*para guardar el pid del proceso*/
} SH_SERVER;

typedef struct _SH_FUNCION
{
  char	nombre[33];
  char	descripcion[256];
  char	tipo_mensaje;
  char	server[33];
  int		suscripciones;
} SH_FUNCION;

int exit_flag;
CGLog* pLog;

void OnReload(int /*sig*/)
{
  signal(SIGHUP,  OnReload);


}

void OnClose(int /*sig*/)
{
  pLog->Add(10, "Finalizando monitor de procesos");
  delete pLog;
  exit(0);
}

void OnChildExit(int /*sig*/)
{
  int st;
  wait(&st);
  signal(SIGCHLD, OnChildExit);

  while(waitpid(-1, &st, WNOHANG|WUNTRACED)>0);
}

int LoadServerTable()
{
  CGMTdb::CSrvTab t;
  ifstream in;
  string s;
  vector<string> vs;

  in.open(MONITOR_CONFIG_PATH "/" GM_FILENAME_SERVER_DB);
  if(! in.is_open())
  {
    return -1;
  }
  if(pLog)
  {
    pLog->Add(50, "CGMTdb::LoadSrv");
  }
  while(getline(in, s))
  {
    if(s.length() > 0 && s[0] != '#') /* salteo comentarios y lineas vacias */
    {
      vs = split(s, '|');
      if(vs.size() < 4)
      {
        in.close();
        pLog->Add(1, "ERROR al parsear configuracion de server");
        return -1;
      }
      t.nombre = vs[0];
      t.descripcion = vs[1];
      t.modo = atoi(vs[2].c_str());
      t.path = vs[3];





    }
  }
  in.close();
  return 0;
}

void StartServer(const char* server)
{

}

void SystemInfo( void )
{
  CGMTdb* pConfig;
  CGMTdb::CSystemInfo si;
  struct tm *p_tm;

  pConfig = new CGMTdb(MONITOR_CONFIG_PATH,MAX_SERVERS, MAX_SERVICES, pLog);
  if(pConfig)
  {
    if(pConfig->Open() == 0)
    {
      if(pConfig->GetSysInfo(&si) == 0)
      {
        p_tm = localtime(&si.start_time);
        pLog->Add(20, "[SYSINFO] Start: %02i/%02i/%04i %02i:%02i:%02i - TPS: %5i - MAX_TPS: %5i - LOGLEVEL: %3i", 
                      p_tm->tm_mday, p_tm->tm_mon+1, p_tm->tm_year+1900, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec,
                      si.load_tps, si.max_tps, si.log_level);

        if(pLog->LogLevel() != si.log_level)
        {
          pLog->LogLevel(si.log_level);
        }
      }
      else
      {
        pLog->Add(1, "ERROR: al obtener informacion del sistema");
      }
    }
    else
    {
      pLog->Add(1, "ERROR: al abrir area de configuracion");
    }
    delete pConfig;
  }
  else
  {
    pLog->Add(1, "ERROR: al acoplarse al area de configuracion");
  }
}

void CheckServers( void )
{
  int i;
  int j;
  CSincMem* pSHM;
  SH_SERVER server;
  struct msqid_ds qds;
  char s[256];

  pSHM = new CSincMem(GM_CONFIG_KEY);
  /* creo un espacio de memoria suficientemente
  grande para contener todos los servers y todos los servicios */
  if(pSHM->Open(  sizeof(CGMTdb::CSystemInfo) +
                      (sizeof(SH_SERVER)*MAX_SERVERS) +
                      (sizeof(SH_FUNCION)*MAX_SERVICES)
                    ) != 0) return;

  for(i = 0; i < MAX_SERVERS; i++)
  {
    if(pSHM->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en CGMTdb::Dump INDEX_SERVER");
      break;
    }
    if(strlen(server.nombre))
    {
      pLog->Add(100, "Server:        %s", server.nombre);
      pLog->Add(100, "  Descripcion: %s", server.descripcion);
      pLog->Add(100, "  Modo:        %i", server.modo);
      pLog->Add(100, "  Path:        %s", server.path);
      sprintf(s, "Server: %-20.20s", server.nombre);
      for(j = 0; j < MAX_SERVER_INSTANCES; j++)
      {
        if(server.cola[j] > 0)
        {
          pLog->Add(100, "  Instancia %i: ", j); 
          sprintf(&s[strlen(s)], " - [%i] PID: %06i ", j, server.pid[j]);
          if(kill(server.pid[j], 0) == 0)
          {
            pLog->Add(100, "    Pid:  %6i  Ok", server.pid[j]);
            strcat(s, "Ok    ");
          }
          else
          {
            pLog->Add(100, "    Pid:  %6i  Error", server.pid[j]);
            strcat(s, "Error ");
            /* Hay que limpiar esta instancia de este server */





          }
          if(msgctl(server.qid[j], IPC_STAT, &qds) != 0)
          {
            pLog->Add(100, "    Cola: Error");
            strcat(s, " Q: Error ");
          }
          else
          {
            pLog->Add(100, "    Cola: %li msgs", qds.msg_qnum);
            sprintf(&s[strlen(s)], " Q: %li msgs ", qds.msg_qnum);
          }
        }
      }
      pLog->Add(1, "[MONITOR] %s", s);
    }
  }
  pSHM->Close();
  delete pSHM;
}

int main(int /*argc*/, char** /*argv*/)
{
  /* Capturo las señales que necesito */
  signal(SIGPIPE, OnClose);
  signal(SIGILL,  OnClose);
  signal(SIGQUIT, OnClose);
  signal(SIGINT,  OnClose);
  signal(SIGTERM, OnClose);
  signal(SIGSEGV, OnClose);
  signal(SIGTSTP, OnClose);
  signal(SIGHUP,  OnReload);
  signal(SIGCHLD, OnChildExit);

  pLog = new CGLog("monitor", LOG_FILES, 100);
  pLog->Add(10, "Inicio monitor de procesos");

  exit_flag = 0;

  while(1)
  {
    SystemInfo();
    CheckServers();


    sleep(10);
  }

  return 0;
}
