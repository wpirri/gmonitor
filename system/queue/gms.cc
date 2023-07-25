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

#include "gms.h"

CGMServer::CGMServer() { }
CGMServer::~CGMServer() { }

/* Colocar en esta funcion lo que se necesite correr al levantar el server */
int CGMServer::Init() { return 0; }

/* Colocar en esta funcion lo que se necesite correr al bajar el serer */
int CGMServer::Exit() { return 0; }

/* Estas rutinas son llamadas para el manejo de transaccion se debe colocar en ellas el c�digo necesario para cada uno de los procesos */
int CGMServer::BeginTrans(unsigned int trans) { return 0; }
int CGMServer::CommitTrans(unsigned int trans) { return 0; }
int CGMServer::RollbackTrans(unsigned int trans) { return 0; }

/* estas rutinas se llaman antes y despu�s de la de procesamiento de mensaje */
int CGMServer::PreMain() { return 0; }
int CGMServer::PosMain() { return 0; }

/* Colocar en esta funcion el proceso que intepreta el mensaje recibido */
int CGMServer::Main(const char *funcion, char typ, void* in, unsigned long inlen, void** out, unsigned long *outlen)
{
  return CGMServerBase::Main(in, inlen, out, outlen);
}

