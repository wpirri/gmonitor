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

#include <gms.h>

#include <gmconst.h>
#include <gmontdb.h>
#include <gmbuffer.h>
#include <cmsg.h>
#include <gmisc.h>
#include <gmstring.h>
#include <gmerror.h>
#include <gmessage.h>
#include <glog.h>

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
using namespace std;

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

void InitSignals();
void OnClose(int sig);
void ServerLoop();

CMsg* pMsg;
CGLog* pLog;
/*CGMTdb* pConfig; queda adentro CGMServer*/
CGMTdb::CSrvTab server_params;
CGMServer* pServer;
int loglevel;

char server_name[17];

int main(int argc, char** argv)
{
  int i;
  int help;
  int rc;
  char* param_list[256];
  CGMTdb::CSystemInfo si;

  /* Cargo los defaults */
  pServer = NULL;
  /* inicio la conexion con el log del sistema */
  help = 0;
  loglevel = -1; /* Default log level */
  /* recorro los parametros */
  for(i = 1; i < argc; i++)
  {
    if(!strcmp("--name", argv[i]))
    {
      i++;
      strcpy(server_name, argv[i]);
    }
    else if(!strncmp("-n", argv[i], 2))
    {
      if(strlen(argv[1]) > 2)
      {
        strcpy(server_name, &(argv[i][2]));
      }
      else
      {
        i++;
        strcpy(server_name, argv[i]);
      }
    }
    else if(!strcmp("-h", argv[i])) help = 1;
    else if(!strcmp("--help", argv[i])) help = 1;
    else if(!strcmp("-d", argv[i]))
    {
      i++;
      loglevel = atoi(argv[i]);
    }
    else if(!strcmp("--debug", argv[i]))
    {
      i++;
      loglevel = atoi(argv[i]);
    }
  }
  if(help)
  {
    fprintf(stderr, "Use: %s -n name\n", argv[0]);
    exit(1);
  }

  pLog = new CGLog(server_name, LOG_FILES, 100);

  if(!strlen(server_name))
  {
    pLog->Add(1, "Parametro esperado '-name' no recibido");
    exit(1);
  }

  InitSignals();

  /* Lo paso a CGMServerBase
  pConfig = new CGMTdb(MONITOR_CONFIG_PATH, MAX_SERVERS, MAX_SERVICES);
  if(pConfig->Open() != 0)
  {
    pLog->Add(1, "Error al acceder a la configuracion");
    exit(1);
  }
  */
  /* traigo el path de la aplicacion que debo levantar */
  /* Hay que dejarlo para despues de crear el objeto CGMServer
  server_params.nombre = server_name;
  if(pConfig->Server(server_params) != GME_OK)
  {
    pLog->Add(1, "Error al traer datos del server,"
        " verifique que el servidor esta dado de alta en la base");
    delete pConfig;
    delete pLog;
    exit(1);
  }
  */
  pServer = new CGMServer();
  
  /* Paso los argumento al server */
  pServer->m_arg_cnt = argc;
  pServer->m_arg_ptr = argv;

  pServer->m_server_name = server_name;

  pServer->m_IdUsuario = "GNU-MONITOR";
  pServer->m_IdCliente = server_name;
  pServer->m_Key = "****************";
  pServer->m_IdGrupo = "SERVERS";

  pServer->LoadConfig();
  server_params.nombre = server_name;
  if(pServer->m_pConfig->Server(server_params) != GME_OK)
  {
    pLog->Add(1, "Error al traer datos del server,"
        " verifique que el servidor esta dado de alta en la base");
    delete pServer;
    delete pLog;
    exit(1);
  }
  pLog->Add(1, "Server: %s - Modo: %i - App: %s",
    server_params.nombre.c_str(),
    server_params.modo, server_params.path.c_str());
  pMsg = new CMsg();
  if(pMsg->Open(server_params.path.c_str()) != 0)
  {
    pLog->Add(1, "Error al crear cola de mensajes (controlar que exista el server %s)",
        server_params.path.c_str());
    delete pMsg;
    delete pServer;
    delete pLog;
    exit(1);
  }
  pLog->Add(1, "Key: %i (%i)", pMsg->GetKey(), pMsg->GetIndex());
  /* actualizo en la base la clave de la cola de mensajes */
  /* para que el monitor sepa donde mandar lo que es para este servidor */
  if(pServer->m_pConfig->AddSrv(server_params.nombre, getpid(), pMsg->GetKey(), pMsg->GetIndex()))
  {
    pLog->Add(1, "Error al registrar server");
    delete pMsg;
    delete pServer;
    delete pLog;
    exit(1);
  }
  if(server_params.modo == GM_MODO_ONLINE)
  {
    if((rc = pServer->Init()) != GME_OK)
    {
      pLog->Add(1, "Error %i al ejecutar CGMServer::Init", rc);
      pServer->m_pConfig->RemoveSrv(server_params.nombre, pMsg->GetIndex());
      delete pMsg;
      delete pServer;
      delete pLog;
      exit(1);
    }
    memset(param_list, 0, sizeof(param_list));
    if(pServer->m_pConfig->SrvParams(server_name, param_list) != GME_OK)
    {
      pLog->Add(1, "Error al traer parametros del server");
      pServer->m_pConfig->RemoveSrv(server_params.nombre, pMsg->GetIndex());
      delete pMsg;
      delete pServer;
      delete pLog;
      exit(1);
    }
    if((rc = pServer->Run(server_params.path.c_str(), param_list)) != GME_OK)
    {
      pLog->Add(1, "Error %i al ejecutar CGMServer::Run", rc);
      pServer->m_pConfig->RemoveSrv(server_params.nombre, pMsg->GetIndex());
      delete pMsg;
      delete pServer;
      delete pLog;
      exit(1);
    }
  }
  else if(server_params.modo == GM_MODO_OFFLINE)
  {

  }
  else if(server_params.modo == GM_MODO_STANDALONE)
  {
    /*
    pServer = new CGMServer();
    pServer->m_config_path = MONITOR_CONFIG_PATH;
    pServer->m_monitor_path = BIN_MONITOR;
    pServer->m_server_name = server_name;
    */
    if((rc = pServer->Init()) != GME_OK)
    {
      pLog->Add(1, "Error %i al ejecutar CGMServer::Init", rc);
      pServer->m_pConfig->RemoveSrv(server_params.nombre, pMsg->GetIndex());
      delete pMsg;
      delete pServer;
      delete pLog;
      exit(1);
    }
    pServer->Suscribe(".log-level", GM_MSG_TYPE_MSG);
  }

  /* Luego de la inicializacion tomo el nivel de logueo del sistema o el pasado por parametro */
  if(loglevel < 0)
  {
    pServer->m_pConfig->GetSysInfo(&si);
    loglevel = si.log_level;
  }
  pLog->LogLevel(loglevel);

  /* Arranco el loop de mensajes*/
  ServerLoop();
  OnClose(0);
  return 0; /* nunca llegaria a esta instruccion */
}

