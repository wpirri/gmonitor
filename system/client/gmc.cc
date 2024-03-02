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

#include "gmc.h"

#include <msgtype.h>
#include <svcstru.h>
#include <gmstring.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
using namespace std;

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>

/* inicializacion del driver */
CGMClient::CGMClient(CGMInitData *init_data)
{
  //m_error.Last(GME_OK);
  m_pidata = new CGMInitData(init_data);
  m_pmsgt = new CGMessage();
  m_pmsgr = new CGMessage();
  m_pmsgt->IdUsuario(m_pidata->m_user);
  m_pmsgt->IdCliente(m_pidata->m_client);
  m_pmsgt->Key(m_pidata->m_key);
  m_pmsgt->IdGrupo(m_pidata->m_group);
  m_pmsgt->CodigoRetorno(0);
  m_pSocket = NULL;
}

/* liberacion de recursos del driver */
CGMClient::~CGMClient()
{
  delete m_pmsgt;
  delete m_pmsgr;
  delete m_pidata;
  if(m_pSocket) delete m_pSocket;
}

/* liberacion del buffer recibido */
/*
int CGMClient::Free(GMIOS s)
{
  return Free(&s);
}
*/
/* liberacion del buffer recibido */
/*
int CGMClient::Free(GMIOS *ps)
{
  if(ps->data && ps->len) free(ps->data);
  ps->len = 0;
  return GME_OK;
}
*/

void CGMClient::Init(CGMInitData *init_data)
{
  /*m_error.Last(GME_OK); */
  m_pidata = new CGMInitData(init_data);
  m_pmsgt = new CGMessage();
  m_pmsgr = new CGMessage();
  m_pmsgt->IdUsuario(m_pidata->m_user);
  m_pmsgt->IdCliente(m_pidata->m_client);
  m_pmsgt->Key(m_pidata->m_key);
  m_pmsgt->IdGrupo(m_pidata->m_group);
  m_pmsgt->CodigoRetorno(0);
  m_pmsgt->IdTrans(0);
  /* este ID hay que resetear los dos juntos */
  m_pmsgt->IdMoreData(0);
  m_pmsgr->IdMoreData(0);
}

/* manejo de transaccion */
int CGMClient::Begin(long to_s)
{
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Begin(...)");
  /* controlo que no tenga una trans empesada */
  if(m_pmsgt->IdTrans() != 0)
  {
    return GME_TRAN_NOT_ALLOWED;
  }
  rc = Call(".begintrans", (const char*)&to_s, sizeof(long), NULL, to_s*100);
  if(rc == GME_OK)
  {
    /* tomo el ID de la transaccion que me viene en el header */
    if(m_pmsgt->IdTrans(m_pmsgr->IdTrans()) == 0)
    {
      /* no deberia ser 0 */
      return GME_INVALID_TRAN;
    }
  }
  return rc;
}

int CGMClient::Commit()
{
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Commit()");
  if(m_pmsgt->IdTrans() == 0)
  {
    /* debo haber iniciado una transaccion primero */
    return GME_TRAN_NOT_INIT;
  }
  rc = Notify(".committrans", NULL, 0);
  if(rc == GME_OK)
  {
    m_pmsgt->IdTrans(0);
  }
  return rc;
}

int CGMClient::Abort()
{
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Abort()");
  if(m_pmsgt->IdTrans() == 0)
  {
    /* debo haber iniciado una transaccion primero */
    return GME_TRAN_NOT_INIT;
  }
  rc = Notify(".aborttrans", NULL, 0);
  if(rc == GME_OK)
  {
    m_pmsgt->IdTrans(0);
  }
  return rc;
}

/* modo eventos - envio */
int CGMClient::Notify(const char *event, const void *data, unsigned long len)
{
  int rc;
  GMIOS gmio;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Notify(...)");
  if((rc = Connect(event)) != 0) return rc;
  m_pmsgt->TipoMensaje(GM_MSG_TYPE_NOT); /* AVISO */
  rc = Send(data, len);
  if(!rc) rc = Recv(&gmio, 3000);
  Discon();
  //Free(gmio);
  return rc;
}

