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

/*
  En este server se pretende implementar el soporte necesario para la
  funcionalidad de control de transacciones.
  Se implementa asignando un id unico a cada transacción en el comienzo de
  esta y un control de tiempo limite para la finalización de la ejecución de
  la misma.
  El server recibirá los ensajes ".begintrans", ".committrans" y ".aborttrans"
  en modo Consulta/Respuesta.
  ".begintrans" - asigna un nuevo id de transaccion.
  ".comittrans" - finaliza una transacción de forma exitosa notificando a
    todos los servidores.
  ".aborttrans" - finaliza una transacción anulando los cambias que esta haya
    provocado.
  El server a la vez de contestar al cliente notifica a los demas servidores
  enviando el mismo mensaj pero en modo evento pasando como dato el id de la
  transacción.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <gmerror.h>
#include <gmontdb.h>
#include <gmstring.h>

#include <gmswaited.h>

#include "transac.h"

#include <string>
#include <iostream>
#include <csignal>
using namespace std;

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

CGMServerWait *m_pServer;
CTransac *m_pTransac;
void OnClose(int sig);

int main(int argc, char** argv, char** env)
{
/*
  int rc;
*/
  char fn[33];
  char typ;
  unsigned long inlen;
  char in[256];
  long indata;
  unsigned long trans_to;
  int wait;

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
  m_pServer->Init("gm_transac");

  m_pServer->m_pLog->Add(1, "Iniciando server");
  if(m_pServer->Suscribe(".begintrans", GM_MSG_TYPE_CR) != GME_OK)
    m_pServer->m_pLog->Add(1, "ERROR al suscribir a .begintrans");
  if(m_pServer->Suscribe(".committrans", GM_MSG_TYPE_NOT) != GME_OK)
    m_pServer->m_pLog->Add(1, "ERROR al suscribir a .committrans");
  if(m_pServer->Suscribe(".aborttrans", GM_MSG_TYPE_NOT) != GME_OK)
    m_pServer->m_pLog->Add(1, "ERROR al suscribir a .aborttrans");
  m_pTransac = new CTransac(m_pServer);

  wait = -1;
  do
  {
    m_pServer->m_pLog->Add(100, "Proximo time-out en %i centesimas (-1 = infinito)", wait);
    switch(m_pServer->Wait(fn, &typ, in, 256, &inlen, wait))
    {
    case GME_WAIT_ERROR:
      OnClose(0);
      break;
    case GME_WAIT_OK:
/*
      rc = GME_OK;
*/
      if( !strcmp(fn, ".begintrans"))
      {
        memcpy(&indata, in, sizeof(long));
        m_pServer->m_ClientData.m_trans = m_pTransac->New(indata);
        /*contesto*/
        if(m_pServer->Resp(NULL, 0, GME_OK) != GME_OK)
        {
	  /* error al responder */
	  m_pServer->m_pLog->Add(15, "ERROR al responder mensaje .begintrans");
        }
        m_pServer->Post(".begintrans",
            &m_pServer->m_ClientData.m_trans,
            sizeof(unsigned int));
        m_pServer->m_pLog->Add(100, "Inicio transaccion %u por %li segundos",
              m_pServer->m_ClientData.m_trans,
              indata);
      }
      else if( !strcmp(fn, ".committrans"))
      {
        if(m_pTransac->Del(m_pServer->m_ClientData.m_trans) == 0)
        {
          m_pServer->Post(".committrans",
              &m_pServer->m_ClientData.m_trans,
              sizeof(unsigned int));
          m_pServer->m_pLog->Add(100, "Transaccion %u finalizada",
                m_pServer->m_ClientData.m_trans);
        }
        else
        {
          m_pServer->m_pLog->Add(15, "ERROR: Se intento finalizar la transaccion %u que no existe",
                m_pServer->m_ClientData.m_trans);
/*
          rc = GME_UNKNOWN_TRAN;
*/
        }
      }
      else if( !strcmp(fn, ".aborttrans"))
      {
        if(m_pTransac->Del(m_pServer->m_ClientData.m_trans) == 0)
        {
          m_pServer->Post(".aborttrans",
              &m_pServer->m_ClientData.m_trans,
              sizeof(unsigned int));
          m_pServer->m_pLog->Add(100, "Transaccion %u abortada",
                m_pServer->m_ClientData.m_trans);
        }
        else
        {
          m_pServer->m_pLog->Add(15, "ERROR: Se intento abortar la transaccion %u que no existe",
                m_pServer->m_ClientData.m_trans);
/*
          rc = GME_UNKNOWN_TRAN;
*/
        }
      }
/*
      else
      {
        rc = GME_SVC_UNRESOLVED;
      }
*/
      /* en este case no va break para que despues procese timeout */
    case GME_WAIT_TIMEOUT:
      m_pServer->m_pLog->Add(100, "Verificando transacciones vencidas");
      while((trans_to = m_pTransac->Check()) > 0)
      {
        m_pServer->Post(".aborttrans", &trans_to, sizeof(unsigned int));
        m_pServer->m_pLog->Add(100, "Transaccion %lu abortada time-out", trans_to);
      }
      break;
    }
    wait = m_pTransac->NextTo();
    if(wait > 0) wait *= 100;
  } while(true);
  return 0;
}

void OnClose(int sig)
{
  m_pServer->m_pLog->Add(1, "Terminando server");
  m_pServer->UnSuscribe(".aborttrans", GM_MSG_TYPE_NOT);
  m_pServer->UnSuscribe(".committrans", GM_MSG_TYPE_NOT);
  m_pServer->UnSuscribe(".begintrans", GM_MSG_TYPE_CR);
  delete m_pServer;
  exit(0);
}

