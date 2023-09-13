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

#include <gmerror.h>
#include <gmontdb.h>
#include <gmbuffer.h>
#include <gmessage.h>
#include <gmstring.h>
#include <cmsg.h>
#include <glog.h>
#include <gmcomm.h>
#include <msgtype.h>
#include <svcstru.h>
//#include <gmsbase.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PID_FILE  "/var/run/Gnu-Monitor_gmt.pid"

void InitSignals();
void OnClose(int sig);
void LogSignal(int sig);
void OnChildExit(int sig);
void Reload(int sig);

int MsgRouter();
void MsgQuery(CGMessage* in, CGMessage* out);
int SelectQueue(const char* funcion, char tipo_mensaje);
void LogMessage(const char* label, CGMessage* msg);

CGLog* pLog;
CGMTdb* pConfig;
CMsg *pMsg;
CGMTdb::CSystemInfo si;


int main(int argc, char** argv)
{
  int i;
  int daemon = 0;
  int tokill = 0;
  int pid;
  int loglevel = 20;
  FILE* f_pid;
  pMsg = NULL;
  pConfig = NULL;
  pLog = NULL;

  /* se empieza a leer del parametro 0 porque inetd manda aca
  el primer argumento, no se si es un bug pero asi se safa */
  for(i = 1; i < argc; i++)
  {
    if(!strcmp("-d", argv[i]))
    {
      daemon = 1;
    }
    else if(!strcmp("-k", argv[i]))
    {
      tokill = 1;
    }
    else if(!strcmp("--debug", argv[i]))
    {
      i++;
      loglevel = atoi(argv[i]);
    }
  }
  if(daemon || tokill)
  {
    /* caturo el pid de una corrida anterior */
    if((f_pid = fopen(PID_FILE, "rb")) != NULL)
    {
      fread(&pid, sizeof(int), 1, f_pid);
      fclose(f_pid);
      if(!kill(pid, SIGTERM))
      {
        /* si mate una corrida anterior espero */
        sleep(3);
      }
    }
    /* si era solamente matar salgo ac� */
    if(tokill)
    {
      return 0;
    }
    /* si tiene que arrancar como demonio forkeo */
    if(fork() > 0)
    {
      /* el padre sale y deja a hijo en background */
      return 0;
    }
  }
  if(daemon)
  {
    /* guardo el pid actual */

    pid = getpid();
    if((f_pid = fopen(PID_FILE, "wb")) != NULL)
    {
      fwrite(&pid, sizeof(int), 1, f_pid);
      fclose(f_pid);
    }
  }
  InitSignals();
  pLog = new CGLog("gmt", LOG_FILES, loglevel);
  pLog->Add(10, "Inicio");
  /* Cre el area de configuracion */
  pConfig = new CGMTdb(MONITOR_CONFIG_PATH,
        MAX_SERVERS, MAX_SERVICES, NULL);
  if(pConfig->Create() != 0)
  {
    pLog->Add(1, "Error al cargar configuracion");
    delete pConfig;
    delete pLog;
    return -1;
  }
  
  pConfig->GetSysInfo(&si);
  if(si.log_level == 0)
  {
  	si.log_level = loglevel;
  }
  si.start_time = time(&si.start_time);
  si.load_tps = 0;
  si.max_tps = 0;
  pConfig->SetSysInfo(si);

  MsgRouter();

  return 0;
}

void InitSignals()
{
  /* Capturo las se�ales que necesito */
  signal(SIGPIPE, OnClose);
  signal(SIGILL,  OnClose);
  signal(SIGQUIT, OnClose);
  signal(SIGINT,  OnClose);
  signal(SIGTERM, OnClose);
  signal(SIGABRT, OnClose);
  signal(SIGSEGV, OnClose);
  signal(SIGHUP,  Reload);
  signal(SIGCHLD, OnChildExit);
}

void Reload(int sig)
{
  pLog->Add(1, "Actualizando configuracion de servidores");
  pConfig->Reload();
  signal(SIGHUP,  Reload);
}