int CGMClient::Notify(string& event, CGMBuffer& data)
{
  return Notify(event.c_str(), data.C_Str(), data.Length());
}

int CGMClient::Notify(string& event, CGMBuffer* data)
{
  return Notify(event.c_str(), data->C_Str(), data->Length());
}

/*
int CGMClient::Broadcast(const char *user, const char *client,
             const char *group, const char *event, const void *data, unsigned long len)
{
  return 0;
}
*/

/* ver si vale la pena ponerle time-out al post */
int CGMClient::Post(const char *event, const void *data, unsigned long len)
{
  int rc;
  GMIOS gmio;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Post(...)");
  if((rc = Connect(event)) != 0) return rc;
  m_pmsgt->TipoMensaje(GM_MSG_TYPE_MSG); /* EVENTO */
  rc = Send(data, len);
  if(!rc) rc = Recv(&gmio, 3000);
  Discon();
  //Free(gmio);
  return rc;
}
int CGMClient::Post(string& event, CGMBuffer& data)
{
  return Post(event.c_str(), data.C_Str(), data.Length());
}

int CGMClient::Post(string& event, CGMBuffer* data)
{
  return Post(event.c_str(), data->C_Str(), data->Length());
}

/* modo eventos - recepcion */
int CGMClient::Suscribe(const char *event, char typ, SERVICE_FUNCTION fcn)
{
  int rc;
  GMIOS gmio;
  char buffer[34];
  SVC_REL svc_rel;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Suscribe(...)");
  /* debe devolver el nombre del servicio de SAF asignado */
  sprintf(buffer, "%c%-32.32s", typ, event);
  rc = Call(".suscribe", buffer, strlen(buffer), &gmio, 3000);
  if(rc == GME_OK)
  {
    /* asocio el evento al SAF */
    strcpy(svc_rel.svc, event);
    strcpy(svc_rel.saf, (char*)gmio.data);
    svc_rel.fcn = fcn;
    m_svc_rel.push_back(svc_rel);
  }
  return  rc;
}

int CGMClient::Suscribe(string& event, char typ, SERVICE_FUNCTION fcn)
{
  return Suscribe(event.c_str(), typ, fcn);
}

int CGMClient::UnSuscribe(const char *event, char typ)
{
  char buffer[34];
  int i, vlen;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Unsuscribe(...)");
  /* desasocio el evento */
  vlen = m_svc_rel.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_svc_rel[i].svc, event) &&
         m_svc_rel[i].typ == typ )   break;
  }
  if(i < vlen)
  {
    m_svc_rel.erase(m_svc_rel.begin() + i);
    sprintf(buffer, "%c%-32.32s", typ, event);
    return Notify(".unsuscribe", buffer, strlen(buffer));
  }
  else
  {
    return -1;
  }
}

int CGMClient::UnSuscribe(string& event, char typ)
{
  return UnSuscribe(event.c_str(), typ);
}

int CGMClient::CheckService(long /*to_cs*/)
{
  int i, vlen;
  GMIOS data;
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::CheckService()");

  vlen = m_svc_rel.size();
  /* para cada uno de los serviciode registrados... */
  for(i = 0; i < vlen; i++)
  {
    /* si existe la funci�n callback... */
    if(m_svc_rel[i].fcn)
    {
      /* me traigo todos los mensajes que tenga encolados... */
      while( Dequeue((const char*)&m_svc_rel[i].saf[0], &data) == GME_OK )
      {
        /* y llamo a la funcion que me dijeron que lo va a atender */
        rc = (m_svc_rel[i].fcn)( m_svc_rel[i].svc,
                                 m_svc_rel[i].typ,
                                 data.data,
                                 data.len );
        //Free(data);
        /* para que las funciones puedan decir "Basta, me estas matando" */
        if(rc == 0) break;
      }
    }
  }
  return 0;
}

