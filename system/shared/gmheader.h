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
#ifndef _GMHEADER_H_
#define _GMHEADER_H_

#include <gmbuffer.h>

#include <string>
using namespace std;

#define VERSION_HEADER "000"

class CGMHeader
{
public:
	CGMHeader();
	virtual ~CGMHeader();

	const char *GetHeader();
	unsigned int GetHeaderLen();
	int SetHeader(const char* msg);
	int SetHeader(CGMBuffer& buffer);
	int SetHeader(CGMBuffer* buffer);

	char TipoMensaje();
	void TipoMensaje(char t);
	unsigned int VersionHeader();
	const char* IdUsuario();
	int IdUsuario(const char* id);
	int IdUsuario(string sid);
	const char* IdCliente();
	int IdCliente(const char* id);
	int IdCliente(string sid);
	const char* Key();
	int Key(const char* key);
	int Key(string skey);
	const char* IdGrupo();
	int IdGrupo(const char* id);
	int IdGrupo(string id);
	const char* Funcion();
	int Funcion(const char* f);
	int Funcion(string sf);
	unsigned int IdTrans();
	unsigned int IdTrans(unsigned int id);
	int IdCola();
	int IdCola(int id);
	unsigned int IdMoreData();
	unsigned int IdMoreData(unsigned int id);
	unsigned int SecuenciaConsulta();
	unsigned int SecuenciaConsulta(unsigned int s);
	unsigned int SecuenciaRespuesta();
	unsigned int SecuenciaRespuesta(unsigned int s);
	char OrigenConsulta();
	int OrigenConsulta(char o);
	char OrigenRespuesta();
	int OrigenRespuesta(char o);
	unsigned int IdOrigen();
	int IdOrigen(unsigned int id);
	unsigned int IdRouter();
	int IdRouter(unsigned int id);
	unsigned int IdDestino();
	int IdDestino(unsigned int id);
	/* hora de inicio del mensaje puesta por el cliente */
	unsigned long TimeStamp();
	void TimeStamp(unsigned long t);
	unsigned int CodigoRetorno();
	void CodigoRetorno(unsigned int c);
	const char* Crc();
	/* Tamaño del bloque de datos contenido en este mensaje */
	unsigned long TamMensaje();
	void TamMensaje(unsigned long i);
	/* Indice del inicio del mensaje, salvo para interacticos debe ser 0 */
	unsigned long IndiceMensaje();
	void IndiceMensaje(unsigned long i);
	/* Tamaño maximo del mensaje a transferir dterminado por el cliente
	debe ser 0 para todos salvo para los interactivos (0 es sin limite) */
	unsigned long TamMaxMensaje();
	void TamMaxMensaje(unsigned long t);
	/* Tamaño total del mensaje, salvo para los interactivos debe ser
	igual a TamMensaje */
	unsigned long TamTotMensaje();
	void TamTotMensaje(unsigned long t);

	char TipoRespuesta(char tc);

protected:
	typedef struct _GMONITOR_HEADER
	{
		char	TipoMensaje[1];
		char	VersionHeader[3];
		char	IdUsuario[16];
		char	IdCliente[16];
		char	Key[16];
		char	IdGrupo[16];
		char	Funcion[32];
		char	IdTrans[5];
		char	IdCola[5];
		char	IdMoreData[5];
		char	SecuenciaConsulta[5];
		char	SecuenciaRespuesta[5];
		char	OrigenConsulta[1];
		char	OrigenRespuesta[1];
		char	IdOrigen[5];
		char	IdRouter[5];
		char	IdDestino[5];
		char	TimeStamp[10];
		char	CodigoRetorno[5];
		char	Crc[16];
		char	TamMensaje[10];
		char	IndiceMensaje[10];
		char	TamMaxMensaje[10];
		char	TamTotMensaje[10];
	} GMONITOR_HEADER;

	int Crc(const char* crc);

	GMONITOR_HEADER m_header;
private:
	/* estas se usan para devolver dato */
	char m_IdUsuario[17];
	char m_IdCliente[17];
	char m_Key[17];
	char m_IdGrupo[17];
	char m_Funcion[33];
	char m_Crc[17];

	void VersionHeader(unsigned int v);
};

#endif /* _GMHEADER_H_ */
