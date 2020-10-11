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
#ifndef _GMERROR_H_
#define _GMERROR_H_

#include <string>
using namespace std;

#define GME_OK                0
#define GME_NO_ERROR          GME_OK
#define GME_UNDEFINED         1   /* Error aún no definido */
#define GME_FILE_NOTFOUND     2   /* No se encontro archivo */
#define GME_FILE_NOTOPEN      3   /* No se pudo abrir archivo */
#define GME_FILE_DUPLICATED   4   /* Archivo ya existe */
#define GME_FILE_ERROR        5   /* Error de archivo */
#define GME_DIRECTORY_ERROR   6   /* Error de directorio */
#define GME_HOST_NOTFOUND     7   /* Error en resolucion de nombre */
#define GME_INVALID_HOST      8   /* Host invalido */
#define GME_NOT_CONNECTED     9   /* No esta conectado */
#define GME_COMM_ERROR       10   /* Error de comunicacion */
#define GME_MSGQ_TIMEOUT     11   /* time-out en la cola de mensajes */
#define GME_MSGQ_ERROR       12   /* Error en la cola de mensajes */
#define GME_INVALID_HEADER   13   /* Error al decodificar el header del mensaje */
#define GME_INVALID_MESSAGE  14   /* Error al decodificar el mensaje */
#define GME_SVR_NOTFOUND     15   /* Error en la busqueda de servers */
#define GME_SVC_NOTFOUND     16   /* El servicio no es resuelto */
#define GME_SVC_UNRESOLVED   17   /* La funcion no es resuelta por el server */
#define GME_SVC_DB_ERROR     18   /* Error en la bas e de datos de servicios */
#define GME_TRAN_NOT_ALLOWED 19   /* intento de comensar una segunda transaccion */
#define GME_UNKNOWN_TRAN     20   /* transaccion no existe en el server*/
#define GME_INVALID_TRAN     21   /* no se pudo iniciar transaccion */
#define GME_TRAN_NOT_INIT    22   /* transaccopn no iniciada */
#define GME_SAF_NOT_FOUND    23   /* no se encontro SAF */
#define GME_MSGTYP_INV       24   /* tipo de mensaje inválido */
#define GME_SVCTYP_INV       24   /* tipo de servicio inválido */

#define GME_MORE_DATA        99   /* Indica mas datos en una comunicacion interactiva */
/* ============================ */
#define GME_ERROR_MAX       100
#define GME_USER_ERROR        GME_ERROR_MAX

class CGMError
{
public:
	CGMError();
	virtual ~CGMError();

	string Message();
	string Message(int error);
	void Last(int error);
	int Last();

protected:
	string m_strerror[GME_ERROR_MAX+1];
	int m_last_error;
};
#endif /* _GMERROR_H_*/

