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
    Iplementacion del header para la mensajeria entre cliente y servidor
*/
#include "gmstring.h"
#include "gmheader.h"
#include "msgtype.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
using namespace std;

#include <string.h>

CGMHeader::CGMHeader()
{
	memset(m_header.TipoMensaje, '?', 1);
	memcpy(m_header.VersionHeader, VERSION_HEADER, 3);
	memset(m_header.IdUsuario, ' ', 16);
	memset(m_header.IdCliente, ' ', 16);
	memset(m_header.Key, ' ', 16);
	memset(m_header.IdGrupo, ' ', 16);
	memset(m_header.Funcion, ' ', 32);
	memset(m_header.IdTrans, '0', 5);
	memset(m_header.IdCola, '0', 5);
	memset(m_header.IdMoreData, '0', 5);
	memset(m_header.SecuenciaConsulta, '0', 5);
	memset(m_header.SecuenciaRespuesta, '0', 5);
	memset(m_header.OrigenConsulta, '0', 1);
	memset(m_header.OrigenRespuesta, '0', 1);
	memset(m_header.IdOrigen, '0', 5);
	memset(m_header.IdRouter, '0', 5);
	memset(m_header.IdDestino, '0', 5);
	memset(m_header.TimeStamp, '0', 10);
	memset(m_header.CodigoRetorno, '0', 5);
	memset(m_header.Crc, ' ', 16);
	memset(m_header.TamMensaje, '0', 10);
	memset(m_header.IndiceMensaje, '0', 10);
	memset(m_header.TamMaxMensaje, '0', 10);
	memset(m_header.TamTotMensaje, '0', 10);
}

CGMHeader::~CGMHeader()
{

}

const char *CGMHeader::GetHeader()
{
	return (const char*)&m_header;
}

unsigned int CGMHeader::GetHeaderLen()
{
	return sizeof(GMONITOR_HEADER);
}

int CGMHeader::SetHeader(const char* msg)
{
	if(!msg) return -1;
	memcpy(&m_header, msg, GetHeaderLen());
	if( memcmp(m_header.VersionHeader, VERSION_HEADER, 3)) return -1;
	if(	m_header.TipoMensaje[0] != GM_MSG_TYPE_CR     &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_MSG    &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_INT    &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_QUE    &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_NOT    &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_R_CR   &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_R_MSG  &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_R_INT  &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_R_QUE  &&
		m_header.TipoMensaje[0] != GM_MSG_TYPE_R_NOT   ) return -1;
	return 0;
}

int CGMHeader::SetHeader(CGMBuffer& buffer)
{
	if(buffer.Length() < GetHeaderLen()) return -1;
	return SetHeader(&buffer);
}

int CGMHeader::SetHeader(CGMBuffer* buffer)
{
	if(!buffer) return -1;
	if(buffer->Length() < GetHeaderLen()) return -1;
	memcpy(&m_header, buffer->Data(), GetHeaderLen());
	return 0;
}

char CGMHeader::TipoMensaje()
{
	return m_header.TipoMensaje[0];
}

void CGMHeader::TipoMensaje(char t)
{
	m_header.TipoMensaje[0] = t;
}

unsigned int CGMHeader::VersionHeader()
{
	return (unsigned int)subint(m_header.VersionHeader,0,3);
}

void CGMHeader::VersionHeader(unsigned int v)
{
	memprint(m_header.VersionHeader,"%03u",v);
}

const char* CGMHeader::IdUsuario()
{
	char *str;
	memset(m_IdUsuario,0,sizeof(m_IdUsuario));
	memcpy(m_IdUsuario, m_header.IdUsuario, 16);
	if((str = strstr(m_IdUsuario, " ")) != NULL)
	{
		str[0] = '\0';
	}
	return (const char*)&m_IdUsuario[0];
}

int CGMHeader::IdUsuario(const char* id)
{
        if(!id) return -1;
        if(strlen(id) < sizeof(m_IdUsuario))
        {
		memprint(m_header.IdUsuario, "%-16.16s", id);
                return 0;
        }
        else
        {
                return -1;
        }
}

int CGMHeader::IdUsuario(string sid)
{
	return IdUsuario(sid.c_str());
}

