/***************************************************************************
 * Copyright (C) 2003 Walter Pirri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 ***************************************************************************/
#ifndef _GMESSAGE_H_
#define _GMESSAGE_H_

#include <gmheader.h>
#include <gmbuffer.h>

class CGMessage : public CGMHeader
{
public:
	CGMessage();
	virtual ~CGMessage();

	/* devuelve buffer del mensaje completo */
	/* para enviar por el stream */
	const char *GetMsg();
	unsigned long GetMsgLen();
	/* se usa para recibir el mensaje y copiarlo a la estructura */
	int SetMsg(const void* msg, unsigned long len);
	int SetMsg(CGMBuffer *buffer);

	/* se usa para poner la parte de datos en el mensaje */
	int SetData(const void* data, unsigned long len);
	int SetData(CGMBuffer *buffer);
	/* devuelve el contenido del mensaje */
	/* sin el header */
	const char* GetData();
	unsigned long GetDataLen();
	/* arma un mensaje de respuesta segun la consulta pasada */
	int SetResponse(CGMessage* msg);

protected:
	char* m_msg_buffer;
	unsigned long m_msg_len;

};

#endif /* _GMESSAGE_H_ */

