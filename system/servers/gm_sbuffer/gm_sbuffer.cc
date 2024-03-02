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
#include <gmbuffer.h>

#include <cstdio>
#include <cstdlib>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include "gmsbuffer.h"

/*
        La variable 'void* m_gptr' es un puntero para uso generico por el server
        es el unico puntero miembro de la clase que puede ser utilizado libremente
        al realizar el programa server.
        La variable 'CGMInitData m_ClientData' se completar� con los valores del
        cliente que solicit� el servicio antes del llamado a la funci�n PreMain()
        y mantendr� este valos para ser utilizado por el server si es necesario
        hasta el final del servicio.
*/

/* los procesos en su fuente principal declaran esta variable para que
todos los mdulos puedan loguear */
extern CGLog* pLog;

CGMServer::CGMServer()
{
	m_gptr = NULL;
}

CGMServer::~CGMServer()
{

}

/* Colocar en esta funcion lo que se necesite correr al levantar el server */
int CGMServer::Init()
{

	m_gptr = new CGMSBuffer(this, pLog);

  pLog->Add(1, "Iniciando server");
	if(Suscribe(".new_buffer", GM_MSG_TYPE_CR) != GME_OK)  /*Call()*/
    pLog->Add(1, "ERROR al suscribir a .new_buffer");
	if(Suscribe(".add_buffer", GM_MSG_TYPE_NOT) != GME_OK) /*Notify()*/
    pLog->Add(1, "ERROR al suscribir a .add_buffer");
	if(Suscribe(".del_buffer", GM_MSG_TYPE_NOT) != GME_OK) /*Notify()*/
    pLog->Add(1, "ERROR al suscribir a .del_buffer");
	if(Suscribe(".get_buffer", GM_MSG_TYPE_CR) != GME_OK)  /*Call()*/
    pLog->Add(1, "ERROR al suscribir a .get_buffer");
	/* Inicio en modo transaccional */
	SetTransMode(true);
	return 0;
}

/* Colocar en esta funcion lo que se necesite correr al bajar el serer */
int CGMServer::Exit()
{
  pLog->Add(1, "Terminando server");
	UnSuscribe(".new_buffer", GM_MSG_TYPE_CR);
	UnSuscribe(".add_buffer", GM_MSG_TYPE_NOT);
	UnSuscribe(".del_buffer", GM_MSG_TYPE_NOT);
  UnSuscribe(".get_buffer", GM_MSG_TYPE_CR);
	/* Fin modo transaccional */
	SetTransMode(false);
	if(m_gptr) delete ((CGMSBuffer*)m_gptr);
	return 0;
}

/* Estas rutinas son llamadas para el manejo de transaccion se debe colocar en ellas el c�digo necesario para cada uno de los procesos */
int CGMServer::BeginTrans(unsigned int trans)
{
	((CGMSBuffer*)m_gptr)->DeleteTrans(trans);
	return 0;
}

int CGMServer::CommitTrans(unsigned int trans)
{
	((CGMSBuffer*)m_gptr)->DeleteTrans(trans);
	return 0;
}

int CGMServer::RollbackTrans(unsigned int trans)
{
	((CGMSBuffer*)m_gptr)->DeleteTrans(trans);
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

int CGMServer::Main(const char *funcion, char /*typ*/, void* in, unsigned long inlen, void* out, unsigned long *outlen, unsigned long /*max_outlen*/)
{
	return ((CGMSBuffer*)m_gptr)->Process(funcion, in, inlen, out, outlen, &m_ClientData);
}