const char* CGMHeader::IdCliente()
{
	char *str;
	memset(m_IdCliente,0,sizeof(m_IdCliente));
	memcpy(m_IdCliente, m_header.IdCliente, 16);
	if((str = strstr(m_IdCliente, " ")) != NULL)
	{
		str[0] = '\0';
	}
	return (const char*)&m_IdCliente[0];
}

int CGMHeader::IdCliente(const char* id)
{
	if(!id) return -1;
	if(strlen(id) < sizeof(m_IdCliente))
	{
		memprint(m_header.IdCliente, "%-16.16s", id);
		return 0;
	}
	else
	{
		return -1;
	}
}

int CGMHeader::IdCliente(string sid)
{
	return IdCliente(sid.c_str());
}

const char* CGMHeader::Key()
{
	memset(m_Key, 0, sizeof(m_Key));
	memcpy(m_Key, m_header.Key, 16);
	m_Key[16] = '\0';
	return (const char*)&m_Key[0];
}

int CGMHeader::Key(const char* key)
{
	if(!key) return -1;
	if(strlen(key) < sizeof(m_Key))
	{
		memprint(m_header.Key, "%-16.16s", key);
		return 0;
	}
	else
	{
		return -1;
	}
}

int CGMHeader::Key(string skey)
{
	return Key(skey.c_str());
}

const char* CGMHeader::IdGrupo()
{
	char *str;
	memset(m_IdGrupo, 0, sizeof(m_IdGrupo));
	memcpy(m_IdGrupo,m_header.IdGrupo, 16);
	if((str = strstr(m_IdGrupo, " ")) != NULL)
	{
		str[0] = '\0';
	}
	return (const char*)&m_IdGrupo[0];
}

int CGMHeader::IdGrupo(const char* id)
{
        if(!id) return -1;
        if(strlen(id) < sizeof(m_IdGrupo))
        {
		memprint(m_header.IdGrupo,"%-16.16s",id);
                return 0;
        }
        else
        {
                return -1;
        }
}

int CGMHeader::IdGrupo(string sid)
{
	return IdGrupo(sid.c_str());
}

const char* CGMHeader::Funcion()
{
	char *str;
	memset(m_Funcion, 0, sizeof(m_Funcion));
	memcpy(m_Funcion, m_header.Funcion, 32);
	if((str = strstr(m_Funcion, " ")) != NULL)
	{
		str[0] = '\0';
	}
	return (const char*)&m_Funcion[0];
}

int CGMHeader::Funcion(const char* f)
{
	if(!f) return -1;
	if(strlen(f) < sizeof(m_Funcion))
	{
		memprint(m_header.Funcion, "%-32.32s", f);
		return 0;
	}
	else
	{
		return -1;
	}
}

int CGMHeader::Funcion(string sf)
{
	return Funcion(sf.c_str());
}

unsigned int CGMHeader::IdTrans()
{
	return (unsigned int)subint(m_header.IdTrans, 0, 5);
}

unsigned int CGMHeader::IdTrans(unsigned int id)
{
	memprint(m_header.IdTrans,"%05u",id);
	return id;
}

int CGMHeader::IdCola()
{
	return subint(m_header.IdCola, 0, 5);
}

int CGMHeader::IdCola(int id)
{
	memprint(m_header.IdCola,"%05u",id);
	return id;
}

unsigned int CGMHeader::IdMoreData()
{
	return (unsigned int)subint(m_header.IdMoreData, 0, 5);
}

unsigned int CGMHeader::IdMoreData(unsigned int id)
{
	memprint(m_header.IdMoreData,"%05u",id);
	return id;
}

unsigned int CGMHeader::SecuenciaConsulta()
{
	return (unsigned int)subint(m_header.SecuenciaConsulta, 0, 5);
}

unsigned int CGMHeader::SecuenciaConsulta(unsigned int s)
{
	memprint(m_header.SecuenciaConsulta,"%05u",s);
	return s;
}

unsigned int CGMHeader::SecuenciaRespuesta()
{
	return (unsigned int)subint(m_header.SecuenciaRespuesta, 0, 5);
}

unsigned int CGMHeader::SecuenciaRespuesta(unsigned int s)
{
	memprint(m_header.SecuenciaRespuesta, "%05u", s);
	return s;
}

char CGMHeader::OrigenConsulta()
{
	return m_header.OrigenConsulta[0];
}