void OnClose(int sig)
{
  switch(sig)
  {
  case SIGINT:
    pLog->Add(1, "Se�al SIGINT - Interrupci�n procedente del teclado");
    break;
  case SIGQUIT:
    pLog->Add(1, "Se�al SIGQUIT - Terminaci�n procedente del teclado");
    break;
  case SIGILL:
    pLog->Add(1, "Se�al SIGILL - Instrucci�n ilegal");
    break;
  case SIGABRT:
    pLog->Add(1, "Se�al SIGABRT - Se�al de aborto procedente de abort(3)");
    break;
  case SIGFPE:
    pLog->Add(1, "Se�al SIGFPE - Excepci�n de coma flotante");
    break;
  case SIGKILL:
    pLog->Add(1, "Se�al SIGKILL - Se�al de matar");
    break;
  case SIGSEGV:
    pLog->Add(1, "Se�al SIGSEGV - Referencia inv�lida a memoria");
    break;
  case SIGPIPE:
    pLog->Add(1, "Se�al SIGPIPE - Tuber�a rota: escritura sin lectores");
    break;
  case SIGALRM:
    pLog->Add(1, "Se�al SIGALRM - Se�al de alarma de alarm(2)");
    break;
  case SIGTERM:
    pLog->Add(1, "Se�al SIGTERM - Se�al de terminaci�n");
    break;
  case SIGUSR1:
    pLog->Add(1, "Se�al SIGUSR1 - Se�al definida por usuario 1");
    break;
  case SIGUSR2:
    pLog->Add(1, "Se�al SIGUSR2 - Se�al definida por usuario 2");
    break;
  case SIGTSTP:
    pLog->Add(1, "Se�al SIGTSTP - Parada escrita en la tty");
    break;
  case SIGTTIN:
    pLog->Add(1, "Se�al SIGTTIN - E. de la tty para un proc. de fondo");
    break;
  case SIGTTOU:
    pLog->Add(1, "Se�al SIGTTOU - S. a la tty para un proc. de fondo");
    break;
  default:
    pLog->Add(1, "Se�al (%i)", sig);
    break;
  }
  if(pMsg) delete pMsg;
  if(pConfig) delete pConfig;
  if(pLog) delete pLog;
  exit(0);
}

void OnChildExit(int sig)
{
  int st;
  CGMTdb::CSystemInfo tsi;

  wait(&st);
  signal(SIGCHLD, OnChildExit);

  pConfig->GetSysInfo(&tsi);
  if(tsi.log_level != si.log_level)
  {
    si.log_level = tsi.log_level;
    pLog->LogLevel(si.log_level);
  }

  while(waitpid(-1, &st, WNOHANG|WUNTRACED)>0);
}

