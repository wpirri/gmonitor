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
#include <sys/types.h>
#include <sys/ipc.h>

#include <sincro.h>

#ifndef HAVE_UNION_SEMUN
union semun
{
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

#define SINC_WAIT	-1
#define SINC_SIGNAL	1

CSincro::CSincro()
{
	m_SemCount = 0;
	m_SemId = -1;
	m_own = false;
	m_key = -1;
}

CSincro::CSincro(int key)
{
	m_SemCount = 0;
	m_SemId = -1;
	m_own = false;
	m_key = key;
}

CSincro::~CSincro()
{
	Close();
}

int CSincro::Key(int key)
{
	if(m_key != -1) return -1;
	m_key = key;
	return 0;
}

int CSincro::Create(int count, int init_val)
{
	Close();
	if(m_key == -1) return -1;
	m_SemCount = count;
	if((m_SemId = semget(m_key, m_SemCount, IPC_CREAT|IPC_EXCL|0600)) == -1)
	{
		return -1;
	}
	/* para saber que soy la clase que creó el semaforo */
	m_own = true;
	/* valor inicial de los semáforos SIGNALED */
	for(int i = 0; i < count; i++)
	{
		Set(i, init_val);
	}
	return 0;
}

int CSincro::Open(int count)
{
	Close();
	if(m_key == -1) return -1;
	m_SemCount = count;
	if((m_SemId = semget(m_key, m_SemCount, 0600)) == -1)
	{
		return -1;
	}
	return 0;
}

void CSincro::Close()
{
	if(m_own && m_SemId != -1)
	{
		semctl(m_SemId, 0, IPC_RMID, 0);
	}
	m_SemId = -1;
	m_own = false;
}

int CSincro::Wait(int sem)
{
	if(m_SemId != -1 && sem < m_SemCount)
	{
		return semop(m_SemId, SemBuff(sem, SINC_WAIT), 1);
	}
	return -1;
}

int CSincro::Signal(int sem)
{
	if(m_SemId != -1 && sem < m_SemCount)
	{
		return semop(m_SemId, SemBuff(sem, SINC_SIGNAL), 1);
	}
	return -1;
}

int CSincro::Set(int sem, int val)
{
	if(m_SemId != -1 && sem < m_SemCount && m_own)
	{
		semun arg;
		arg.val = val;
		return semctl(m_SemId, sem, SETVAL, arg);
	}
	return -1;
}

sembuff* CSincro::SemBuff(int sem, int opt)
{
	static sembuff m_SemBuff;

	m_SemBuff.sem_num = sem;
	m_SemBuff.sem_op = opt;
	m_SemBuff.sem_flg = 0;
	return &m_SemBuff;
}

