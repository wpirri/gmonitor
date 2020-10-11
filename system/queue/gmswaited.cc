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


#include "gmswaited.h"

#include "gmstring.h"

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

CGLog* pLog;

CGMServerWait::CGMServerWait()
{
	m_log_level = 20;
	m_srcMessage = -1;
	m_pMsg = NULL;
	m_pLog = NULL;
}

CGMServerWait::~CGMServerWait()
{
	Exit();
	if(m_pMsg) { delete m_pMsg; }
	if(m_pLog) { delete m_pLog; }
}

int CGMServerWait::Init(const char *server_name)
{
	m_server_name = server_name;

  m_IdUsuario = "Gnu-Monitor";
  m_IdCliente = m_server_name;
  m_Key = "****************";
  m_IdGrupo = "Server";

	m_pLog = new CGLog(server_name, LOG_FILES, m_log_level);
	pLog = m_pLog; /* asigno la variable de log para los que la agarran de extern  */
	/* traigo el path de la aplicacion que debo levantar */
	m_server_params.nombre = m_server_name;
	LoadConfig();
	if(m_pConfig->Server(m_server_params) != GME_OK)
	{
		m_pLog->Add(1, "Error al traer datos del server,"
				" verifique que el servidor esta dado de alta en la base");
		delete m_pLog;
		return -1;
	}

	m_pLog->Add(100, "Server: %s - Modo: %i - App: %s",
		m_server_params.nombre.c_str(), m_server_params.modo, m_server_params.path.c_str());
	m_pMsg = new CMsg();
	if(m_pMsg->Open(m_server_params.path.c_str()) != 0)
	{
		m_pLog->Add(1, "Error al crear cola de mensajes (controlar que exista el server)");
		delete m_pMsg;
		delete m_pLog;
		return -1;
	}
	m_pLog->Add(100, "Key: %i (%i)", m_pMsg->GetKey(), m_pMsg->GetIndex());
	/* actualizo en la base la clave de la cola de mensajes */
	/* para que el monitor sepa donde mandar lo que es para este servidor */
	if(m_pConfig->AddSrv(m_server_params.nombre, m_pMsg->GetKey(), m_pMsg->GetIndex()))
	{
		m_pLog->Add(1, "Error al registrar server");
		delete m_pMsg;
		delete m_pLog;
		return -1;
	}
  Suscribe(".log-level", GM_MSG_TYPE_MSG);
	return GME_OK;
}

int CGMServerWait::Init(string &server_name)
{
	Init(server_name.c_str());
  return GME_OK;
}

int CGMServerWait::Wait(char *fn, char *typ, void *data, unsigned long maxlen, unsigned long *datalen, long to_cs)
{
	CGMBuffer MBuffer;
	CGMessage inMessage;

	/* Por ahora solo se puede atender de a un mensaje a la vez */
	/* Asi que se guarda el id de donde hay que responder y listo */
	while((m_srcMessage = m_pMsg->Receive(&MBuffer, to_cs)) > 0)
	{
		if(inMessage.SetMsg(MBuffer.Data(), MBuffer.Length()) == 0)
		{
			MBuffer.Clear();
      m_outMessage.SetResponse(&inMessage);
      if( !strcmp(inMessage.Funcion(), ".log-level") )
      {
        MBuffer.Set(m_outMessage.GetMsg(), m_outMessage.GetMsgLen());
        m_pMsg->Send(m_srcMessage, &MBuffer);
        MBuffer.Clear();
        m_pLog->Add(1, "Cambiando nivel de logueo a %i",
                     subint(inMessage.GetData(), 0, inMessage.GetDataLen()));
        m_pLog->LogLevel(subint(inMessage.GetData(), 0, inMessage.GetDataLen()));
        continue;
      }
      /* dato no entra en el buffer */
      if(maxlen && inMessage.GetDataLen() > maxlen) return GME_WAIT_ERROR;
      /* si es un mensaje que no requiere respuesta con datos lo contesto ya */
      if( inMessage.TipoMensaje() == GM_MSG_TYPE_MSG ||
        /*inMessage.TipoMensaje() == GM_MSG_TYPE_QUE ||*/
          inMessage.TipoMensaje() == GM_MSG_TYPE_NOT  )
      {
        m_outMessage.CodigoRetorno(GME_OK);
        MBuffer.Set(m_outMessage.GetMsg(), m_outMessage.GetMsgLen());
        m_pMsg->Send(m_srcMessage, &MBuffer);
        MBuffer.Clear();
      }
      /* comienzo a pasar los datos */
			strcpy(fn, inMessage.Funcion());
      *typ = inMessage.TipoMensaje();
			if(data && maxlen)
			{
				memcpy(data, inMessage.GetData(), inMessage.GetDataLen());
				if(datalen)
				{
					*datalen = inMessage.GetDataLen();
				}
			}
			/* lleno la estructura con los datos del cliente */
			m_ClientData.m_host = "";
			m_ClientData.m_port = 0;
			m_ClientData.m_user = inMessage.IdUsuario();
			m_ClientData.m_client = inMessage.IdCliente();
			m_ClientData.m_key = inMessage.Key();
			m_ClientData.m_group = inMessage.IdGrupo();
			m_ClientData.m_trans = inMessage.IdTrans();
			m_ClientData.m_timestamp = inMessage.TimeStamp();
			m_ClientData.m_flags = 0;
			return GME_WAIT_OK;
		}
		else
		{
			return GME_WAIT_TIMEOUT; /* se recibió un buffer vacío */
		}
	}
	if(m_srcMessage == 0) return GME_WAIT_TIMEOUT;  /* time-out */
	else  return GME_WAIT_ERROR; /* error */
}

int CGMServerWait::Resp(const void *data, unsigned long datalen, int rc)
{
	CGMBuffer MBuffer;

	if(rc == GME_OK)
	{
		m_outMessage.SetData(data, datalen);
		m_outMessage.CodigoRetorno(rc);
		m_outMessage.IdTrans(m_ClientData.m_trans);
	}
	else
	{
		m_outMessage.CodigoRetorno(rc);
	}
	m_outMessage.OrigenRespuesta(GM_ORIG_SVR);
	m_outMessage.IdDestino(getpid());
	MBuffer.Set(m_outMessage.GetMsg(), m_outMessage.GetMsgLen());
	if(m_pMsg->Send(m_srcMessage, &MBuffer) != 0)
	{
		m_pLog->Add(1, "Error al responder mensaje");
	}
	MBuffer.Clear();

	return GME_OK;
}

int CGMServerWait::Exit()
{
  UnSuscribe(".log-level", GM_MSG_TYPE_MSG);
	return m_pConfig->RemoveSrv(m_server_params.nombre, m_pMsg->GetIndex());
}

unsigned int CGMServerWait::GetLogLevel()
{
	return m_pLog->LogLevel();
}

void CGMServerWait::SetLogLevel(unsigned int level)
{
	m_pLog->LogLevel(level);
}

