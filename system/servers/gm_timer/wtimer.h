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
#ifndef _WTIMER_H_
#define _WTIMER_H_

#include <vector>
#include <string>
using namespace std;

#include <gmbuffer.h>

#include <time.h>

class CWTimer
{
public:
	CWTimer();
	virtual ~CWTimer();

	/* Programa un timer para un momento determinado */
	unsigned int At(unsigned long end, const char* servicio, char tipo,
			unsigned int trans, const char* user,
			const char* client, const char* key, const char* group,
			const void* msg, unsigned long msg_len);
	/* Programa un timer para un delay determinado que puede reprogramarse
           automaticamente */
	unsigned int Set(unsigned int seconds, const char* servicio, char tipo,
			unsigned int trans, const char* user,
			const char* client, const char* key, const char* group,
			const void* msg, unsigned long msg_len, bool repeat);
	/* Devuelve el tiempo faltante para el vencimiento del primer timer en centesimas de segundo */
	long Rest(long max_wait = -1);
	/* Devuelve el indice a un timer vencido o -1 si no hay ninguno */
	int Check();
	/* Devuelve los datos de un timer */
	int Get(unsigned int id, char* servicio, char* tipo,
			unsigned int trans, const char* user,
			const char* client, const char* key, const char* group,
			void** msg, unsigned long* msg_len);
	/* Se debe convocar al terminar de procesar un timer */
	int Ok(unsigned int id);
	/* Borra un timer por Id, verifica identidad del cliente */
	int DelId(unsigned int id,
				const char* user, const char* client,
				const char* key, const char* group);
	/* Borra un o varios timer por transacción */
	int DelTrans(unsigned int trans);
	protected:

	unsigned int GetNewId();

	typedef struct _ST_WTIMER
	{
		unsigned long id; /*para poder hacer referencia al borrar*/
		time_t start_t;		/* time_t del seteo del timer */
		time_t end_t;		/* time_t de vencimiento del timer */
		unsigned int restart;	/* si es distinto de cero luego de
                                           ejecutar el timer lo vuelve a setear
                                           sumandole este valor al end_t */
		/* datos para control de seguridad al modificar o borrar */
		unsigned int trans;
		string user;
		string client;
		string key;
		string group;
		/* datos para generar el evento */
		char servicio[32];
		char tipo;
		CGMBuffer* buffer;
	} ST_WTIMER;

	unsigned int m_last_id;

	vector <ST_WTIMER> m_vTimer;
};
#endif /* _GWTIMER_H_ */

