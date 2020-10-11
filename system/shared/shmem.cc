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
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>

#include "shmem.h"

CShMem::CShMem()
{
	m_shm_id = -1;
	m_pshm = (void*)-1;
	m_shm_length = 0;
	m_key = -1;
	m_own = false;
}

CShMem::CShMem(int key)
{
	m_shm_id = -1;
	m_pshm = (void*)-1;
	m_shm_length = 0;
	m_key = key;
	m_own = false;
}

CShMem::~CShMem()
{
	Close();
}

int CShMem::Key(int key)
{
	if(m_key != -1) return -1;
	m_key = key;
	return 0;
}

int CShMem::Create(int length)
{
	/* me aseguro de no estar olvidando un area creada anteriormente */
	Close();
	if(m_key == -1) return -1;
	/* creo un area de memoria compartida */
	m_shm_id = shmget(m_key, length, IPC_CREAT|IPC_EXCL|0600);
	if(m_shm_id == -1) return -1;
	/* me atacho al area de memoria creada */
	m_pshm = shmat(m_shm_id, (void*)0, 0);
	if(m_pshm == (void*)-1)
	{
		Close();
		return -1;
	}
	m_shm_length = length;
	memset(m_pshm, 0, m_shm_length);
	m_own = true;
	return 0;
}

int CShMem::Open(int length)
{
	/* me aseguro de no estar olvidando un area creada anteriormente */
	Close();
	if(m_key == -1) return -1;
	/* tomo el id al area de memoria */
	m_shm_id = shmget(m_key, length, 0600);
	if(m_shm_id == -1) return -1;
	/* me atacho al area de memoria */
	m_pshm = shmat(m_shm_id, (void*)0, 0);
	if(m_pshm == (void*)-1)
	{
		Close();
		return -1;
	}
	m_shm_length = length;
	return 0;
}

void CShMem::Close()
{
	if(m_pshm && m_pshm != (void*)-1)
	{
		shmdt(m_pshm);
	}
	if(m_shm_id != -1 && m_own)
	{
		shmctl(m_shm_id, IPC_RMID, NULL);
	}
	m_shm_id = -1;
	m_pshm = (void*)-1;
}

int CShMem::SetAt(int pos, const void* data, int length)
{
	if(m_pshm == (void*)-1) return -1;
	if( (!data && length) || length < 0 ) return -1;
	memcpy( (char*)m_pshm + pos, data, length);
	return 0;
}

int CShMem::GetAt(int pos, void* data, int length)
{
	if(m_pshm == (void*)-1) return -1;
	if( (!data && length) || length < 0 ) return -1;
	memcpy(data, (char*)m_pshm + pos, length);
	return 0;
}

bool CShMem::IsOwner()
{
	return m_own;
}

