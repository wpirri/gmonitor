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
    Funciones para la creacion y utilizacion de colas de mensajes
*/

#include <cmsg.h>

#include <string>
#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
using namespace std;

#include <unistd.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>

CMsg::CMsg()
{
	m_fd = -1;
	m_error = 0;
	m_key = -1;
	m_priority = MSG_DEFAULT_PRIORITY;
}

CMsg::CMsg(int fd)
{
	m_fd = fd;
	m_error = 0;
	m_key = -1;
	m_priority = MSG_DEFAULT_PRIORITY;
}

CMsg::~CMsg()
{
	Close();
}

int CMsg::Open(const char *path)
{
	int rc;
	char appname[FILENAME_MAX];

	if(path)
	{
		strcpy(appname, path);
	}
	else
	{
		/* obtengo el path al ejecutable que esta corriendo LINUX */
		rc = readlink("/proc/self/exe", appname, sizeof(appname));
		if(rc < 0)
		{
			/* FreeBSD */
			rc = readlink("/proc/self/file", appname, sizeof(appname));
		}
		if(rc < 0)
		{
			return -1;
		}
		appname[rc] = '\0';
	}
	/* entro en un loop para buscar la clave para la cola
	esto me permite hacer varias colas para la misma aplicacion */
	m_index = -1;
	do
	{
		if(++m_index > 255) break;
		m_key = ftok(appname, m_index);
		if(m_key == -1) return -1;
		m_fd = msgget(m_key, IPC_CREAT|IPC_EXCL|0600);
		if(m_fd == -1 && errno != EEXIST) break;
	} while (m_fd == -1);

	if(m_fd != -1)
	{
		return 0;
	}
	else
	{
		m_error = errno;
		return -1;
	}
}

int CMsg::Close()
{
	if(m_fd != -1)
	{
		msgctl(m_fd, IPC_RMID, NULL);
		m_fd = -1;
	}
	return 0;
}

