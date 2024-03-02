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

#include <gms.h>

#include <gmerror.h>
#include <gmconst.h>
#include <gmbuffer.h>
#include <cmsg.h>
#include <gmisc.h>
#include <gmstring.h>
#include <glog.h>

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <syslog.h>
#include <fcntl.h>


/*
        La variable 'void* m_gptr' es un puntero para uso generico por el server
        es el unico puntero miembro de la clase que puede ser utilizado libremente
        al realizar el programa server.
        La variable 'CGMInitData m_ClientData' se completar� con los valores del
        cliente que solicit� el servicio antes del llamado a la funci�n PreMain()
        y mantendr� este valos para ser utilizado por el server si es necesario
        hasta el final del servicio.
*/

extern CGLog* pLog;

CGMServer::CGMServer() { }

CGMServer::~CGMServer() { }

/* Colocar en esta funcion lo que se necesite correr al levantar el server */
int CGMServer::Init()
{
  pLog->Add(1, "Iniciando server");
  openlog("gm_log", LOG_CONS, LOG_DAEMON);
  /* Servicio de prueba de enlace */
  if(Suscribe(".eco", GM_MSG_TYPE_CR) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .eco");
  /* Servicio para loggear mensajes en el log del sistema */
  if(Suscribe(".log", GM_MSG_TYPE_MSG) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .log");
  /* Inicio en modo transaccional */
  SetTransMode(true);
  return 0;
}

/* Colocar en esta funcion lo que se necesite correr al bajar el serer */
int CGMServer::Exit()
{
  pLog->Add(1, "Terminando server");
  UnSuscribe(".eco", GM_MSG_TYPE_CR);
  UnSuscribe(".log", GM_MSG_TYPE_MSG);
  return 0;
}

/* Estas rutinas son llamadas para el manejo de transaccion se debe colocar en ellas el c�digo necesario para cada uno de los procesos */
int CGMServer::BeginTrans(unsigned int trans)
{
  pLog->Add(100, "INICIO DE TRANSACCION %u", trans);
  return 0;
}
int CGMServer::CommitTrans(unsigned int trans)
{
  pLog->Add(100, "COMMIT DE TRANSACCION %u", trans);
  return 0;
}
int CGMServer::RollbackTrans(unsigned int trans)
{
  pLog->Add(100, "ROLLBACK DE TRANSACCION %u", trans);
  return 0;
}

/* estas rutinas se llaman antes y despu�s de la de procesamiento de mensaje */
int CGMServer::PreMain()
{
  return 0;
}
int CGMServer::PosMain()
{
  return 0;
}

/* Colocar en esta funcion el proceso que intepreta el mensaje recibido */
int CGMServer::Main(const char *funcion, char /*typ*/, void* in, unsigned long inlen, void* out, unsigned long *outlen, unsigned long /*max_outlen*/)
{
  pLog->Add(100, "CGMServer::Main(%s, 0x%08X, %lu)", funcion, in, inlen);
  *outlen = 0;
  if(!strcmp(funcion, ".eco"))
  {
    *outlen = inlen;
    memcpy(out, in, *outlen);
    sleep(1); /* para probar los timeouts */
    return GME_OK;
  }
  else if(!strcmp(funcion, ".log"))
  {
    syslog( LOG_INFO, "[%s][%s][%s][%s] %s",
        m_ClientData.m_host.c_str(),
        m_ClientData.m_user.c_str(),
        m_ClientData.m_client.c_str(),
        m_ClientData.m_group.c_str(),
        (char*)in);
    return GME_OK;
  }
  return GME_SVC_UNRESOLVED;
}

