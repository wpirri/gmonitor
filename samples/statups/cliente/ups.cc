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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>

#include <gmonitor/gmc.h>

#define PWRS_NORMAL     0
#define PWRS_UPS        1
#define PWRS_BAT        2
#define PWRS_UNKNOW     3
#define STATUS_FILE "/tmp/ups_status"

void WriteStatus(int st);

int main(int argc, char** argv)
{
        CGMInitData gminit;
        CGMClient *pClient;
  CGMError gmerror;

        CGMBuffer query;
        CGMBuffer response;

        int rc;
  int i;
  int status;
  int old_status;
  int intervalo;
  int help;
  int test;

  char strtemp[256];

  signal(SIGALRM,         SIG_IGN);
  signal(SIGPIPE,         SIG_IGN);
  intervalo = 10;
  test = 0;
  help = 0;

  for(i = 1; i < argc; i++)
  {
    if(!strcmp(argv[i], "-h"))
    {
      i++;
      gminit.m_host = argv[i];
    }
    else if(!strcmp(argv[i], "-i"))
    {
      i++;
      intervalo = atoi(argv[i]);
    }
    else if(!strcmp(argv[i], "-t"))
    {
      test = 1;
    }
    else
    {
      help = 1;
    }
  }

  if(help)
  {
    fprintf(stderr, "Uso: %s [-h host] [-i intervalo] [-t]\n", argv[0]);
    fprintf(stderr, "     donde 'host':      nombre o dirección del server\n");
    fprintf(stderr, "     donde 'intervalo': intervalo entre consultas\n");
    fprintf(stderr, "     donde -t:          coloca al servicio en modo test\n");
    exit(1);
  }

  if( !test)
  {
    /* forkeo y termino el padre para quedar como demonio */
    if(fork()) exit(0);
    /* cierror std in/out y stderr para desengancharme de la terminal */
    close(1);
    close(2);
  }

  pClient = new CGMClient(&gminit);

  old_status = -1;
        while(1)
        {
    query.Clear();
    response.Clear();

                rc = pClient->Call("status_ups", query, response, 30000);
                if(rc == 0)
                {
      memcpy(&status, response.Data(), sizeof(int));
      WriteStatus(status);
      if(status != old_status)
      {
        switch(status)
        {
        case PWRS_NORMAL:
          if(test)
          {
            printf("PWRS_NORMAL\n");
          }
          else
          {
            system("/etc/init.d/powerfail stop");
          }
          break;
        case PWRS_UPS:
          if(test)
          {
            printf("PWRS_UPS\n");
          }
          else
          {
            system("/etc/init.d/powerfail start");
          }
          break;
        case PWRS_BAT:
          if(test)
          {
            printf("PWRS_BAT\n");
          }
          else
          {
            system("/etc/init.d/powerfail now");
          }
          break;
        case PWRS_UNKNOW:
          if(test)
          {
            printf("PWRS_UNKNOW\n");
          }
          break;
        }
        old_status = status;
      }
                }
    else
    {
      if(test) printf("Error %i, %s\n", rc, gmerror.Message(rc).c_str());
    }
                sleep(intervalo);
        }
        delete pClient;
        return 0;
}

void WriteStatus(int st)
{
  FILE *arch;
  char buffer[256];
  time_t t;

  if((arch = fopen(STATUS_FILE, "wb")) == NULL) return;
  t = time(&t);
  switch(st)
  {
  case PWRS_NORMAL:
    sprintf(buffer, "PWRS_NORMAL|%s", asctime(localtime(&t)));
    break;
  case PWRS_UPS:
    sprintf(buffer, "PWRS_UPS|%s", asctime(localtime(&t)));
    break;
  case PWRS_BAT:
    sprintf(buffer, "PWRS_BAT|%s", asctime(localtime(&t)));
    break;
  case PWRS_UNKNOW:
    sprintf(buffer, "PWRS_UNKNOW|%s", asctime(localtime(&t)));
    break;
  default:
    sprintf(buffer, "0x02X|%s", st, asctime(localtime(&t)));
    break;
  }
  fwrite(buffer, 1, strlen(buffer), arch);
  fclose(arch);
}

