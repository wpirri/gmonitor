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

#include <unistd.h>

#include <sincmem.h>

#define SSHMEM_READ_SEM		0
#define SSHMEM_WRITE_SEM	1
#define SSHMEM_MAX_SEM		2

#define WRITE_BLOQ_IN		1
#define WRITE_BLOQ_OUT		-1

CSincMem::CSincMem()
{
	m_key = -1;
}

CSincMem::CSincMem(int key)
{
	m_key = key;
}

CSincMem::~CSincMem()
{
	Close();
}

int CSincMem::Create(int length)
{
	/* asigno la clave de los semaforos */
	if(CSincro::Key(m_key) != 0) return -1;
	/* asigno la cleve de la memoria compartida */
	if(CShMem::Key(m_key+1) != 0) return -1;
	/* creo los semaforos que necesito para el control de acceso a memoria */
	if(CSincro::Create(SSHMEM_MAX_SEM, -1) != 0) return -1;
	m_length = length;
	/* creo el area de memoria pedida mas el espacio que necesito
           para controlar el acceso, el primer long es para el contador de lectores */
	if(CShMem::Create(m_length + sizeof(long)) != 0)
	{
		CSincro::Close();
		return -1;
	}
	/* una vez terminad la inicializacion abro todos los semaforos */
	Signal(SSHMEM_READ_SEM);
	Signal(SSHMEM_WRITE_SEM);
	return 0;
}

int CSincMem::Open(int length)
{
	if(CSincro::Key(m_key) != 0) return -1;
	if(CShMem::Key(m_key+1) != 0) return -1;
	if(CSincro::Open(SSHMEM_MAX_SEM) != 0) return -1;
	m_length = length;
	if(CShMem::Open(m_length + sizeof(long)) != 0)
	{
		CSincro::Close();
		return -1;
	}
	return 0;
}

void CSincMem::Close()
{
	CShMem::Close();
	CSincro::Close();
}

int CSincMem::SetAt(int pos, const void* data, int length)
{
	int rc;

	if(pos < 0) return -1;
	pos += sizeof(long);
	if(Wait(SSHMEM_WRITE_SEM) != 0) return -1;
	rc = CShMem::SetAt(pos, data, length);
	Signal(SSHMEM_WRITE_SEM);
	return rc;
}

int CSincMem::GetAt(int pos, void* data, int length)
{
	int rc;

	if(pos < 0) return -1;
	pos += sizeof(long);
	if(WriteBloq(WRITE_BLOQ_IN) != 0) return -1;
	/* Agarro el dato que necesito */
	rc = CShMem::GetAt(pos, data, length);
	WriteBloq(WRITE_BLOQ_OUT);
	return rc;
}

/*
	Por medio de este miembro se logra que el primer lector que entre haga el
	bloqueo de escritura y el ultimo en salir lo libere.
*/
int CSincMem::WriteBloq(int in_out)
{
	long count;
	
	/* entro al contador de lectores y si soy el primero bloqueo el semaforo de escritores */
	if(Wait(SSHMEM_READ_SEM) != 0) return -1;
	if(CShMem::GetAt(0, &count, sizeof(long)) != 0)
	{
		Signal(SSHMEM_READ_SEM);
		return -1;
	}
	if(in_out == WRITE_BLOQ_IN)
	{
		count++;
		if(count == 1)
		{
			/* si es el primero bloqueo el semaforo de escritura */
			if(Wait(SSHMEM_WRITE_SEM) != 0)
			{
				Signal(SSHMEM_READ_SEM);
				return -1;
			}
		}
	}
	else if(in_out == WRITE_BLOQ_OUT)
	{
		count--;
		if(count == 0)
		{
			/* si es el ultimo desbloqueo el semaforo de escritura */
			if(Signal(SSHMEM_WRITE_SEM) != 0)
			{
				Signal(SSHMEM_READ_SEM);
				return -1;
			}
		}
		else if(count < 0)
		{
			/* se está llendo uno que no declaró la entrada */
			Signal(SSHMEM_READ_SEM);
			return -1;
		}
	}
	if(CShMem::SetAt(0, &count, sizeof(long)) != 0)
	{
		Signal(SSHMEM_WRITE_SEM);
		Signal(SSHMEM_READ_SEM);
		return -1;
	}
	Signal(SSHMEM_READ_SEM);
	return 0;
}

bool CSincMem::IsOwner()
{
	return CShMem::IsOwner();
}

