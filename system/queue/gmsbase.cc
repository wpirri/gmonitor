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

#include "gmsbase.h"

#include <gmconst.h>
#include <gmontdb.h>
#include <gmbuffer.h>
#include <cmsg.h>
#include <gmisc.h>
#include <gmerror.h>
#include <gmessage.h>
#include <glog.h>
#include <svcstru.h>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>


CGMServerBase::CGMServerBase()
{
  m_monitor_path = BIN_MONITOR;
  m_transactional_mode = false;
  m_pConfig = NULL;
}

CGMServerBase::~CGMServerBase()
{
  SetTransMode(false);
  /*  delete m_pMsg;*/
  delete m_pConfig;
}

int CGMServerBase::LoadConfig()
{
  extern CGLog* pLog;

  m_pConfig = new CGMTdb(NULL, MAX_SERVERS, MAX_SERVICES, pLog);
  m_pConfig->Open();
  return 0;
}

/* modo eventos */
int CGMServerBase::Notify(const char *event, const void *data, unsigned long len)
{
  CGMessage omsg;
  CGMessage imsg;
  CMsg qmsg;
  char resp_buffer[GM_COMM_MSG_LEN];
  long resp_len;

  omsg.IdUsuario(m_IdUsuario.c_str());
  omsg.IdCliente(m_IdCliente.c_str());
  omsg.Key(m_Key.c_str());
  omsg.IdGrupo(m_IdGrupo.c_str());
  omsg.IdTrans(0);
  omsg.IdCola(0);
  omsg.IdOrigen(getpid());
  omsg.IdRouter(0);
  omsg.TimeStamp(0);
  omsg.CodigoRetorno(0);
  omsg.TipoMensaje(GM_MSG_TYPE_NOT);
  omsg.SecuenciaConsulta(1);
  omsg.SecuenciaRespuesta(0);
  omsg.OrigenConsulta(GM_ORIG_SVR);
  omsg.Funcion(event);
  omsg.SetData(data, len);
  /* Hago la consulta a la cola del router */
  resp_len = qmsg.Query(qmsg.GetRemoteKey(BIN_MONITOR), 
                        omsg.GetMsg(), omsg.GetMsgLen(), 
                        &resp_buffer[0], GM_COMM_MSG_LEN, 3000);
  if(resp_len > 0)
  {
    imsg.SetMsg(&resp_buffer[0], resp_len);
  }
  else
  {
    imsg.SetResponse(&omsg);
    imsg.CodigoRetorno(GME_MSGQ_ERROR);
  }
  return imsg.CodigoRetorno();
}

int CGMServerBase::Notify(string& event, CGMBuffer& data)
{
  return Notify(event.c_str(), data.C_Str(), data.Length());
}

int CGMServerBase::Notify(string& event, CGMBuffer* data)
{
  if(data)
  {
    return Notify(event.c_str(), data->C_Str(), data->Length());
  }
  else
  {
    return Notify(event.c_str(), NULL, 0);
  }
}

/*
int CGMServerBase::Broadcast(const char *user, const char *client,
               const char *group, const void *data, unsigned long len)
{
  return 0;
}
*/

int CGMServerBase::Post(const char *event, const void *data, unsigned long len)
{
  CGMessage omsg;
  CGMessage imsg;
  CGMBuffer ibuff;
  CGMBuffer obuff;
  CMsg qmsg;

  /* Inicializo el mensaje */
  omsg.IdUsuario(m_IdUsuario.c_str());
  omsg.IdCliente(m_IdCliente.c_str());
  omsg.Key(m_Key.c_str());
  omsg.IdGrupo(m_IdGrupo.c_str());
  omsg.IdTrans(0);
  omsg.IdCola(0);
  omsg.IdOrigen(getpid());
  omsg.IdRouter(0);
  omsg.TimeStamp(0);
  omsg.CodigoRetorno(0);
  omsg.TipoMensaje(GM_MSG_TYPE_MSG);
  omsg.SecuenciaConsulta(1);
  omsg.SecuenciaRespuesta(0);
  omsg.OrigenConsulta(GM_ORIG_SVR);
  omsg.Funcion(event);
  omsg.SetData(data, len);
  /* Hago la consulta a la cola del router */
  ibuff.Set(omsg.GetMsg(), omsg.GetMsgLen());
  if(qmsg.Query(qmsg.GetRemoteKey(BIN_MONITOR), &ibuff, &obuff, 3000) > 0)
  {
    imsg.SetMsg(&obuff);
  }
  else
  {
    imsg.SetResponse(&omsg);
    imsg.CodigoRetorno(GME_MSGQ_ERROR);
  }
  return imsg.CodigoRetorno();
}