/* modo consulta/respuesta */
int CGMClient::Call(const char *fn, const void *query, unsigned long qlen,
          GMIOS *presp, long to_cs)
{
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Call(...)");
  if((rc = Connect(fn)) != 0) return rc;
  m_pmsgt->TipoMensaje(GM_MSG_TYPE_CR); /* CONSULTA */
  rc = Send(query, qlen);
  if(!rc) rc = Recv(presp, to_cs);
  Discon();
  return rc;
}

int CGMClient::Call(string fn, CGMBuffer& query, CGMBuffer& response, long to_cs)
{
  return Call(fn, &query, &response, to_cs);
}

int CGMClient::Call(string fn, CGMBuffer* query, CGMBuffer* response, long to_cs)
{
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Call(...)");
  if((rc = Connect(fn)) != 0) return rc;
  m_pmsgt->TipoMensaje(GM_MSG_TYPE_CR); /* CONSULTA */
  rc = Send(query);
  if(!rc) rc = Recv(response, to_cs);
  Discon();
  return rc;
}

/*
int CGMClient::ACall(const char *fn, const void *query, unsigned long qlen)
{
  return GME_OK;
}
*/

/*
int CGMClient::GetReply(GMIOS *presp, long to_cs)
{
  return GME_OK;
}
*/

/*
int CGMClient::Cancel()
{
  return GME_OK;
}
*/

/* modo interactivo */
int CGMClient::Connect(const char *fn, unsigned long transfer_len)
{
  int rc;

  if(m_pSocket) delete m_pSocket;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Connect(%s, %lu)", fn, transfer_len);
  m_pSocket = new CTcp();
  if((rc = m_pSocket->Connect("",
      m_pidata->m_host,
      m_pidata->m_port)) != 0)
  {
    delete m_pSocket;
    m_pSocket = NULL;
    /* error en la conexion */
    if(rc == CTCP_HOST_NOT_FOUND)
      return GME_HOST_NOTFOUND;
    else if(rc == CTCP_ADDRESS_NOT_FOUND)
      return GME_INVALID_HOST;
    else
      return GME_UNDEFINED;
  }

  m_pmsgt->TipoMensaje(GM_MSG_TYPE_INT); /* Interactivo por default */
  m_pmsgt->SecuenciaConsulta(0);
  m_pmsgt->SecuenciaRespuesta(0);
  m_pmsgt->Funcion(fn);
  m_pmsgt->OrigenConsulta(GM_ORIG_CLI);
  m_pmsgt->IdOrigen(getpid());
  m_pmsgt->SetData(NULL, 0);
  m_transfer_len = transfer_len;
  m_indice_mensaje = 0l;
  m_pmsgt->CodigoRetorno(0);
  /* NUNCA reseterar IdTrans salvo en el constructor y commit o abort !!! */
  /*m_pmsgt->IdTrans(0);*/ 
  /* este ID hay que resetear los dos juntos */
  m_pmsgt->IdMoreData(0);
  m_pmsgr->IdMoreData(0);

  return GME_OK;
}

int CGMClient::Connect(string fn, unsigned long transfer_len)
{
  return Connect(fn.c_str(), transfer_len);
}

