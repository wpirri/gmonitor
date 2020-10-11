/***************************************************************************
    Copyright (C) 2003   Walter Pirri

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/
#include <gmonitor/gmerror.h>
#include <gmonitor/gmontdb.h>
/*#include <gmonitor/gmstring.h>*/
#include <gmonitor/gmswaited.h>

#include <string>
#include <iostream>
#include <csignal>
using namespace std;

#include <unistd.h>
#include <time.h>
#include <syslog.h>

CGMServerWait *m_pServer;
void OnClose(int sig);
int Check();

int wpower_init();
int wpower_status();
void wpower_check_ups();

time_t time_count;

int main(int argc, char** argv, char** env)
{
	int rc;
	char fn[33];
	unsigned long inlen;
	char ch[1];
	/*unsigned long outlen;*/
	int wait;
	int status;

	signal(SIGPIPE, SIG_IGN);
        signal(SIGKILL,         OnClose);
        signal(SIGTERM,         OnClose);
        signal(SIGSTOP,         OnClose);
        signal(SIGABRT,         OnClose);
        signal(SIGQUIT,         OnClose);
        signal(SIGINT,          OnClose);
        signal(SIGILL,          OnClose);
        signal(SIGFPE,          OnClose);
        signal(SIGSEGV,         OnClose);
        signal(SIGBUS,          OnClose);

	m_pServer = new CGMServerWait;
	m_pServer->Init("statups");
	m_pServer->m_pLog->Add(1, "Iniciando server de Monitoreo de UPS");

	if(wpower_init() == 0)
	{
		syslog(LOG_INFO, "MONITOR-UPS: Iniciado");
	}
	else
	{
		syslog(LOG_ERR, "MONITOR-UPS: UPS no conectasa");
	}

	if(( rc =  m_pServer->Suscribe("status_ups", GM_MSG_TYPE_CR)) != GME_OK)
	{
		m_pServer->m_pLog->Add(1, "ERROR %i al suscribir servicio status_ups", rc);
		OnClose(0);
	}
	m_pServer->SetLogLevel(20);
	time_count = 0;
	wait = Check();
	while((rc = m_pServer->Wait(fn, ch, 1, &inlen, wait * 100)) >= 0)
	{
		if(rc > 0)
		{
			m_pServer->m_pLog->Add(100, "Query recibido fn = [%s] rc = %i", fn, rc);
			if( !strcmp(fn, "status_ups"))
			{
				status = wpower_status();
				m_pServer->m_pLog->Add(100, "Status de UPS = %i", status);
				if(m_pServer->Resp(&status, sizeof(int), GME_OK) != GME_OK)
				{
					/* error al responder */
					m_pServer->m_pLog->Add(50, "ERROR al responder mensaje");
				}
			}
			else
			{
				m_pServer->m_pLog->Add(50, "GME_SVC_NOTFOUND");
				m_pServer->Resp(NULL, 0, GME_SVC_NOTFOUND);
			}
		}
		/* me fijo si hay que ver el estado de la UPS */
		wait = Check();
		m_pServer->m_pLog->Add(100, "Proximo control en %i segundos", wait);
	}
	m_pServer->m_pLog->Add(50, "ERROR en la espera de mensajes");
	OnClose(0);
	return 0;
}

void OnClose(int sig)
{
	m_pServer->m_pLog->Add(1, "Exit on signal %i", sig);
	m_pServer->UnSuscribe("status_ups", GM_MSG_TYPE_CR);
	delete m_pServer;
	exit(0);
}

/*
	Controla el estado de la UPS si ya pasaron 5 segundos del ultimo control
	sino no lo controla.
	Siempre devuelve el tiempo que falta para el proximo control.
*/
int Check()
{
	time_t t;
	t = time(&t);

	if(t >= time_count)
	{
		m_pServer->m_pLog->Add(100, "Hora de controlar estado de UPS");
		wpower_check_ups();
		time_count = t + 5; /* chequear cada 5 segundos */
	}
	return (time_count - t);
}

