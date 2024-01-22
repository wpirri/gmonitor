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

#include <gmerror.h>
#include <gmontdb.h>
#include <glog.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

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
  int i, j, rc;
  CGMTdb* pConfig;
  vector <CGMTdb::CSrvTab> v_svr;

  pConfig = new CGMTdb(MONITOR_CONFIG_PATH,MAX_SERVERS, MAX_SERVICES, pLog);
  if(pConfig)
  {
    if(pConfig->Open() == 0)
    {
      v_svr = pConfig->ServerList(NULL, 0);
      for(i = 0; i < (int)v_svr.size(); i++)
      {
        pLog->Add(100, "[MONITOR] Verificando server %s, %i instancias.", v_svr[i].nombre.c_str(), (int)v_svr[i].cola.size());

        for(j = 0; j < (int)v_svr[i].cola.size(); j++)
        {
          if(v_svr[i].pid[j] > 0 && v_svr[i].cola[j] > 0)
          {
            rc = kill(v_svr[i].pid[j], 0);
            if(rc == 0)
            {
              pLog->Add(100, "[MONITOR] -> %i: [OK] (PID: %i Cola: 0x%08x)", (j+1), v_svr[i].pid[j], v_svr[i].cola[j]);
            }
            else
            {
              pLog->Add(100, "[MONITOR] -> %i: [ERROR] el proceso no se está ejecutando", (j+1));
              pConfig->RemoveSrv(v_svr[i].nombre, j);
              StartServer(v_svr[i].nombre.c_str());
            }
          }
          else if(v_svr[i].pid[j] <= 0)
          {
            pLog->Add(100, "[MONITOR] -> %i: [ERROR] ID de proceso invalido", (j+1));
          }
          else if(v_svr[i].cola[j] <= 0)
          {
            pLog->Add(100, "[MONITOR] -> %i: [ERROR] valor de queue invalido", (j+1));
          }
        }
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