int MsgRouter()
{
  int rc;
  int from;
  CGMessage *pinmsg;
  CGMessage *poutmsg;
  CGMessage *pin2msg;
  CGMessage *pout2msg;
  CGMBuffer *pbuffer;
  CGMTdb::CSystemInfo tsi;
  ST_SBUFFER* psbuffer;
  unsigned long psbuffer_len;

  pMsg = new CMsg;
  pinmsg = new CGMessage;
  poutmsg = new CGMessage;
  pbuffer = new CGMBuffer;

  pMsg->Open();

  /* Este loop es para que el router est� permanentemente en escucha */
  do
  {
    pbuffer->Clear();
    pLog->Add(100, "Esperando mensaje");
    if((from = pMsg->Receive(pbuffer)) < 0)
    {
      pLog->Add(10, "Error de comunicacion");
      break;
    }
    /* Forkeo para que el hijo se quede procesando el mensaje mientras
            el padre queda listo para atender otro */
    if(fork() == 0)
    {
      pLog->Add(100, "Forkeando para procesar mensaje");
      if((rc = pinmsg->SetMsg(pbuffer)) < 0)
      {
        pLog->Add(10, "Error en el mensaje recibido");
        exit(0);
      }
      pinmsg->IdRouter(getpid());
      LogMessage("IN", pinmsg);
      /* Discrmino el tratamiento para los mensajes interactivos */
      if(pinmsg->TipoMensaje() == GM_MSG_TYPE_INT)
      {
        pLog->Add(100, "Mensaje interactivo");
        /* Me fijo si es el primer requerimiento */
        if( !pinmsg->IdMoreData())
        {
          pLog->Add(100, "Inicio de mensaje interactivo");
          /* Si es el primero tiro la consulta y despu�s trato la respuesta */
          MsgQuery(pinmsg, poutmsg);
          /* No me importa si sali� OK o no, igual me fijo los datos
          a ver si tengo que recortar */
          if(poutmsg->GetDataLen() > pinmsg->TamMaxMensaje())
          {
            /* Hay que recortar */
            pLog->Add(100, "El mensaje necesita continuacion");
            /* Anoto el tama�o del mensaje completo antes de recortarlo */
            poutmsg->TamTotMensaje(poutmsg->GetDataLen());
            /* para el mensaje el servicio de buffer necesito dos contenedores nuevos */
            pin2msg = new CGMessage;
            pout2msg = new CGMessage;
            /* preparo los datos para el servicio de buffer */
            psbuffer_len = sizeof(ST_SBUFFER) + poutmsg->GetDataLen();
            psbuffer = (ST_SBUFFER*)calloc(psbuffer_len, sizeof(char));
            psbuffer->new_buffer.len = poutmsg->GetDataLen();
            memcpy(&psbuffer->new_buffer.data[0], poutmsg->GetData(),
              psbuffer->new_buffer.len);
            /* recorto el mensaje que voy a devolver al tama�o maximo */
            poutmsg->SetData(&psbuffer->new_buffer.data[0], pinmsg->TamMaxMensaje());
            /*la vuelta del query la uso para armar el mensaje del servicio de buffers*/
            pin2msg->SetMsg(poutmsg->GetMsg(), poutmsg->GetHeaderLen());
            pin2msg->Funcion(".new_buffer");
            pin2msg->TipoMensaje(GM_MSG_TYPE_CR);
            pin2msg->SecuenciaConsulta(0);
            pin2msg->SetData(psbuffer, psbuffer_len);
            /* lo borro ahora asi puedo usar la variable para la respuesta */
            free(psbuffer);
            /* nuevo query */
            LogMessage("IN", pin2msg);
            MsgQuery(pin2msg, pout2msg);
            LogMessage("OUT", pout2msg);
            /* verifico el valor de retorno */
            if((rc = pout2msg->CodigoRetorno()) == GME_OK)
            {
              /*casteo un puntero a los datos devueltos por el servicio de buffer*/
              psbuffer = (ST_SBUFFER*)pout2msg->GetData();
              /* paso el ID del buffer a la respuesta */
              poutmsg->IdMoreData(psbuffer->new_buffer.id);
              /* Retorno indicaci�n de mas datos */
              poutmsg->CodigoRetorno(GME_MORE_DATA);
            }
            else
            {
              pLog->Add(10, "ERROR %i en servicio .new_buffer", rc);
              poutmsg->SetData(NULL, 0);
              poutmsg->TamTotMensaje(0);
              poutmsg->CodigoRetorno(rc);
            }
            delete pin2msg;
            delete pout2msg;
          }
        }
        else /* interactivo - continuacion */
        {
          pLog->Add(100, "Continuacion de mensaje interactivo %u", pinmsg->IdMoreData());
          /* Si tengo valor en IdMoreData es porque estoy pidiendo mas datos */
          poutmsg->SetResponse(pinmsg);
          /* Los datos del responder en este caso son los del router */
          poutmsg->OrigenRespuesta(GM_ORIG_ROUTER);
          poutmsg->IdDestino(getpid());
          /* para el mensaje el servicio de buffer necesito dos contenedores nuevos */
          pin2msg = new CGMessage;
          pout2msg = new CGMessage;
          /* preparo los datos para el servicio de buffer */
          psbuffer_len = sizeof(ST_SBUFFER);
          psbuffer = (ST_SBUFFER*)calloc(psbuffer_len, sizeof(char));
          psbuffer->get_buffer.id = pinmsg->IdMoreData();
          psbuffer->get_buffer.offset = pinmsg->IndiceMensaje();
          psbuffer->get_buffer.maxlen = pinmsg->TamMaxMensaje();
          /* Armo la consulta al servicio de buffer */
          pin2msg->SetMsg(poutmsg->GetMsg(), poutmsg->GetHeaderLen());
          pin2msg->Funcion(".get_buffer");
          pin2msg->TipoMensaje(GM_MSG_TYPE_CR);
          pin2msg->SecuenciaConsulta(0);
          pin2msg->SetData(psbuffer, psbuffer_len);
          /* ya no lo necesito */
          free(psbuffer);
          /* nuevo query */
          LogMessage("IN", pin2msg);
          MsgQuery(pin2msg, pout2msg);
          LogMessage("OUT", pout2msg);
          if((rc = pout2msg->CodigoRetorno()) == GME_OK)
          {
            /* casteo un puntero a los datos devueltos por el servicio de buffer */
            psbuffer = (ST_SBUFFER*)pout2msg->GetData();
            poutmsg->SetData(&psbuffer->get_buffer.data[0], psbuffer->get_buffer.len);
            /* seteo los valores necesarios para pedir el resto de los pedacitos */
            poutmsg->TamTotMensaje(psbuffer->get_buffer.totlen);
            /*comparando los tama�os  me doy cuenta de que ya no necesito mas el buffer*/
            if(psbuffer->get_buffer.len < pinmsg->TamMaxMensaje())
            {
              /* si es el ultimo mensaje borro el buffer */
              psbuffer_len = sizeof(ST_SBUFFER);
              psbuffer = (ST_SBUFFER*)calloc(psbuffer_len, sizeof(char));
              psbuffer->del_buffer.id = pinmsg->IdMoreData();
              /* Armo la consulta al servicio de buffer */
              pin2msg->SetMsg(poutmsg->GetMsg(), poutmsg->GetHeaderLen());
              pin2msg->Funcion(".del_buffer");
              pin2msg->TipoMensaje(GM_MSG_TYPE_NOT);
              pin2msg->SecuenciaConsulta(0);
              pin2msg->SetData(psbuffer, psbuffer_len);
              /* ya no lo necesito */
              free(psbuffer);
              /* nuevo query */
              LogMessage("IN", pin2msg);
              MsgQuery(pin2msg, pout2msg);
              LogMessage("OUT", pout2msg);
              /* el ultimo mensaje lleva el OK */
              poutmsg->CodigoRetorno(GME_OK);
            }
            else
            {
              /* El codigo de error indica que el mensaje tiene continuacion */
              poutmsg->CodigoRetorno(GME_MORE_DATA);
            }
          }
          else
          {
            pLog->Add(10, "ERROR %i en servicio .get_buffer", rc);
            poutmsg->CodigoRetorno(rc);
          }
          delete pin2msg;
          delete pout2msg;
        }
      }
      else
      {
        if( !strcmp(pinmsg->Funcion(), ".log-level"))
        {
          pConfig->GetSysInfo(&tsi);
          tsi.log_level = subint(pinmsg->GetData(), 0, pinmsg->GetDataLen());
          pConfig->SetSysInfo(tsi);
          pLog->LogLevel(tsi.log_level);
        }
        MsgQuery(pinmsg, poutmsg);
      }
      LogMessage("OUT", poutmsg);
      pbuffer->Set(poutmsg->GetMsg(), poutmsg->GetMsgLen());
      if(pMsg->Send(from, pbuffer) != 0)
      {
        pLog->Add(10, "Error al devolver el mensaje");
      }
      /* Establesco las condiciones para que el ciclo del mensaje continue o finalice */
      if( poutmsg->TipoMensaje() != GM_MSG_TYPE_R_INT ||
        (poutmsg->TipoMensaje() == GM_MSG_TYPE_R_INT && poutmsg->CodigoRetorno() != GME_MORE_DATA))
      {
        /* limpio el buffer de moredata */


      }
      /* Una vez terminada su tar�a el hijo se va */
      exit(0);
    } /* Ac� termina el fork */
  } while(1);
  delete poutmsg;
  delete pinmsg;
  delete pMsg;
  delete pbuffer;
  return 0;
}