int CGMClient::Send(const void *query, unsigned long qlen)
{
  int rc;
  time_t t;

  if( !m_pSocket )
  {
    if( ReConnect() != GME_OK ) return GME_NOT_CONNECTED;
  }
  /*if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Send(0x%X, %lu)", (int)query, qlen);*/
  m_pmsgt->SecuenciaConsulta(m_pmsgt->SecuenciaConsulta()+1);
  t = time(&t);
  m_pmsgt->TimeStamp((unsigned long)t);
  if(m_pmsgt->TipoMensaje() == GM_MSG_TYPE_INT && m_pmsgr->IdMoreData())
  {
    m_pmsgt->IdMoreData(m_pmsgr->IdMoreData());
  }
  else
  {
    m_pmsgt->IdMoreData(0);
  }
  m_pmsgt->TamMaxMensaje(m_transfer_len);
  m_pmsgt->IndiceMensaje(m_indice_mensaje);
  m_pmsgt->SetData(query, qlen);

  LogMessage("SND", m_pmsgt);

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "enviando mensaje por socket");
  if((rc = m_pSocket->Send(m_pmsgt->GetMsg(), m_pmsgt->GetMsgLen())) != 0)
  {
    /* error al enviar */
    if(rc == CTCP_NOT_CONNECTED)
    {
      if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: m_pSocket->Send(...) --> CTCP_NOT_CONNECTED");
      return GME_NOT_CONNECTED;
    }
    else if(rc == CTCP_WRITE_ERROR)
    {
      if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: m_pSocket->Send(...) --> CTCP_WRITE_ERROR");
      return GME_COMM_ERROR;
    }
    else
    {
      if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: m_pSocket->Send(...) --> UNDEFINED [%i]", rc);
      return GME_UNDEFINED;
    }
  }
  return GME_OK;
}

int CGMClient::ReConnect()
{
  int rc;

  if(m_pSocket) delete m_pSocket;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::ReConnect()");
  m_pSocket = new CTcp();
  if((rc = m_pSocket->Connect("",
      m_pidata->m_host,
      m_pidata->m_port)) != 0)
  {
    delete m_pSocket;
    m_pSocket = NULL;
    /* error en la conexion */
    if(rc == CTCP_HOST_NOT_FOUND)
      return GME_HOST_NOTFOUND;
    else if(rc == CTCP_ADDRESS_NOT_FOUND)
      return GME_INVALID_HOST;
    else
      return GME_UNDEFINED;
  }
  m_pmsgt->SetData(NULL, 0);
  return GME_OK;
}

int CGMClient::Send(CGMBuffer& query)
{
  return Send(query.Data(), query.Length());
}

int CGMClient::Send(CGMBuffer* query)
{
  return Send(query->Data(), query->Length());
}

