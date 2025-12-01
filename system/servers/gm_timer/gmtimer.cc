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
#include "wtimer.h"

#include <gmerror.h>
#include <gmstring.h>
#include <svcstru.h>

#include <string>
#include <iostream>
#include <csignal>
using namespace std;

#include <string.h>

CGMTimer::CGMTimer(CGMServerWait *pServer, CGLog* plog)
{
  m_pLog = plog;
  m_pTimer = new CWTimer();
  m_pServer = pServer;
}

CGMTimer::~CGMTimer()
{
  delete m_pTimer;
}

int CGMTimer::Message(const char *funcion,
            void* in, unsigned long inlen,
            void** out, unsigned long* outlen,
            CGMServerBase::CLIENT_DATA* pClientData)
{
  ST_STIMER* pt;

  /* proceso el mensaje que lleg� */
  pt = (ST_STIMER*)in;
  *outlen = 0;
  *out = NULL;
  if( !strcmp(".set_timer", funcion))
  {
    m_pLog->Add(100, ".set_timer()");
    if(inlen < sizeof(pt->set_timer))
    {
      m_pLog->Add(10, "ERROR: Datos insuficientes .set_timer()");
      return GME_UNDEFINED;
    }
    if(pt->set_timer.delay && !pt->set_timer.at)
    {
      m_pLog->Add(100, "SET TIMER    delay: %u", pt->set_timer.delay);
      m_pLog->Add(100, "          servicio: (%c)%-32.32s",
                  pt->set_timer.modo_servicio, pt->set_timer.servicio);
      m_pLog->Add(100, "              user: %-32.32s", pClientData->m_user.c_str());
      m_pLog->Add(100, "            client: %-32.32s", pClientData->m_client.c_str());
      m_pLog->Add(100, "             group: %-32.32s", pClientData->m_group.c_str());
      m_pLog->Add(100, "              data: (%ul)%-32.32s",
                  pt->set_timer.len, pt->set_timer.data);
      pt->set_timer.id = m_pTimer->Set(pt->set_timer.delay,
              pt->set_timer.servicio, pt->set_timer.modo_servicio,
              pClientData->m_trans, pClientData->m_user.c_str(),
              pClientData->m_client.c_str(), pClientData->m_key.c_str(),
              pClientData->m_group.c_str(),
              &pt->set_timer.data[0], pt->set_timer.len,
              (pt->set_timer.tipo_timer == 'R')?true:false);
    }
    else if(pt->set_timer.at && !pt->set_timer.delay)
    {
      m_pLog->Add(100, "SET TIMER       at: %ul", pt->set_timer.at);
      m_pLog->Add(100, "          servicio: (%c)%-32.32s",
                  pt->set_timer.modo_servicio, pt->set_timer.servicio);
      m_pLog->Add(100, "              user: %-32.32s", pClientData->m_user.c_str());
      m_pLog->Add(100, "            client: %-32.32s", pClientData->m_client.c_str());
      m_pLog->Add(100, "             group: %-32.32s", pClientData->m_group.c_str());
      m_pLog->Add(100, "              data: (%ul)[%-32.32s]",
                  pt->set_timer.len, pt->set_timer.data);
      pt->set_timer.id = m_pTimer->At(pt->set_timer.at,
            pt->set_timer.servicio, pt->set_timer.modo_servicio,
            pClientData->m_trans, pClientData->m_user.c_str(),
            pClientData->m_client.c_str(), pClientData->m_key.c_str(),
            pClientData->m_group.c_str(),
            &pt->set_timer.data[0], pt->set_timer.len);
    }
    else /* Hay un error con los parametros */
    {
      m_pLog->Add(10, "ERROR: En parametros de .set_timer()");
      return GME_UNDEFINED;
    }
    pt->set_timer.data[0] = '\0';
    pt->set_timer.len = 0;
    if(pt->set_timer.id == 0)
    {
      *outlen = 0;
      m_pLog->Add(10, "ERROR: No se pudo obtener un timer en .set_timer()");
      return GME_UNDEFINED;
    }
    *outlen = sizeof(ST_STIMER);
    *out = pt;
  }
  else if( !strcmp(".kill_timer", funcion))
  {
    m_pLog->Add(100, ".kill_timer()");
    if(inlen < sizeof(pt->kill_timer))
    {
      m_pLog->Add(10, "ERROR: Datos insuficientes .kill_timer()");
      return GME_UNDEFINED;
    }
    if(m_pTimer->DelId(pt->kill_timer.id, pClientData->m_user.c_str(),
      pClientData->m_client.c_str(),
       pClientData->m_key.c_str(), pClientData->m_group.c_str()) != GME_OK)
    {
      m_pLog->Add(10, "ERROR: Al borrar timer .kill_timer()");
      return GME_UNDEFINED;
    }
  }
  /* proceso los mensajes enviados por el server de transacciones */
  else if( !strcmp(".committrans", funcion) || !strcmp(".aborttrans", funcion))
  {
    if(m_pTimer->DelTrans((unsigned int)(*((unsigned int*)in))) != GME_OK)
    {
      m_pLog->Add(10, "ERROR: Al borrar timer commit/abort");
      return GME_UNDEFINED;
    }
  }
  else
  {
    m_pLog->Add(10, "ERROR: Servicio %s no resuelto por este server", funcion);
    return GME_SVC_UNRESOLVED;
  }
  return GME_OK;
}

long CGMTimer::TimeOut()
{
  /* para los timers */
  int idx;
  char servicio[33];
  char tipo;
  void* msg;
  unsigned long msg_len;
  int rc;

  /* Proceso vencimiento de timers */
  m_pLog->Add(100, "Check time-out");
  while( (idx = m_pTimer->Check()) > 0 )
  {
    if( m_pTimer->Get(idx, servicio, &tipo, 0, NULL, NULL, NULL, NULL, &msg, &msg_len) != 0)
    {
      m_pLog->Add(10, "ERROR recuperando datos del timer %i", idx);
      break;
    }
      m_pLog->Add(100, "TIMER VENCIDO   id: %i", idx);
      m_pLog->Add(100, "          servicio: (%c)%-32.32s",
                  tipo, servicio);
      m_pLog->Add(100, "              data: (%ul)[%-32.32s]",
                  msg_len, msg);
    /* inicializo rc en -1 por si no se resuelve el timer */
    rc = -1;
    switch(tipo)
    {
      /* ahora hay que mandar el mensaje */
      case GM_MSG_TYPE_CR:
        /*rc = Call(const char *fn, const char *query,
              unsigned long qlen, char **response,
              unsigned long *rlen,long to);*/
        break;
      case GM_MSG_TYPE_MSG:
        rc = m_pServer->Post(servicio, msg, msg_len);
        break;
      case GM_MSG_TYPE_INT:
        /* no va a ser posible */
        break;
      case GM_MSG_TYPE_NOT:
        rc = m_pServer->Notify(servicio, msg, msg_len);
        break;
      default:
        break;
    } /* switch(tipo) */
    if(rc != 0)
    {
      /* Error al ejecutar el mensaje del timer */
      m_pLog->Add(10, "ERROR %i svc: %s typ: %c", rc, servicio, tipo);
      if(rc > 0)
      {


      }
    }
    m_pTimer->Ok(idx);
  }
  return m_pTimer->Rest();
}
