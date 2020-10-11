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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "gmerror.h"
#include "gmontdb.h"
#include "gmstring.h"

#include "gmswaited.h"

#include <string>
#include <iostream>
#include <csignal>
using namespace std;

#include <unistd.h>

CGMServerWait *m_pServer;
void OnClose(int sig);
char *m_pInBuffer;

int main(int argc, char** argv, char** env)
{
	int rc;
	char fn[33];
	unsigned long inlen;
	int wait;

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

	m_pInBuffer = NULL;
	m_pServer = new CGMServerWait;
	m_pServer->Init("nombre-del-server");

	if(( rc =  m_pServer->Suscribe("algun-servicio", 'M')) != GME_OK)
	{
		m_pServer->m_pLog->Add(1, "ERROR %i al suscribir algun-servicio", rc);
		OnClose(0);
	}

	m_pInBuffer = (char*)calloc(1024, sizeof(char));

	wait = -1;
	while((rc = m_pServer->Wait(fn, m_pInBuffer, 1024, &inlen, wait)) >= 0)
	{
		if(rc > 0)
		{
			m_pServer->m_pLog->Add(1, "Se recibio un mensaje de %i bytes", rc);
			/* proceso el mensaje que llegó, en realidad por ahora
                           para probar lo reenvio */
			/*
			sprintf(m_pOutBuffer, "[%s] -> ", fn);
			outlen = inlen;
			memcpy(m_pOutBuffer + strlen(m_pOutBuffer), m_pInBuffer, outlen);
			*/
			sleep(5); /* para probar los timeouts */
			if(m_pServer->Resp(NULL, 0, GME_OK) != GME_OK)
			{
				/* error al responder */
				m_pServer->m_pLog->Add(1, "ERROR al responder mensaje");
			}
		}
		/* cosas que hago siempre */


		m_pServer->m_pLog->Add(1, "... sabadaba ...", rc);


		
	}
	OnClose(0);
	return 0;
}

void OnClose(int sig)
{
	if(m_pInBuffer) free(m_pInBuffer);
	m_pServer->UnSuscribe("algun-servicio");
	delete m_pServer;
	exit(0);
}

