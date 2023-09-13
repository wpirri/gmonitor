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

#include <gmerror.h>

CGMError::CGMError()
{
	m_strerror[GME_OK              ] = "OK";
	m_strerror[GME_UNDEFINED       ] = "Error no definido";
	m_strerror[GME_FILE_NOTFOUND   ] = "Archivo no encontrado";
	m_strerror[GME_FILE_NOTOPEN    ] = "Archivo no abierto";
	m_strerror[GME_FILE_DUPLICATED ] = "Archivo duplicado";
	m_strerror[GME_FILE_ERROR      ] = "Error de archivo";
	m_strerror[GME_DIRECTORY_ERROR ] = "Error de directorio";
	m_strerror[GME_HOST_NOTFOUND   ] = "No se encontro el host";
	m_strerror[GME_INVALID_HOST    ] = "Host invalido";
	m_strerror[GME_NOT_CONNECTED   ] = "No conectado";
	m_strerror[GME_COMM_ERROR      ] = "Error de comunicacion";
	m_strerror[GME_MSGQ_TIMEOUT    ] = "Time out en cola de mensajes";
	m_strerror[GME_MSGQ_ERROR      ] = "Error en cola de mensajes";
	m_strerror[GME_INVALID_HEADER  ] = "Error de interpretacion de header";
	m_strerror[GME_INVALID_MESSAGE ] = "Error de interpretacion de mensaje";
	m_strerror[GME_SVR_NOTFOUND    ] = "Error en la busqueda de server";
	m_strerror[GME_SVC_NOTFOUND    ] = "Error en la busqueda de servicio";
	m_strerror[GME_SVC_UNRESOLVED  ] = "Servicio no resuelto por server";
	m_strerror[GME_SVC_DB_ERROR    ] = "Error en la base de servicios";
	m_strerror[GME_TRAN_NOT_ALLOWED] = "Transaccion no permitida";
	m_strerror[GME_UNKNOWN_TRAN    ] = "Transaccion desconocida";
	m_strerror[GME_INVALID_TRAN    ] = "Transaccion invalida";
	m_strerror[GME_TRAN_NOT_INIT   ] = "Transaccion no iniciada";
	m_strerror[GME_SAF_NOT_FOUND   ] = "SAF no encontrado";
	m_strerror[GME_MSGTYP_INV      ] = "Tipo de mensaje invalido";
 	m_strerror[GME_SVCTYP_INV      ] = "Tipo de servicio invalido";
 	m_strerror[GME_NO_DATA         ] = "Ok, pero sin datos";
	m_strerror[ GME_MORE_DATA      ] = "Existen mas datos para el mensaje";

	m_strerror[GME_ERROR_MAX] = "Indice de error fuera de rango";
}

CGMError::~CGMError()
{
	return;
}

string CGMError::Message()
{
	return Message(Last());
}

string CGMError::Message(int error)
{
	if(error < 0 ) error *= (-1);

	if(error > 0 && error < GME_ERROR_MAX)
	{
		return m_strerror[error];
	}
	else
	{
		return m_strerror[GME_ERROR_MAX];
	}
}

void CGMError::Last(int error)
{
	m_last_error = error;
}

int CGMError::Last()
{
	return m_last_error;
}