/* Time out expresado en segundos */
int CGMClient::Recv(GMIOS *presp, long to_cs)
{
  int rc;
  char buffer[GM_COMM_MSG_LEN+1];
  CGMBuffer gmBuffer;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "recibiendo mensaje de socket");
  /* Parchazo: Si el mensaje es interactivo y es la peticion de mas datos... */
  if(m_pmsgt->TipoMensaje() == GM_MSG_TYPE_INT && m_indice_mensaje)
  {
    if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "pidiendo mas datos el interactivo");
    Send(NULL, 0);
  }
  /* Asigno por default nulos a estos valores si tengo que devolverlos */
  if(presp)
  {
    presp->data[0] = 0;
    presp->len = 0;
  }
  /* Limpio el buffer donde voy a recibir los datos */
  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "leyendo el socket");
  gmBuffer.Clear();
  if((rc = m_pSocket->Receive(buffer, GM_COMM_MSG_LEN, (int)(to_cs * 10))) > 0)
  {
    gmBuffer.Add(buffer, rc);
    while((rc = m_pSocket->Receive(buffer, GM_COMM_MSG_LEN, 0)) > 0)
    {
      if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "agregando al buffer %i bytes", rc);
      gmBuffer.Add(&buffer[0], rc);
      if(rc < GM_COMM_MSG_LEN)
      {
        if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "se recibio el ultimo cachito");
        break;
      }
    }
  }
  else
  {
    if(rc == CTCP_NOT_CONNECTED)
    {
      if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: m_pSocket->Receive(...) --> CTCP_NOT_CONNECTED");
      return GME_NOT_CONNECTED;
    }
    else if(rc == CTCP_WRITE_ERROR)
    {
      if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: m_pSocket->Receive(...) --> CTCP_WRITE_ERROR");
      return GME_COMM_ERROR;
    }
    else
    {
      if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: m_pSocket->Receive(...) --> UNDEFINED [%i]", rc);
      return GME_UNDEFINED;
    }
  }
  Discon();
  if(m_pmsgr->SetMsg(gmBuffer.Data(), gmBuffer.Length()) != 0)
  {
    if(m_pidata->IsVerbose()) syslog(LOG_ERR, "ERROR: recibido header invalido");
    return GME_INVALID_HEADER;
  }
  LogMessage("RCV", m_pmsgr);
  /* controlo la integridad del mensaje recibido */

  /* ... en algun momento se va a hacer */

  if( (m_pmsgr->CodigoRetorno() == GME_OK || m_pmsgr->CodigoRetorno() == GME_MORE_DATA)  &&
    (m_pmsgr->TipoMensaje() == GM_MSG_TYPE_R_CR || m_pmsgr->TipoMensaje() == GM_MSG_TYPE_R_INT) &&
    presp )
  {
    if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "copiando info del ensaje recibido");
    /* Tomo la data solamente si no hubo error y para los mensajes CONSULTA / RESPUESTA e INTERACTIVOS */
    presp->len = m_pmsgr->GetDataLen();
    if(presp->len > 0)
    {
      if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "copiando %lu bytes de datos del mensaje recibido", presp->len);
      /* lo paso al mensaje de salida */
      //presp->data = (char*)malloc(presp->len + sizeof(char));
      memcpy(presp->data, m_pmsgr->GetData(), presp->len);
      /* En mensajes interactivos recalculo los datos para poder pedir mas */
      if(m_pmsgr->TipoMensaje() == GM_MSG_TYPE_R_INT)
      {
        if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "es respuesta de mensaje interactivo");
        if(presp->len == m_transfer_len && m_pmsgr->CodigoRetorno() == GME_MORE_DATA)
        {
          if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "el mensaje interactivo tiene continuacion");
          m_indice_mensaje += presp->len;
        }
        else
        {
          if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "el mensaje interactivo no tiene continuacion o es el ultimo");
          m_indice_mensaje = 0l;
        }
      }
    }
    else
    {
      presp->data[0] = 0;
    }
    return m_pmsgr->CodigoRetorno();
  }
  else if(m_pmsgr->CodigoRetorno() != 0)
  {
    if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "el mensaje se�ala error en el codigo de retorno");
    return m_pmsgr->CodigoRetorno();
  }
  else
  {
    if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "respuesta OK");
    return GME_OK;
  }
}

int CGMClient::Recv(CGMBuffer& response, long to_cs)
{
  return Recv(&response, to_cs);
}

int CGMClient::Recv(CGMBuffer* response, long to_cs)
{
  GMIOS gmio;
  int rc;

  if(( rc = Recv(&gmio, to_cs) ) == GME_OK)
  {
    response->Add(gmio.data, gmio.len);
    //Free(gmio);
  }
  return rc;
}

int CGMClient::Discon()
{
  if( !m_pSocket ) return GME_OK;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Discon()");
  m_pSocket->Close();
  delete m_pSocket;
  m_pSocket = NULL;
  return GME_OK;
}

/* modo encolado */
int CGMClient::Enqueue(const char* queue, const void *data, unsigned long len)
{
  ST_SQUEUE st;
  int rc;

  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Enqueue(...)");
  strcpy(st.head.saf_name, queue);
  st.head.len = len;
  memcpy(&st.data[0], data, len);
  rc = Notify(".enqueue", &st, sizeof(ST_SQUEUE::head) + len);
  return rc;
}

int CGMClient::Enqueue(string queue, const void *data, unsigned long len)
{
  return Enqueue(queue.c_str(), data, len);
}

int CGMClient::Dequeue(const char* queue, GMIOS *pdata)
{
  if(m_pidata->IsVerbose()) syslog(LOG_DEBUG, "CGMClient::Dequeue(...)");
  return Call(".dequeue", queue, strlen(queue)+1, pdata, 3000);
}

int CGMClient::Dequeue(string queue, GMIOS *pdata)
{
  return Dequeue(queue.c_str(), pdata);
}

