/***************************************************************************
    Copyright (C) 2003  Walter Pirri

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

#ifndef _GMC_H_
#define _GMC_H_

#include <string>
#include <vector>

#include "ctcp.h"
#include "gmidat.h"
#include "gmerror.h"
#include "gmessage.h"

typedef short (*SERVICE_FUNCTION)(const char*, char, const void*, unsigned long);

class CGMClient
{
public:
  /*typedef int (*_unsol_handler_t)(char *data, unsigned long len);*/
  /* inicializacion del driver */
  CGMClient(CGMInitData *init_data = NULL);
  /* liberacion de recursos del driver */
  virtual ~CGMClient();
  /* Una estructura para el intercambio de datos */
  typedef struct _GMIOS
  {
    unsigned long len;
    void *data;
  } GMIOS;
  /* liberacion del buffer recibido */
  int Free(GMIOS s);
  int Free(GMIOS* ps);

  /* Inicialización alternativa */
  void Init(CGMInitData *init_data = NULL);

  /* manejo de transaccion */
  int Begin(long to_s);
  int Commit();
  int Abort();

  /* Notificacion - GM_MSG_TYPE_NOT */
  int Notify(const char *event, const void *data, unsigned long len);
  int Notify(string& event, CGMBuffer& data);
  int Notify(string& event, CGMBuffer* data);
  /* Eventos - GM_MSG_TYPE_MSG*/
  int Post(const char *event, const void *data, unsigned long len);
  int Post(string& event, CGMBuffer& data);
  int Post(string& event, CGMBuffer* data);
  int Broadcast(const char *user, const char *client,
          const char *group, const char *event, const void *data, unsigned long len);
  /* suscripcion a eventos */
  int Suscribe(const char *event, char typ, SERVICE_FUNCTION fcn);
  int Suscribe(string& event, char typ, SERVICE_FUNCTION fcn);
  int UnSuscribe(const char *event, char typ);
  int UnSuscribe(string& event, char typ);
  int CheckService(long to_cs = -1);

  /* modo consulta/respuesta */
  int Call(const char *fn, const void *query, unsigned long qlen, GMIOS *presp, long to);
  int Call(string fn, CGMBuffer& query, CGMBuffer& response, long to_cs);
  int Call(string fn, CGMBuffer* query, CGMBuffer* response, long to_cs);
  int ACall(const char *fn, const void *query, unsigned long qlen);
  int GetReply(GMIOS *presp, long to_cs);
  int Cancel();

  /* modo interactivo */
  int Connect(const char *fn, unsigned long transfer_len = 0);
  int Connect(string fn, unsigned long transfer_len = 0);
  int Send(const void *query, unsigned long qlen);
  int Send(CGMBuffer& query);
  int Send(CGMBuffer* query);
  int Recv(GMIOS *resp, long to_cs = 1000);
  int Recv(CGMBuffer& response, long to_cs = 1000);
  int Recv(CGMBuffer* response, long to_cs = 1000);
  int Discon();
  int ReConnect();

  /* modo encolado */
  int Enqueue(const char* queue, const void *data, unsigned long len);
  int Enqueue(string queue, const void *data, unsigned long len);
  int Dequeue(const char* queue, GMIOS *pdata);
  int Dequeue(string queue, GMIOS *pdata);
protected:
  typedef struct _SVC_REL
  {
    char svc[33];
    char typ;
    char saf[33];
    SERVICE_FUNCTION fcn;;
  } SVC_REL;
  CTcp* m_pSocket;
  CGMessage *m_pmsgt;
  CGMessage *m_pmsgr;
  CGMInitData *m_pidata;
  vector<SVC_REL> m_svc_rel;
  /* miembros que se utilizan para mensajes interactivos */
  unsigned long m_transfer_len;
  unsigned long m_indice_mensaje;
  /* No sacar auque no se use para que se incluya este objeto
     en la libreria */
  CGMError m_error;

  void LogMessage(const char* label, CGMessage* msg);
};
#endif /* _GMC_H_ */