int CGMHeader::OrigenConsulta(char o)
{
	m_header.OrigenConsulta[0] = o;
	/* Dejamos un valor de retorno para cuando se valide el parámetro */
	return 0;
}

char CGMHeader::OrigenRespuesta()
{
	return m_header.OrigenRespuesta[0];
}

int CGMHeader::OrigenRespuesta(char o)
{
	m_header.OrigenRespuesta[0] = o;
	/* Dejamos un valor de retorno para cuando se valide el parámetro */
	return 0;
}

unsigned int CGMHeader::IdOrigen()
{
	return (unsigned int)subint(m_header.IdOrigen, 0, 5);
}

int CGMHeader::IdOrigen(unsigned int id)
{
	memprint(m_header.IdOrigen, "%05u", id);
	/* Dejamos un valor de retorno para cuando se valide el parámetro */
	return 0;
}

unsigned int CGMHeader::IdRouter()
{
	return (unsigned int)subint(m_header.IdRouter, 0, 5);
}

int CGMHeader::IdRouter(unsigned int id)
{
	memprint(m_header.IdRouter, "%05u", id);
	/* Dejamos un valor de retorno para cuando se valide el parámetro */
	return 0;
}

unsigned int CGMHeader::IdDestino()
{
	return (unsigned int)subint(m_header.IdDestino, 0, 5);
}

int CGMHeader::IdDestino(unsigned int id)
{
	memprint(m_header.IdDestino, "%05u", id);
	/* Dejamos un valor de retorno para cuando se valide el parámetro */
	return 0;
}

void CGMHeader::TimeStamp(unsigned long t)
{
	memprint(m_header.TimeStamp,"%010lu",t);
}

unsigned long CGMHeader::TimeStamp()
{
	return (unsigned int)sublong(m_header.TimeStamp, 0,10);
}

unsigned int CGMHeader::CodigoRetorno()
{
	return (unsigned int)subint(m_header.CodigoRetorno, 0,5);
}

void CGMHeader::CodigoRetorno(unsigned int c)
{
	memprint(m_header.CodigoRetorno, "%05u", c);
}

const char* CGMHeader::Crc()
{
	memset(m_Crc, 0, sizeof(m_Crc));
	memcpy(m_Crc, m_header.Crc, 16);
	return (const char*)&m_Crc[0];
}

int CGMHeader::Crc(const char* crc)
{
	if(!crc) return -1;
	if(strlen(crc) < sizeof(m_Crc))
	{
		memprint(m_header.Crc,"%-16.16s",crc);
		return 0;
	}
	else
	{
		return -1;
	}
}

unsigned long CGMHeader::TamMensaje()
{
	return (unsigned long)sublong(m_header.TamMensaje, 0, 10);
}

void CGMHeader::TamMensaje(unsigned long t)
{
	memprint(m_header.TamMensaje, "%10lu", t);
}

unsigned long CGMHeader::IndiceMensaje()
{
	return (unsigned long)sublong(m_header.IndiceMensaje, 0, 10);
}

void CGMHeader::IndiceMensaje(unsigned long i)
{
	memprint(m_header.IndiceMensaje, "%10lu", i);
}

unsigned long CGMHeader::TamMaxMensaje()
{
	return (unsigned long)sublong(m_header.TamMaxMensaje, 0, 10);
}

void CGMHeader::TamMaxMensaje(unsigned long t)
{
	memprint(m_header.TamMaxMensaje, "%10lu", t);
}

unsigned long CGMHeader::TamTotMensaje()
{
	return (unsigned long)sublong(m_header.TamTotMensaje, 0, 10);
}

void CGMHeader::TamTotMensaje(unsigned long t)
{
	memprint(m_header.TamTotMensaje, "%10lu", t);
}

char CGMHeader::TipoRespuesta(char tc)
{
        switch(tc)
        {
        case GM_MSG_TYPE_CR:
                return GM_MSG_TYPE_R_CR;
        case GM_MSG_TYPE_MSG:
                return GM_MSG_TYPE_R_MSG;
        case GM_MSG_TYPE_INT:
                return GM_MSG_TYPE_R_INT;
        case GM_MSG_TYPE_QUE:
                return GM_MSG_TYPE_R_QUE;
        case GM_MSG_TYPE_NOT:
                return GM_MSG_TYPE_R_NOT;
        default:
                return '!';
        }
}