void MsgQuery(CGMessage* in, CGMessage* out)
{
  CMsg  *pMsg;
  CGMBuffer buff_in;
  CGMBuffer buff_out;
  /* Para almacenar el header de los mensajes que no tienen datos de respuesta */
  CGMHeader msg_header;
  vector <int> lista_colas;
  int vlen;
  int cola;
  int rc;
  int ppid = 0;

  /*pLog->Add(50, "MsgQuery(...)");*/
  /* inicializo el mensaje de respuesta */
  out->SetResponse(in);
  pMsg = new CMsg();
  if(pMsg->Open() != 0)
  {
    delete pMsg;
    out->CodigoRetorno(GME_MSGQ_ERROR);
    out->OrigenRespuesta(GM_ORIG_ROUTER);
    pLog->Add(10, "MsgQuery - Error al abrir cola de mensaje");
  }
  else
  {
    buff_in.Set(in->GetMsg(), in->GetMsgLen());
    /* procedo segun tipo de mensaje */
    switch(in->TipoMensaje())
    {
    case GM_MSG_TYPE_CR: /* consulta / respuesta */
    case GM_MSG_TYPE_INT: /* interactivo */
      /* Hay que buscar la cola menos cargada */
      if((cola = SelectQueue(in->Funcion(), in->TipoMensaje())) < 0)
      {
        pLog->Add(10, "MsgQuery - Error al buscar funcion %s tipo '%c'",
            in->Funcion(), in->TipoMensaje());
        out->CodigoRetorno(GME_SVC_NOTFOUND);
        out->OrigenRespuesta(GM_ORIG_ROUTER);
        break;
      }
      /*pLog->Add(50, "Usando cola 0x%X para %s", cola, in->Funcion());*/
      if(( rc = pMsg->Query(cola, &buff_in, &buff_out, 3000)) <= 0)
      {
        out->SetData(buff_out.Data(), buff_out.Length());
        if(rc < 0)
        {
          out->CodigoRetorno(GME_MSGQ_ERROR);
          pLog->Add(10, "MsgQuery - Error en el Query de mensaje");
        }
        else
        {
          out->CodigoRetorno(GME_MSGQ_TIMEOUT);
          pLog->Add(10, "MsgQuery - Time out en el Query de mensaje");
        }
        out->OrigenRespuesta(GM_ORIG_ROUTER);
        out->IdDestino(getpid());
      }
      else
      {
        /*pLog->Add(50, "Envio OK, Completando respuesta");*/
        if(out->SetMsg(buff_out.Data(), buff_out.Length()) != 0)
        {
          out->CodigoRetorno(GME_MSGQ_ERROR);
          out->OrigenRespuesta(GM_ORIG_ROUTER);
          out->IdDestino(getpid());
          pLog->Add(10, "MsgQuery - Error al setear mensaje devuelto");
        }
      }
      break;
    case GM_MSG_TYPE_NOT:
      /*  notificacion
          Se envía a la cola mas libre sin esperar respuesta
      */
      /* Hay que buscar la cola menos cargada */
      if((cola = SelectQueue(in->Funcion(), in->TipoMensaje())) < 0)
      {
        pLog->Add(10, "MsgQuery - Error al buscar funcion %s tipo '%c'",
            in->Funcion(), in->TipoMensaje());
        out->CodigoRetorno(GME_SVC_NOTFOUND);
        out->OrigenRespuesta(GM_ORIG_ROUTER);
        break;
      }
      /* Forkeo:
          El padre vuelve con el Ok de mensaje procesado
          El hijo se encarga de enviarlo y exit
      */
      pLog->Add(100, "MsgQuery - Fork para desacoplar respuesta");
      if(fork() == 0)
      {
        out->CodigoRetorno(GME_OK);
        out->OrigenRespuesta(GM_ORIG_ROUTER);
        out->IdDestino(getpid());
      }
      else
      {
        pLog->Add(100, "MsgQuery - Enviando %s por cola 0x%X", in->Funcion(), cola);
        pMsg->Query(cola, &buff_in, &buff_out, 3000);
        /*
        if(( rc = pMsg->Query(cola, &buff_in, &buff_out, 3000)) <= 0)
        {
          if(rc < 0)
          {
            pLog->Add(1, "MsgQuery - ERROR Enviando %s por cola 0x%X", in->Funcion(), cola);
          }
          else
          {
            pLog->Add(1, "MsgQuery - Time-Out Enviando %s por cola 0x%X", in->Funcion(), cola);
          }
        }
        */
        exit(0);
      }
      break;
    case GM_MSG_TYPE_MSG:
      /*  Evento 
          Se envía a todos los suscriptores sin esperar respuesta
      */
      /* Se envia a todas */
      /*pLog->Add(50, "Buscando %s en modo %c", in->Funcion(), in->TipoMensaje());*/
      lista_colas = pConfig->Cola(in->Funcion(), in->TipoMensaje());
      if((vlen = lista_colas.size()) == 0)
      {
        pLog->Add(10, "MsgQuery - Error al buscar funcion %s tipo '%c'",
            in->Funcion(), in->TipoMensaje());
        out->CodigoRetorno(GME_SVC_NOTFOUND);
        out->OrigenRespuesta(GM_ORIG_ROUTER);
        break;
      }
      /* Forkeo:
          El padre vuelve con el Ok de mensaje procesado
          El hijo se encarga de enviarlo y exit
      */
      pLog->Add(100, "MsgQuery - Fork para desacoplar respuesta");
      if(fork() == 0)
      {
        out->CodigoRetorno(GME_OK);
        out->OrigenRespuesta(GM_ORIG_ROUTER);
        out->IdDestino(getpid());
      }
      else
      {
        /*pLog->Add(50, "Enviando %s por %i  colas", in->Funcion(), vlen);*/
        for(cola = 0; cola < vlen; cola++)
        {
          pLog->Add(100, "MsgQuery - Enviando %s por cola 0x%X", in->Funcion(), lista_colas[cola]);
          pMsg->Query(lista_colas[cola], &buff_in, &buff_out, 3000);
          /*
          if(( rc = pMsg->Query(lista_colas[cola], &buff_in, &buff_out, 3000)) <= 0)
          {
            if(rc < 0)
            {
              pLog->Add(1, "MsgQuery - ERROR Enviando %s por cola 0x%X", in->Funcion(), lista_colas[cola]);
            }
            else
            {
              pLog->Add(1, "MsgQuery - Time-Out Enviando %s por cola 0x%X", in->Funcion(), lista_colas[cola]);
            }
          }
          */
        }
        exit(0);
      }
      break;
    default:
      pLog->Add(10, "MsgQuery - Tipo de mensaje [%c] no implementado", in->TipoMensaje());
      break;
    }
    delete pMsg;
  }

  if(ppid > 0) exit(0);

}