/*
	Responde un Query
*/
int CMsg::Send(int key, CGMBuffer* msg)
{
	char data[GM_COMM_MSG_LEN];
	int fd;

	/* verifico que se haya hecho el Open */
	if(m_fd == -1) return -1;
	if((fd = msgget(key, 0)) == -1)
	{
		m_error = errno;
		return -1;
	}
	//data = (char*)malloc(sizeof(long) + msg->Length());
	memcpy(data, &m_priority, sizeof(long));
	/* vuelvo a cargar el default */
	m_priority = MSG_DEFAULT_PRIORITY;
	memcpy((char*)(data+sizeof(long)), msg->Data(), msg->Length());
	if(msgsnd(fd, data, sizeof(long) + msg->Length(), 0) != 0)
	{
		m_error = errno;
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int CMsg::Send(int key, const char* msg, long len)
{
	char data[GM_COMM_MSG_LEN];
	int fd;

	/* verifico que se haya hecho el Open */
	if(m_fd == -1) return -1;
	if((fd = msgget(key, 0)) == -1)
	{
		m_error = errno;
		return -1;
	}
	//data = (char*)malloc(sizeof(long) + msg->Length());
	memcpy(&data[0], &m_priority, sizeof(long));
	/* vuelvo a cargar el default */
	m_priority = MSG_DEFAULT_PRIORITY;
	memcpy((char*)((&data[0])+sizeof(long)), msg, len);
	if(msgsnd(fd, (char*)&data[0], sizeof(long) + len, 0) != 0)
	{
		m_error = errno;
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

/*
	Recibe un Query, to en cent de seg., -1 infinito
*/
long CMsg::Receive(CGMBuffer* msg, long to_cs)
{
	char data[GM_COMM_MSG_LEN];
	long rc, len;
	long from;

	/* verifivc que se haya hecho el Open */
	if(m_fd == -1) return (-1);
	rc = Wait(to_cs);
	if(rc < 0) return (-2); /* error */
	if(rc == 0) return 0; /* time-out */
	//data = (char*)malloc(rc + sizeof(long));
	if((len = msgrcv(m_fd, data, rc + sizeof(long), -10, 0)) < 0)
	{
		m_error = errno;
		//free(data);
		return (-3);
	}
	/* copy en la variable from el segundo long que viene en el mensaje
	que corresponde al ID de la cola donde se espera la respuesta */
	/* el primer long es el canal del mensaje, va de 1 a 10 seg�n como se haya enviado
	   mas adelante se va a usar para definr priorida de mensajes */
	/*memcpy(, data, sizeof(long));*/
	/* el segundo es el ID de la cola para responder */
	memcpy(&from, (char*)(data + sizeof(long)), sizeof(long));
	/* el mensaje real viene 2 long despu del comienzo del buffer */
	*msg = "";
	msg->Add((char*)(data + (2*sizeof(long))), len - (2*sizeof(long)));
    //free(data);
	return (from>0)?from:(-5);
}

long CMsg::Receive(char* msg, long max_len, long to_cs)
{
	char data[GM_COMM_MSG_LEN];
	long rc, len;
	long from;

	/* verifivc que se haya hecho el Open */
	if(m_fd == -1) return (-1);
	rc = Wait(to_cs);
	if(rc < 0) return (-2); /* error */
	if(rc == 0) return 0; /* time-out */
	//data = (char*)malloc(rc + sizeof(long));
	if((len = msgrcv(m_fd, (char*)&data[0], rc + sizeof(long), -10, 0)) < 0)
	{
		m_error = errno;
		//free(data);
		return (-3);
	}
	/* copy en la variable from el segundo long que viene en el mensaje
	que corresponde al ID de la cola donde se espera la respuesta */
	/* el primer long es el canal del mensaje, va de 1 a 10 seg�n como se haya enviado
	   mas adelante se va a usar para definr priorida de mensajes */
	/*memcpy(, data, sizeof(long));*/
	if(len <= max_len)
	{
		/* el segundo es el ID de la cola para responder */
		memcpy(&from, (char*)((&data[0])+sizeof(long)), sizeof(long));
		/* el mensaje real viene 2 long despu del comienzo del buffer */
		memcpy(msg, (char*)((&data[0])+(2*sizeof(long))), len - (2*sizeof(long)));
		//free(data);
		return (from>0)?from:(-5);
	}
	else
	{
		return (-4);
	}
}

int CMsg::Query(int key, CGMBuffer* qmsg, CGMBuffer* rmsg, long to_cs)
{
	int fd;
	char data[GM_COMM_MSG_LEN];
	long rc, len;
	bool close_on_exit = false;

	if( key <= 0 || !qmsg || !rmsg ) return (-1); /* deben existir */
	/* verifico que se haya hecho el Open */
	if(m_fd == -1)
	{
		if(Open(NULL) != 0)
		{
			return (-2);
		}
		close_on_exit = true;
	}
	/* tomo un fd a la cola */
	if((fd = msgget(key, 0)) == -1)
	{
		m_error = errno;
		return (-3);
	}
	/* aloco memoria para el numero de canal, el ID de la cola de retorno y los datos */
	//data = (char*)malloc((2*sizeof(long)) + qmsg->Length());
	/* en el primer long es el canal */
	memcpy(data, &m_priority, sizeof(long));
	/* vuelvo a cargar el default */
	m_priority = MSG_DEFAULT_PRIORITY;
	/* despu�s el ID de la cola para la respuesta */
	memcpy((char*)(data+sizeof(long)), &m_key, sizeof(long));
	/* despu�s los datos */
	memcpy((char*)(data+(2*sizeof(long))), qmsg->Data(), qmsg->Length());
	/* envio */
	if(msgsnd(fd, data, (2*sizeof(long)) + qmsg->Length(), 0) == -1)
	{
		m_error = errno;
		//free(data);
		close(fd);
		if(close_on_exit) Close();
		return (-4);
	}
	//free(data);
	close(fd);
	/* recepcion */
	rc = Wait(to_cs);
	if(rc < 0)
	{
		if(close_on_exit) Close();
		return (-5); /* error */
	}
	if(rc == 0)
	{
		if(close_on_exit) Close();
		return 0; /* time-out */
	}
	/* aloco memoria suficiente para lo que haya en la cola */
	//data = (char*)malloc(rc + sizeof(long));
	/* me lo traigo */
	if((len = msgrcv(m_fd, data, rc + sizeof(long), -10, 0)) < 0)
	{
		m_error = errno;
		//free(data);
		if(close_on_exit) Close();
		return (-6);
	}
	*rmsg = "";
	rmsg->Add((char*)(data + sizeof(long)), len - sizeof(long));
	//free(data);
	if(close_on_exit) Close();
	return (len - sizeof(long));
}

long CMsg::Query(int key, const char* qmsg, long qlen, char* rmsg, long rmax_len, long to_cs)
{
	int fd;
	char data[GM_COMM_MSG_LEN];
	long rc, len;
	bool close_on_exit = false;

	if( key <= 0 || !qmsg || !rmsg ) return (-1); /* deben existir */
	/* verifico que se haya hecho el Open */
	if(m_fd == -1)
	{
		if(Open(NULL) != 0)
		{
			return (-2);
		}
		close_on_exit = true;
	}
	/* tomo un fd a la cola */
	if((fd = msgget(key, 0)) == -1)
	{
		m_error = errno;
		return (-3);
	}
	/* en el primer long es el canal */
	memcpy(&data[0], &m_priority, sizeof(long));
	/* vuelvo a cargar el default */
	m_priority = MSG_DEFAULT_PRIORITY;
	/* despues el ID de la cola para la respuesta */
	memcpy((char*)((&data[0])+sizeof(long)), &m_key, sizeof(long));
	/* despues los datos */
	memcpy((char*)((&data[0])+(2*sizeof(long))), qmsg, qlen);
	/* envio */
	if(msgsnd(fd, &data[0], (2*sizeof(long)) + qlen, 0) == -1)
	{
		m_error = errno;
		//free(data);
		close(fd);
		if(close_on_exit) Close();
		return (-4);
	}
	//free(data);
	close(fd);
	/* recepcion */
	rc = Wait(to_cs);
	if(rc < 0)
	{
		if(close_on_exit) Close();
		return (-5); /* error */
	}
	if(rc == 0)
	{
		if(close_on_exit) Close();
		return 0; /* time-out */
	}
	/* aloco memoria suficiente para lo que haya en la cola */
	//data = (char*)malloc(rc + sizeof(long));
	/* me lo traigo */
	if((len = msgrcv(m_fd, (char*)&data[0], rc + sizeof(long), -10, 0)) < 0)
	{
		m_error = errno;
		//free(data);
		if(close_on_exit) Close();
		return (-6);
	}
	if(len <= rmax_len)
	{
		memcpy(rmsg, (char*)((&data[0])+sizeof(long)), len-sizeof(long));
	}
	else
	{
		len = 0;
	}
	//free(data);
	if(close_on_exit) Close();
	return (len - sizeof(long));
}

int CMsg::GetKey()
{
	return m_key;
}

int CMsg::GetId()
{
	return m_fd;
}

int CMsg::GetIndex()
{
	return m_index;
}

long CMsg::GetCount()
{
	struct msqid_ds qds;

	/* verifivo que se haya hecho el Open */
	if(m_fd == -1) return -1;
	/* obtengo el status de la cola */
	if(msgctl(m_fd, IPC_STAT, &qds) != 0)
	{
		m_error = errno;
		return -1;
	}
	return (long)qds.msg_qnum;
}

long CMsg::GetRemoteCount(int key)
{
	int fd;
	struct msqid_ds qds;

	if( key <= 0 ) return -1; /* debe existir */
	/* tomo un fd a la cola */
	if((fd = msgget(key, 0)) == -1)
	{
		return -1;
	}
	/* obtengo el status de la cola */
	if(msgctl(fd, IPC_STAT, &qds) != 0)
	{
		close(fd);
		return -1;
	}
	close(fd);
	return (long)qds.msg_qnum;
}

int CMsg::SetPriority(int p)
{
	if(p > 0 || p <= 10)
	{
		m_priority = p;
		return 0;
	}
	else return -1;
}

int CMsg::Wait(long to_cs)
{
	long tto;
	struct msqid_ds qds;
	time_t t;

	/* verifico que se haya hecho el Open */
	if(m_fd == -1) return -1;
	/* espero por los datos */
	if(to_cs > 1000)
	{
		/* tiempos de espera mayores a 10 seg */
		t = time(&t);
		for( tto = t + (to_cs / 100); tto > t; t = time(&t) )
		{
			// pego una mirada a la cola
			if(msgctl(m_fd, IPC_STAT, &qds) != 0)
			{
				m_error = errno;
				return -1;
			}
			/* si llego algo salto */
			if(qds.msg_qnum > 0)
			{
				/* si recibo algo devuelvo el tam de la cola */
				return qds.msg_cbytes;
			}
			else
			{
				/* si no recibo nada hago un pausita */
				usleep(10000l);
			}
		}
	}
	else if( to_cs >= 0)
	{
		/* tiempos de espera menores a 10 seg cn presici�n de sentesimas */
		for(tto = to_cs ; tto ; tto--)
		{
			// pego una mirada a la cola
			if(msgctl(m_fd, IPC_STAT, &qds) != 0)
			{
				m_error = errno;
				return -1;
			}
			/* si llego algo salto */
			if(qds.msg_qnum > 0)
			{
				/* si recibo algo devuelvo el tam de la cola */
				return qds.msg_cbytes;
			}
			else
			{
				/* si no recibo nada hago un pausita */
				usleep(10000l);
			}
		}
	}
	else
	{
		/* espera infinita */
		while(1)
		{
			// pego una mirada a la cola
			if(msgctl(m_fd, IPC_STAT, &qds) != 0)
			{
				m_error = errno;
				return -1;
			}
			/* si llego algo salto */
			if(qds.msg_qnum > 0)
			{
				/* si recibo algo devuelvo el tam de la cola */
				return qds.msg_cbytes;
			}
			else
			{
				/* si no recibo nada hago un pausita */
				usleep(10000l);
			}
		}
	}
	return 0; /* salio por to */
}

int CMsg::GetRemoteKey(const char* path, int index)
{
	return ftok(path, index);
}

