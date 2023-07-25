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
#ifndef _GMS_H_
#define _GMS_H_

#include "gmsbase.h"

class CGMServer : public CGMServerBase
{
public:
	CGMServer();
	~CGMServer();

	/* Colocar en esta funcion lo que se necesite correr al
	levantar el server */
	int Init();
	/* Colocar en esta funcion lo que se necesite correr al
	bajar el serer */
	int Exit();
	/* Colocar en esta funcion el proceso que intepreta el
	mensaje recibido */
	int Main(const char *funcion, char typ, void* in, unsigned long inlen, void** out, unsigned long *outlen);
	/* estas rutinas se llaman antes y despu�s de la de
	procesamiento de mensaje */
	int PreMain();
	int PosMain();
	/* Estas rutinas son llamadas para el manejo de transaccion
	se debe colocar en ellas el c�digo necesario para cada uno
	de los procesos */
	int BeginTrans(unsigned int trans);
	int CommitTrans(unsigned int trans);
	int RollbackTrans(unsigned int trans);

};
#endif /* _GMS_H_ */