/*
  Selecciona la cola menos cargada de entra las que responden
  al servicio requerido en el modo requerido
*/
int SelectQueue(const char* funcion, char tipo_mensaje)
{
  vector <int> lista_colas;
  int vlen;
  int qlen;
  int i;
  int candidate;
  int candidate_qlen;
  CMsg msg;
  int qi;

  pLog->Add(50, "SelectQueue - Buscando server mas libre para %s en modo %c", funcion, tipo_mensaje);
  lista_colas = pConfig->Cola(funcion, tipo_mensaje);
  if((vlen = lista_colas.size()) == 0) return -1;
  if(vlen > 1)
  {
    /* voy rotando y busco la cola menos cargada */
    qi = getpid() % vlen;
    candidate = lista_colas[qi];
    candidate_qlen = msg.GetRemoteCount(candidate);
    pLog->Add(100, "SelectQueue - Cola candidata 0x%08X con %i msgs", lista_colas[qi], candidate_qlen);
    for(i = 1; i < vlen; i++)
    {
      /* miro la cola una sola vez */
      if(++qi >= vlen) qi = 0;
      qlen = msg.GetRemoteCount(lista_colas[qi]);
      pLog->Add(100, "SelectQueue - Cola candidata 0x%08X con %i msgs", lista_colas[qi], qlen);
      if(qlen >= 0 && qlen < candidate_qlen)
      {
        candidate = lista_colas[qi];
        candidate_qlen = qlen;
      }
    }
    pLog->Add(100, "SelectQueue - Cola seleccionada 0x%08X", candidate);
    return candidate;
  }
  else
  {
    pLog->Add(100, "SelectQueue - Hay una sola cola: 0x%08X", lista_colas[0]);
    return lista_colas[0];
  }
}

