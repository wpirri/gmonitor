/***************************************************************************
    Copyright (C) 2003   Walter Pirri

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

#include "gmtimer.h"

#include <gmerror.h>
#include <gmontdb.h>
#include <gmstring.h>

#include <gmswaited.h>

#include <string>
#include <iostream>
#include <csignal>
using namespace std;

#include <unistd.h>
#include <stdlib.h>

CGMServerWait *m_pServer;
CGMTimer* m_pGMTimer;

void OnClose(int sig);
char m_pInBuffer[GM_COMM_MSG_LEN];
char m_pOutBuffer[GM_COMM_MSG_LEN];
/*char *m_pOutBuffer;*/

int main(int /*argc*/, char** /*argv*/, char** /*env*/)
{
  int rc;
  char fn[33];
  char typ;
  unsigned long inlen;
  unsigned long outlen;
  long wait;

  signal(SIGPIPE, SIG_IGN);
  signal(SIGKILL,         OnClose);
  signal(SIGTERM,         OnClose);
  /*
  signal(SIGSTOP,         OnClose);
  signal(SIGABRT,         OnClose);
  signal(SIGQUIT,         OnClose);
  signal(SIGINT,          OnClose);
  signal(SIGILL,          OnClose);
  signal(SIGFPE,          OnClose);
  signal(SIGSEGV,         OnClose);
  signal(SIGBUS,          OnClose);
  */

  m_pServer = new CGMServerWait;
  m_pServer->Init("gm_timer");
  m_pServer->SetTransMode(true);
  m_pServer->m_pLog->Add(1, "Iniciando server");

  m_pGMTimer = new CGMTimer(m_pServer, m_pServer->m_pLog);

  if(( rc =  m_pServer->Suscribe(".set_timer", GM_MSG_TYPE_CR)) != GME_OK)
    m_pServer->m_pLog->Add(1, "ERROR al suscribir a .set_timer");
  if(( rc = m_pServer->Suscribe(".kill_timer", GM_MSG_TYPE_NOT)) != GME_OK)
    m_pServer->m_pLog->Add(1, "ERROR al suscribir a .kill_timer");


  wait = -1;
  while((rc = m_pServer->Wait(fn, &typ, m_pInBuffer, 1024, &inlen, wait)) >= 0)
  {
    if(rc > 0)
    {
      m_pServer->m_pLog->Add(100, "RECIBIDO %s, 0x%X, %lu", fn, m_pInBuffer, inlen);
      /* proceso el mensaje que lleg� */
      rc = m_pGMTimer->Message(fn, m_pInBuffer, inlen,
                   (void**)&m_pOutBuffer, &outlen,
                  &m_pServer->m_ClientData);
      /*contesto*/
      if(m_pServer->Resp(m_pOutBuffer, outlen, rc) != GME_OK)
      {
        /* error al responder */
        m_pServer->m_pLog->Add(15, "ERROR al responder mensaje [%s] typ: %c", fn, typ);
      }
    }
    /* Proceso vencimiento de timers */
    wait = m_pGMTimer->TimeOut();
    m_pServer->m_pLog->Add(100, "Proximo evento en %li centesimas de segundos", wait);
  }
  OnClose(0);
  return 0;
}

void OnClose(int /*sig*/)
{
  m_pServer->m_pLog->Add(1, "Terminando server");
  /*if(m_pOutBuffer) free(m_pOutBuffer);*/
  m_pServer->UnSuscribe(".kill_timer", GM_MSG_TYPE_NOT);
  m_pServer->UnSuscribe(".set_timer", GM_MSG_TYPE_CR);
  delete m_pGMTimer;
  delete m_pServer;
  exit(0);
}