int CGMServerBase::Post(string& event, CGMBuffer& data)
{
  return Post(event.c_str(), data.C_Str(), data.Length());
}

int CGMServerBase::Post(string& event, CGMBuffer* data)
{
  if(data)
  {
    return Post(event.c_str(), data->C_Str(), data->Length());
  }
  else
  {
    return Post(event.c_str(), NULL, 0);
  }
}

int CGMServerBase::Suscribe(const char *event, char tipo_mensaje)
{
  int rc;

  if( !m_pConfig) return -1;
  rc = m_pConfig->AddSvc(event, tipo_mensaje, m_server_name.c_str());
  return rc;
}

int CGMServerBase::UnSuscribe(const char *event, char tipo_mensaje)
{
  if( !m_pConfig) return -1;
  return m_pConfig->RemoveSvc(event, tipo_mensaje, m_server_name.c_str());
}

/* modo consulta/respuesta */
int CGMServerBase::Call(const char *fn, const void *query, unsigned long qlen, GMIOS *presp,  int to)
{
  CGMessage omsg;
  CGMessage imsg;
  CGMBuffer ibuff;
  CGMBuffer obuff;
  CMsg qmsg;

  omsg.IdUsuario(m_IdUsuario.c_str());
  omsg.IdCliente(m_IdCliente.c_str());
  omsg.Key(m_Key.c_str());
  omsg.IdGrupo(m_IdGrupo.c_str());
  omsg.IdTrans(0);
  omsg.IdCola(0);
  omsg.IdOrigen(getpid());
  omsg.IdRouter(0);
  omsg.TimeStamp(0);
  omsg.CodigoRetorno(0);
  omsg.TipoMensaje(GM_MSG_TYPE_CR);
  omsg.SecuenciaConsulta(1);
  omsg.SecuenciaRespuesta(0);
  omsg.OrigenConsulta(GM_ORIG_SVR);
  omsg.Funcion(fn);
  omsg.SetData(query, qlen);

  /* Hago la consulta a la cola del router */
  ibuff.Set(omsg.GetMsg(), omsg.GetMsgLen());
  if(qmsg.Query(qmsg.GetRemoteKey(BIN_MONITOR), &ibuff, &obuff, to) > 0)
  {
    imsg.SetMsg(&obuff);
    if(presp)
    {
      //presp->data = (char*)calloc(imsg.GetDataLen(), sizeof(char));
      memcpy(presp->data, imsg.GetData(), imsg.GetDataLen());
      presp->len = imsg.GetDataLen();
    }
  }
  else
  {
    imsg.SetResponse(&omsg);
    imsg.CodigoRetorno(GME_MSGQ_ERROR);
  }
  return imsg.CodigoRetorno();
}

int CGMServerBase::Call(string& fn, CGMBuffer& query, CGMBuffer& response, int to)
{
  return Call(fn, &query, &response, to);
}

int CGMServerBase::Call(string& fn, CGMBuffer* query, CGMBuffer* response, int to)
{
  GMIOS gmio;
  int rc;

  if(query)
  {
    rc = Call((const char*)fn.c_str(), query->Data(),
          (unsigned int)query->Length(), &gmio, to);
  }
  else
  {
    rc = Call((const char*)fn.c_str(), NULL, 0, &gmio, to);
  }

  if(rc == GME_OK)
  {
    if(response)
    {
      response->Add(gmio.data, gmio.len);
    }
    //free(gmio.data);
  }
  return rc;
}

/* liberacion del buffer recibido */
/*
int CGMServerBase::Free(GMIOS s)
{
  return Free(&s);
}
*/
/* liberacion del buffer recibido */
/*
int CGMServerBase::Free(GMIOS *ps)
{
  if(ps->data && ps->len) free(ps->data);
  ps->len = 0;
  return GME_OK;
}
*/