void CGMClient::LogMessage(const char* label, CGMessage* msg)
{
  char strTmp[60]; /* para mostrar hasta 60 caracteres del mensaje */
  int i;

  if( !m_pidata->IsVerbose()) return;

  syslog(LOG_DEBUG, "== %-3.3s ==============================================================", label);
  syslog(LOG_DEBUG, "(%c)%28.28s %05lu Id: Tr/%05u Md/%05u Or/%05u Ds/%05u Orig: Co/%c Re/%c Rc: %03u",
              msg->TipoMensaje(),
              msg->Funcion(),
              msg->GetDataLen(),
              msg->IdTrans(), msg->IdMoreData(), msg->IdOrigen(), msg->IdDestino(),
              msg->OrigenConsulta(), msg->OrigenRespuesta(),
              msg->CodigoRetorno());
  syslog(LOG_DEBUG, "  Tam. Total:     [%lu]", msg->GetMsgLen());
  syslog(LOG_DEBUG, "  Tam. Header:    [%u]",  msg->GetHeaderLen());
  syslog(LOG_DEBUG, "  Tam. Datos:     [%lu]", msg->GetDataLen());
  syslog(LOG_DEBUG, "  Tipo Mensaje:   [%c]",  msg->TipoMensaje());
  syslog(LOG_DEBUG, "  Version Header: [%u]",  msg->VersionHeader());
  syslog(LOG_DEBUG, "  Id Usuario:     [%s]",  msg->IdUsuario());
  syslog(LOG_DEBUG, "  Id Cliente:     [%s]",  msg->IdCliente());
  syslog(LOG_DEBUG, "  Key:            [%s]",  msg->Key());
  syslog(LOG_DEBUG, "  Id Grupo:       [%s]",  msg->IdGrupo());
  syslog(LOG_DEBUG, "  Funcion:        [%s]",  msg->Funcion());
  syslog(LOG_DEBUG, "  IdTrans:        [%u]",  msg->IdTrans());
  syslog(LOG_DEBUG, "  IdCola:         [%i]",  msg->IdCola());
  syslog(LOG_DEBUG, "  IdMoreData:     [%u]",  msg->IdMoreData());
  syslog(LOG_DEBUG, "  Sec. Cons:      [%u]",  msg->SecuenciaConsulta());
  syslog(LOG_DEBUG, "  Sec. Resp:      [%u]",  msg->SecuenciaRespuesta());
  syslog(LOG_DEBUG, "  Orig. Cons.     [%c]",  msg->OrigenConsulta());
  syslog(LOG_DEBUG, "  Orig. Resp.     [%c]",  msg->OrigenRespuesta());
  syslog(LOG_DEBUG, "  Id Origen       [%u]",  msg->IdOrigen());
  syslog(LOG_DEBUG, "  Id Router       [%u]",  msg->IdRouter());
  syslog(LOG_DEBUG, "  Id Destino      [%u]",  msg->IdDestino());
  syslog(LOG_DEBUG, "  TimeStamp:      [%lu]", msg->TimeStamp());
  syslog(LOG_DEBUG, "  IndiceMensaje:  [%lu]", msg->IndiceMensaje());
  syslog(LOG_DEBUG, "  TamMaxMensaje:  [%lu]", msg->TamMaxMensaje());
  syslog(LOG_DEBUG, "  TamTotMensaje:  [%lu]", msg->TamTotMensaje());
  syslog(LOG_DEBUG, "  CodigoRetorno:  [%u]",  msg->CodigoRetorno());

  if(msg->GetDataLen() > 0)
  {
    syslog(LOG_DEBUG, "  CRC Datos:      [%s]", msg->Crc());
    syslog(LOG_DEBUG, "===============================================================================");
    memcpy(strTmp, msg->GetData(), 60);
    for(i = 0; i < 60 && i < (int)msg->GetDataLen(); i++)
    {
      if(strTmp[i] < ' ') strTmp[i]= '.';
      else if(strTmp[i] > 'z') strTmp[i]= '?';
    }
    i--;
    syslog(LOG_DEBUG, "  Datos:          [%-*.*s]", i, i, strTmp);
  }
  syslog(LOG_DEBUG, "===============================================================================");
}
