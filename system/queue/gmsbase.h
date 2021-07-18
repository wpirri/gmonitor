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
#ifndef _GMSBASE_H_
#define _GMSBASE_H_

#include "gmidat.h"
#include "gmbuffer.h"
#include "msgtype.h"
#include "cmsg.h"
#include "gmontdb.h"

#include <iostream>
#include <string>
using namespace std;

class CGMServerBase
{
public:
  CGMServerBase();
  ~CGMServerBase();

  typedef struct _GMIOS
  {
    unsigned long len;
    void *data;
  } GMIOS;

  int LoadConfig();
  /* modo aviso */
  int Notify(const char *event, const void *data, unsigned long len);
  int Notify(string& event, CGMBuffer& data);
  int Notify(string& event, CGMBuffer* data);
  /* modo evento */
  int Post(const char *event, const void *data, unsigned long len);
  int Post(string& event, CGMBuffer& data);
  int Post(string& event, CGMBuffer* data);
  int Broadcast(const char *user, const char *client,
          const char *group, const void *data, unsigned long len);
  /* modo encolado */
  int Enqueue(const char* queue, const void *data, unsigned long len);
  int Enqueue(string& queue, const void *data, unsigned long len);
  int Dequeue(const char* queue, GMIOS *pdata);
  int Dequeue(string& queue, GMIOS *pdata);
  /* modo consulta/respuesta */
  int Call(const char *fn,  const void *query, unsigned long qlen, GMIOS *presp, int to);
  int Call(string& fn, CGMBuffer& query, CGMBuffer& response, int to);
  int Call(string& fn, CGMBuffer* query, CGMBuffer* response, int to);
  /* Inicia y finaliza el modo transaccional */
  int SetTransMode(bool on_off);
  /* Suscripci�n a servicios */
  int Suscribe(const char *event, char tipo_mensaje);
  int UnSuscribe(const char *event, char tipo_mensaje);
  /* Para servers ON-LINE y OFF-LINE */
  int Run(const char* exe, char* const params[]);
  int Main(void* in, unsigned long inlen, void** out, unsigned long *outlen);
  /* liberacion del buffer recibido */
  int Free(GMIOS s);
  int Free(GMIOS* ps);

  /* puntero provisto por la clase para uso generico */
  void* m_gptr;

  /* Algunos datos del entorno */
  string m_monitor_path;
  /*string m_config_path;*/
  string m_server_name;

  /* Datos para el armado de los mensajes */
  string m_IdUsuario;
  string m_IdCliente;
  string m_Key;
  string m_IdGrupo;
  /*
  unsigned int m_IdCola;
  unsigned int m_IdTrans;
  unsigned int m_IdOrigen;
  unsigned int m_IdRouter;
  unsigned int m_IdDestino;
  unsigned long m_TimeStamp;
  */

  typedef struct _CLIENT_DATA
  {
    string m_host;
    unsigned int m_port;
    string m_user;
    string m_client;
    string m_key;
    string m_group;
    unsigned int m_trans;
    unsigned long m_timestamp;
    long m_flags;
  } CLIENT_DATA;
  /* Objeto para contener los datos del cliente */
  CLIENT_DATA m_ClientData;
  CGMTdb* m_pConfig;
  char** m_arg_ptr;
  int    m_arg_cnt;

protected:
  bool m_transactional_mode;
  int m_stdin_pipe;
  int m_stdout_pipe;
  int m_stderr_pipe;
  /*CMsg *m_pMsg;*/

private:
  int WaitRead(int fd, void *buff, int count, long to_cs);

};
#endif /* _GMS_H_ */
