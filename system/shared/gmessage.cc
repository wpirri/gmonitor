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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/*
    Implementacion del protocolo de mensajeria, esta clase junto con
    GMHeader forman el mensaje que comunica al cliente con el server
*/
#include <gmstring.h>
#include <gmessage.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
using namespace std;

#include <string.h>

CGMessage::CGMessage()
{
	m_msg_buffer = new char[GetHeaderLen()];
	m_msg_len = GetHeaderLen();
}

CGMessage::~CGMessage()
{
	delete m_msg_buffer;
}

const char* CGMessage::GetMsg()
{
/*
	Ya no se setea ac� pra que sea un dato puesto por el cliente
	time_t t;
	t = time(&t);
	TimeStamp(t);
*/
	memcpy(m_msg_buffer, GetHeader(), GetHeaderLen());
	return (const char*)&m_msg_buffer[0];
}

unsigned long CGMessage::GetMsgLen()
{
	return m_msg_len;
}

int CGMessage::SetMsg(const void* msg, unsigned long len)
{
	if( !msg || !len) return (-1);

	if(len < GetHeaderLen())
	{
		return -1;
	}
	delete m_msg_buffer;
	m_msg_buffer = new char[len];
	m_msg_len = len;
	memcpy(m_msg_buffer, msg, len);
	SetHeader(m_msg_buffer);
	return 0;
}

int CGMessage::SetMsg(CGMBuffer *buffer)
{
	if(! buffer) return -1;
	return SetMsg(buffer->Data(), buffer->Length());
}

int CGMessage::SetData(const void* msg, unsigned long len)
{
	if( !msg || !len) return (-1);

	delete m_msg_buffer;
	m_msg_buffer = new char[GetHeaderLen() + len];
	if(msg && len > 0)
	{
		memcpy(&m_msg_buffer[GetHeaderLen()], msg, len);
	}
	m_msg_len = GetHeaderLen() + len;
	Crc("*** NULL CRC ***");
	TamMensaje(len);
	return 0;
}

int CGMessage::SetData(CGMBuffer *buffer)
{
	if(! buffer) return -1;
	return SetData(buffer->Data(), buffer->Length());
}

const char *CGMessage::GetData()
{
	return (const char*)&m_msg_buffer[GetHeaderLen()];
}

unsigned long CGMessage::GetDataLen()
{
	return m_msg_len - GetHeaderLen();
}

int CGMessage::SetResponse(CGMessage* msg)
{
	if( !msg) return (-1);

	TipoMensaje(TipoRespuesta(msg->TipoMensaje()));
	IdUsuario(msg->IdUsuario());
	IdCliente(msg->IdCliente());
	IdGrupo(msg->IdGrupo());
	Funcion(msg->Funcion());
	IdTrans(msg->IdTrans());
	IdCola(msg->IdCola());
	IdMoreData(msg->IdMoreData());
	SecuenciaConsulta(msg->SecuenciaConsulta());
	SecuenciaRespuesta(msg->SecuenciaRespuesta()+1);
	OrigenConsulta(msg->OrigenConsulta());
	OrigenRespuesta(msg->OrigenRespuesta());
	IdOrigen(msg->IdOrigen());
	IdRouter(msg->IdRouter());
	IdDestino(msg->IdDestino());
	TimeStamp(msg->TimeStamp());
	IndiceMensaje(msg->IndiceMensaje());
	TamMaxMensaje(msg->TamMaxMensaje());
	CodigoRetorno(0);

	return 0;
}

