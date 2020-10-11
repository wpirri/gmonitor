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
#include "gmonitor/gms.h"

/*
	La variable 'void* m_gptr' es un puntero para uso generico por el server
	es el unico puntero miembro de la clase que puede ser utilizado libremente
	al realizar el programa server.
	La variable 'CGMInitData m_ClientData' se completar� con los valores del
	cliente que solicit� el servicio antes del llamado a la funci�n PreMain()
	y mantendr� este valos para ser utilizado por el server si es necesario
	hasta el final del servicio.
*/

CGMServer::CGMServer() { }

CGMServer::~CGMServer() { }

/* Colocar en esta funcion lo que se necesite correr al levantar el server */
int CGMServer::Init() { return 0; }

/* Colocar en esta funcion lo que se necesite correr al bajar el serer */
int CGMServer::Exit() { return 0; }

/* Estas rutinas son llamadas para el manejo de transaccion se debe colocar en ellas el c�digo necesario para cada uno de los procesos */
int CGMServer::BeginTrans() { return 0; }
int CGMServer::CommitTrans() { return 0; }
int CGMServer::RollbackTrans() { return 0; }

/* estas rutinas se llaman antes y despu�s de la de procesamiento de mensaje */
int CGMServer::PreMain() { return 0; }
int CGMServer::PosMain() { return 0; }

/* Colocar en esta funcion el proceso que intepreta el mensaje recibido */
int CGMServer::Main(const char *funcion, void* in, unsigned long inlen, void** out, unsigned long *outlen)
{



	return 0;
}