void InitSignals()
{
  /* Capturo las se�ales que necesito */
  signal(SIGALRM,   SIG_IGN);
  signal(SIGPIPE,   SIG_IGN);
  signal(SIGKILL,   OnClose);
  signal(SIGTERM,   OnClose);
  signal(SIGSTOP,   OnClose);
  signal(SIGABRT,   OnClose);
  signal(SIGQUIT,   OnClose);
  signal(SIGINT,    OnClose);
  signal(SIGILL,    OnClose);
  signal(SIGFPE,    OnClose);
  signal(SIGSEGV,   OnClose);
  signal(SIGBUS,    OnClose);
}

void OnClose(int sig)
{
  pLog->Add(50, "Exit on signal %i", sig);
  if(sig == SIGTERM)
  {
    signal(SIGTERM, OnClose);
    /* preparo la salida */
    pServer->UnSuscribe(".log-level", GM_MSG_TYPE_MSG);
    pServer->Exit();
    pServer->m_pConfig->RemoveSrv(server_params.nombre, pMsg->GetIndex());
    pLog->Add(100, "Exit Ok");
    delete pMsg;
    delete pServer;
    delete pLog;
  }
  exit(0);
}

void ServerLoop()
{
  int rc;
  long from;
  char* msg;
  unsigned long msglen;
  char* param_list[256];
  CGMBuffer MBuffer;
  CGMessage inMessage;
  CGMessage outMessage;
  CGMTdb::CFcnTab funcion_param;

  while((from = pMsg->Receive(&MBuffer, -1)) >= 0) // espera infinita  hasta recibir algo
  {
    /* descompongo el mensaje */
    if(inMessage.SetMsg(MBuffer.Data(), MBuffer.Length()) != 0)
    {
      MBuffer.Clear();
      continue;
    }
    MBuffer.Clear();

    outMessage.SetResponse(&inMessage);
    rc = 0;

    if(server_params.modo == GM_MODO_OFFLINE)
    {
      do {
        /*
        pServer = new CGMServer();
        pServer->m_config_path = MONITOR_CONFIG_PATH;
        pServer->m_monitor_path = BIN_MONITOR;
        pServer->m_server_name = server_name;
        */
        if((rc = pServer->Init()) != GME_OK)
        {
          pLog->Add(1, "Error %i al ejecutar CGMServer::Init", rc);
          /* TODO: genero un mensaje de error para devolver */


          break;
        }
        /* en el ultimo parametro va funcion_param */
        memset(param_list, 0, sizeof(param_list));
        if((rc = pServer->m_pConfig->FcnParams(server_params.path, param_list)) != GME_OK)
        {
          pLog->Add(1, "Error al traer parametros del server");
          /* TODO: genero un mensaje de error para devolver */


          break;
        }
        pLog->Add(100, "Ejecutando: [%s][%s][%s][%s]",
            server_params.path.c_str(),
            (param_list[0])?param_list[0]:"",
            (param_list[1])?param_list[1]:"",
            (param_list[2])?param_list[2]:"");
        if((rc = pServer->Run(server_params.path.c_str(), param_list)) != GME_OK)
        {
          pLog->Add(1, "Error %i al ejecutar CGMServer::Run", rc);
          /* TODO: genero un mensaje de error para devolver */


          break;
        }
        break;
      } while(1);
    }

    /* En todos los modos se ejecuta de aca en adelante */
    msglen = 0;
    msg = NULL;
    while(rc == 0) /* el loop es solo para el salto y de paso controlo que no haya erroresd e antes */
    {
      /* lleno la estructura con los datos del cliente */
      pServer->m_ClientData.m_host = "";
      pServer->m_ClientData.m_port = 0;
      pServer->m_ClientData.m_user = inMessage.IdUsuario();
      pServer->m_ClientData.m_client = inMessage.IdCliente();
      pServer->m_ClientData.m_key = inMessage.Key();
      pServer->m_ClientData.m_group = inMessage.IdGrupo();
      pServer->m_ClientData.m_trans = inMessage.IdTrans();
      pServer->m_ClientData.m_timestamp = inMessage.TimeStamp();
      pServer->m_ClientData.m_flags = 0;

      /* Capturo primero si es algun comando */
      if( !strcmp(inMessage.Funcion(), ".begintrans"))
      {
        if((rc = pServer->BeginTrans((unsigned int)*inMessage.GetData())) != GME_OK)
        {
          pLog->Add(1, "Error %i al ejecutar CGMServer::BeginTrans", rc);
          /* genero un mensaje de error para devolver */


          break;
        }
      }
      else if( !strcmp(inMessage.Funcion(), ".committrans"))
      {
        if((rc = pServer->CommitTrans((unsigned int)*inMessage.GetData())) != GME_OK)
        {
          pLog->Add(1, "Error %i al ejecutar CGMServer::CommitTrans", rc);
          /* genero un mensaje de error para devolver */


          break;
        }
      }
      else if( !strcmp(inMessage.Funcion(), ".aborttrans"))
      {
        if((rc = pServer->RollbackTrans((unsigned int)*inMessage.GetData())) != GME_OK)
        {
          pLog->Add(1, "Error %i al ejecutar CGMServer::AbortTrans", rc);
          /* genero un mensaje de error para devolver */


          break;
        }
      }
      else if( !strcmp(inMessage.Funcion(), ".log-level"))
      {
        pLog->LogLevel(subint(inMessage.GetData(), 0, inMessage.GetDataLen()));
      }
      else /* si no es comando lo paso al main */
      {
        /* si es un mensaje que no requiere respuesta con datos lo contesto ya */
        if( inMessage.TipoMensaje() == GM_MSG_TYPE_MSG ||
          /*inMessage.TipoMensaje() == GM_MSG_TYPE_QUE ||*/
            inMessage.TipoMensaje() == GM_MSG_TYPE_NOT  )
        {
          outMessage.SetData(NULL, 0);
          outMessage.CodigoRetorno(GME_OK);
          MBuffer.Set(outMessage.GetMsg(), outMessage.GetMsgLen());
          if(pMsg->Send(from, &MBuffer) != 0)
          {
            pLog->Add(1, "Error al responder mensaje");
          }
          MBuffer.Clear();
        }
        if((rc = pServer->PreMain()) != GME_OK)
        {
          pLog->Add(50, "Error %i al ejecutar CGMServer::PreMain", rc);
          /* genero un mensaje de error para devolver */


          break;
        }
        if((rc = pServer->Main(inMessage.Funcion(), inMessage.TipoMensaje(),
            (void*)inMessage.GetData(),inMessage.GetDataLen(),
            (void**)&msg, &msglen)) != GME_OK)
        {
          pLog->Add(50, "Error %i al ejecutar CGMServer::Main", rc);
          /* genero un mensaje de error para devolver */


          break;
        }
        if((rc = pServer->PosMain()) != GME_OK)
        {
          pLog->Add(50, "Error %i al ejecutar CGMServer::PosMain", rc);
          /* genero un mensaje de error para devolver */


          break;
        }
      }
      /* no sacar este break */
      break;
    }
    if( inMessage.TipoMensaje() != GM_MSG_TYPE_MSG &&
      /*inMessage.TipoMensaje() != GM_MSG_TYPE_QUE &&*/
        inMessage.TipoMensaje() != GM_MSG_TYPE_NOT  )
    {
      if(rc == GME_OK)
      {
        outMessage.SetData(msg, msglen);
        outMessage.CodigoRetorno(rc);
      }
      else
      {
        outMessage.CodigoRetorno(rc);
      }
      outMessage.OrigenRespuesta(GM_ORIG_SVR);
      outMessage.IdDestino(getpid());
      if(msg) free(msg); /* esta linea libera la memoria que se haya asignado dentro de Main */
      MBuffer.Set(outMessage.GetMsg(), outMessage.GetMsgLen());
      if(pMsg->Send(from, &MBuffer) != 0)
      {
        pLog->Add(1, "Error al responder mensaje");
      }
      MBuffer.Clear();
    }
  }
}