void LogMessage(const char* label, CGMessage* msg)
{
  char strTmp[60]; /* para mostrar hasta 60 caracteres del mensaje */
  int i;

  pLog->Add(100, "== %-3.3s ==============================================================", label);
  pLog->Add(20, "(%c)%-28.28s %05lu Id: Tr/%05u Md/%05u Or/%010u Ds/%010u Orig: Co/%c Re/%c Rc: %03u",
              msg->TipoMensaje(),
              msg->Funcion(),
              msg->GetDataLen(),
              msg->IdTrans(), msg->IdMoreData(), msg->IdOrigen(), msg->IdDestino(),
              msg->OrigenConsulta(), msg->OrigenRespuesta(),
              msg->CodigoRetorno());
  pLog->Add(100, "  Tam. Total:     [%lu]", msg->GetMsgLen());
  pLog->Add(100, "  Tam. Header:    [%u]",  msg->GetHeaderLen());
  pLog->Add(100, "  Tam. Datos:     [%lu]", msg->GetDataLen());
  pLog->Add(100, "  Tipo Mensaje:   [%c]",  msg->TipoMensaje());
  pLog->Add(100, "  Version Header: [%u]",  msg->VersionHeader());
  pLog->Add(100, "  Id Usuario:     [%s]",  msg->IdUsuario());
  pLog->Add(100, "  Id Cliente:     [%s]",  msg->IdCliente());
  pLog->Add(100, "  Key:            [%s]",  msg->Key());
  pLog->Add(100, "  Id Grupo:       [%s]",  msg->IdGrupo());
  pLog->Add(100, "  Funcion:        [%s]",  msg->Funcion());
  pLog->Add(100, "  IdTrans:        [%u]",  msg->IdTrans());
  pLog->Add(100, "  IdCola:         [%i]",  msg->IdCola());
  pLog->Add(100, "  IdMoreData:     [%u]",  msg->IdMoreData());
  pLog->Add(100, "  Sec. Cons:      [%u]",  msg->SecuenciaConsulta());
  pLog->Add(100, "  Sec. Resp:      [%u]",  msg->SecuenciaRespuesta());
  pLog->Add(100, "  Orig. Cons.     [%c]",  msg->OrigenConsulta());
  pLog->Add(100, "  Orig. Resp.     [%c]",  msg->OrigenRespuesta());
  pLog->Add(100, "  Id Origen       [%u]",  msg->IdOrigen());
  pLog->Add(100, "  Id Router       [%u]",  msg->IdRouter());
  pLog->Add(100, "  Id Destino      [%u]",  msg->IdDestino());
  pLog->Add(100, "  TimeStamp:      [%lu]", msg->TimeStamp());
  pLog->Add(100, "  IndiceMensaje:  [%lu]", msg->IndiceMensaje());
  pLog->Add(100, "  TamMaxMensaje:  [%lu]", msg->TamMaxMensaje());
  pLog->Add(100, "  TamTotMensaje:  [%lu]", msg->TamTotMensaje());
  pLog->Add(100, "  CodigoRetorno:  [%u]",  msg->CodigoRetorno());

  if(msg->GetDataLen() > 0)
  {
    pLog->Add(100, "  CRC Datos:      [%s]", msg->Crc());
    pLog->Add(100, "===============================================================================");
    memcpy(strTmp, msg->GetData(), 60);
    for(i = 0; i < 60 && i < (int)msg->GetDataLen(); i++)
    {
      if(strTmp[i] < ' ') strTmp[i]= '.';
      else if(strTmp[i] > 'z') strTmp[i]= '?';
    }
    i--;
    pLog->Add(100, "  Datos:          [%-*.*s]", i, i, strTmp);
  }
  pLog->Add(100, "===============================================================================");
}