int CGMServerBase::SetTransMode(bool on_off)
{
  if(m_transactional_mode && !on_off)
  {
    UnSuscribe(".aborttrans", GM_MSG_TYPE_MSG);
    UnSuscribe(".committrans", GM_MSG_TYPE_MSG);
    UnSuscribe(".begintrans", GM_MSG_TYPE_MSG);
  }
  else if(!m_transactional_mode && on_off)
  {
    Suscribe(".begintrans", GM_MSG_TYPE_MSG);
    Suscribe(".committrans", GM_MSG_TYPE_MSG);
    Suscribe(".aborttrans", GM_MSG_TYPE_MSG);
  }
  else
  {
    return -1;
  }
  m_transactional_mode = on_off;
  return 0;
}

int CGMServerBase::Run(const char* exe, char* const params[])
{
  int rc;
  char *p[256];
  int i;

  memset(p, 0, 256*sizeof(char*));
  p[0] = (char*)malloc(strlen(exe)+1);
  strcpy(p[0], exe);
  if(params)
  {
    for(i = 0; params[i]; i++)
    {
      p[i+1] = (char*)malloc(strlen(params[i])+1);
      strcpy(p[i+1], params[i]);
    }
  }
  /* Lanzo el proceso server */
  rc = exec_with_stdio(&m_stdin_pipe, &m_stdout_pipe, &m_stderr_pipe, exe, p);
  for(i = 0; p[i]; i++)
  {
    free(p[i]);
  }
  if(rc < 0)
  {
    return rc;
  }
  /* los pipes de los que voy a leer los pongo no bloqueantes */
  fcntl(m_stdout_pipe, F_SETFL, O_NONBLOCK);
  fcntl(m_stderr_pipe, F_SETFL, O_NONBLOCK);

  return 0;
}

/* Esta ya no se usa, ahora los mensajes se mandan a la cola del router
   la dejo porque me da lastima borrarla */
int CGMServerBase::Main(void* in, unsigned long inlen, void* out, unsigned long *outlen, unsigned long max_outlen)
{
  CGMBuffer buffer;
  char appBuffer[GM_COMM_MSG_LEN];
  int rc;

  *outlen = 0;
  buffer.Clear();
  /* mando el mensaje recibido por el pipe de entrada del proceso server */
  if(inlen > 0)
  {
    if(write(m_stdin_pipe, in, inlen) < 0)
    {
      return -1;
    }
    else
    {
      /* cierro el para que termine la entrada de datos */
      /*close(stdin_pipe);*/
    }
  }
  /* leo del pipe de salida del proceso server */
  /* espera maxima para que el server conteste */
  rc = WaitRead(m_stdout_pipe, appBuffer, sizeof(appBuffer), 6000);
  if(rc > 0)
  {
    do
    {
      buffer.Add(appBuffer, rc);
    } while((rc = WaitRead(m_stdout_pipe, appBuffer, sizeof(appBuffer), 10)) > 0);
  }
  else
  {
    /* Me fijo si tir� algo por error */
    WaitRead(m_stderr_pipe, appBuffer, sizeof(appBuffer), 1);
  }
  if(buffer.Length() <= max_outlen)
  {
    memcpy(out, buffer.Data(), buffer.Length());
    *outlen = buffer.Length();
    return 0;
  }
  else
  {
    return (-1);
  }
}
/* Esta ya no se usa, ahora los mensajes se mandan a la cola del router
   la dejo porque me da lastima borrarla */
/* el time-out to_cs es en cent�simas de segundo */
int CGMServerBase::WaitRead(int fd, void* buff, int count, long to_cs)
{
  int rc = 0;
  long tto;

  for(tto = to_cs ; tto || to_cs < 0 ; tto--)
  {
    if((rc = read(fd, buff, count)) > 0)
    {
            break;
    }
    usleep(10000l);
  }
  return rc;
}

int CGMServerBase::Enqueue(const char* queue, const void *data, unsigned long len)
{
  ST_SQUEUE st;
  int rc;

  strcpy(st.head.saf_name, queue);
  st.head.len = len;
  memcpy(&st.data[0], data, len);
  rc = Notify(".enqueue", &st, sizeof(ST_SQUEUE::head) + len);

  return rc;
}

int CGMServerBase::Enqueue(string& queue, const void *data, unsigned long len)
{
  return Enqueue((const char*)queue.c_str(), data, len);
}

int CGMServerBase::Dequeue(const char* queue, GMIOS *pdata)
{
  return Call(".dequeue", queue, strlen(queue)+1, pdata, 3000);
}

int CGMServerBase::Dequeue(string& queue, GMIOS *pdata)
{
  return Dequeue((const char*)queue.c_str(), pdata);
}

