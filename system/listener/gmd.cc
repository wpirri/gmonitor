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

#include "gmerror.h"
#include "ctcp.h"
#include "gmbuffer.h"
#include "cmsg.h"
#include "glog.h"
#include "gmcomm.h"
#include "gmessage.h"
#include "msgtype.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <csignal>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <syslog.h>

#define PID_FILE  "/var/run/Gnu-Monitor_gmd.pid"

CTcp* pListenSocket;
int verbose;

void OnClose(int sig);
void OnChildExit(int sig);
void SocketError(int sig);
int MessageProc(CTcp* pclient, CMsg* pmsg);
void LogMessage(const char* label, CGMessage* msg);

int main(int argc, char** argv)
{
  int i;
  int pid;
  int help = 0;
  int daemon = 0;
  int tokill = 0;
  int mode_listener = 0;
  int mode_inetd = 0;
  int mode_xinetd = 0;
  int listen_port = 5533;
  int wait_to_listen = 1;
  CTcp* pClientSocket;
  CMsg* pMsg;
  FILE* f_pid;

  pListenSocket = NULL;
  verbose = 0;
  /* inicio la conexi�n con el log del sistema */
  openlog("gmd", LOG_CONS|LOG_PID, LOG_USER);

  /* se empieza a leer del parametro 0 porque inetd manda aca
  el primer argumento, no se si es un bug pero asi se safa */
  for(i = 1; i < argc; i++)
  {
    if(!strcmp("-d", argv[i]))
    {
      daemon = 1;
    }
    else if(!strcmp("-h", argv[i]))
    {
      help = 1;
    }
    else if(!strcmp("-k", argv[i]))
    {
      tokill = 1;
    }
    else if(!strcmp("-ml", argv[i]) ||
      !strcmp("--mode=listener" , argv[i]))
    {
      mode_listener = 1;
    }
    else if(!strcmp("-mi", argv[i]) ||
      !strcmp("--mode=inetd" , argv[i]))
    {
      mode_inetd = 1;
    }
    else if(!strcmp("-mx", argv[i]) ||
      !strcmp("--mode=xinetd" , argv[i]))
    {
      mode_xinetd = 1;
    }
    else if(!strcmp("-m", argv[i]) || !strcmp("--listen_port" , argv[i]))
    {
      ++i;
      listen_port = atoi(argv[i]);
    }
    else if(!strcmp("-v", argv[i]))
    {
      verbose = 1;
    }
    else help = 1;
  }
  /* ayuda */
  if(!tokill && !help &&
    ((mode_listener + mode_inetd + mode_xinetd) != 1))
  {
    /* controlo que si no entro para help o para matar una
    corrida anterior debo tener uno y solo un modo seleccionado */
    help = 1;
  }
  if(help)
  {
    fprintf(stderr, "GNU-Monitor - M�dulo Listener.\n");
    fprintf(stderr, "Use %s [options] listen_mode\n", argv[0]);
    fprintf(stderr, "  Listen modes:\n");
    fprintf(stderr, "    -ml|--mode=listener: Listener standalone.\n");
    fprintf(stderr, "    -mi|--mode=inetd:    Operando a travez de inetd.\n");
    fprintf(stderr, "    -mx|--mode=xintd:    Operando a travex de xinetd.\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -d:  En modo listener levanta como demonio.\n");
    fprintf(stderr, "    -k:  Baja un demonio de listener y sale.\n");
    fprintf(stderr, "    -h:  Muestra esta ayuda y sale.\n");
    fprintf(stderr, "    -l:  En modo listener permite cambiar el port de escucha (default 5533).\n");
    fprintf(stderr, "    -v:  Muestra informacion de DEBUG.\n");
    return 1;
  }
  if(verbose) syslog(LOG_DEBUG, "Fin parseo de opciones");
  /* si no se seleccion� el listener propietario apago el demonio */
  if(!mode_listener) daemon = 0;
  /* me fijo si voy a necesitar el pidfile */
  if(daemon || tokill)
  {
    if(verbose) syslog(LOG_DEBUG, "Matando corrida anterior");
    /* caturo el pid de una corrida anterior */
    if((f_pid = fopen(PID_FILE, "rb")) != NULL)
    {
      fread(&pid, sizeof(int), 1, f_pid);
      fclose(f_pid);
      if(!kill(pid, SIGTERM))
      {
        /* si mate una corrida anterior espero */
        if(verbose) syslog(LOG_DEBUG, "Esperando 3s antes de arrancar");
        sleep(3);
      }
    }
    /* si era solamente matar salgo ac� */
    if(tokill)
    {
      if(verbose) syslog(LOG_DEBUG, "Fin corrida para eliminar otro proceso");
      return 0;
    }
    /* si tiene que arrancar como demonio forkeo */
    if(verbose) syslog(LOG_DEBUG, "Forkeando para generar demonio");
    if(fork() > 0)
    {
      /* el padre sale y deja a hijo en background */
      if(verbose) syslog(LOG_DEBUG, "Se cierra el padre luego de forkear");
      return 0;
    }
  }
  if(daemon)
  {
    /* guardo el pid actual */
    if(verbose) syslog(LOG_DEBUG, "Guardando PID");
    pid = getpid();
    if((f_pid = fopen(PID_FILE, "wb")) != NULL)
    {
      fwrite(&pid, sizeof(int), 1, f_pid);
      fclose(f_pid);
    }
  }
  /* ****************************************************************** */
  if(verbose) syslog(LOG_DEBUG, "Inicializando signails");

   /* Capturo las se�ales que necesito */
  signal(SIGPIPE, SIG_IGN);
  signal(SIGKILL, OnClose);
  signal(SIGTERM, OnClose);
  signal(SIGSTOP, OnClose);
  signal(SIGABRT, OnClose);
  signal(SIGQUIT, OnClose);
  signal(SIGINT, OnClose);
  signal(SIGILL, OnClose);
  signal(SIGFPE, OnClose);
  signal(SIGSEGV, OnClose);
  signal(SIGBUS, OnClose);
  signal(SIGALRM, OnClose);
  signal(SIGCHLD, OnChildExit);
  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SocketError);
 
  if(verbose) syslog(LOG_DEBUG, "Iniciando cola de mensajes");
  pMsg = new CMsg;

  if(pMsg->Open() != 0)
  {
    syslog(LOG_ERR, "ERROR: al abrir cola de mensaje");
    delete pMsg;
    return 1;
  }
  if(mode_listener)
  {
    if(verbose) syslog(LOG_DEBUG, "Iniciando listener");
    pListenSocket = new CTcp();
    do
    {
      while((pClientSocket = pListenSocket->Listen(NULL, listen_port)) != NULL)
      {
        wait_to_listen = 1;
        if(verbose) syslog(LOG_DEBUG, "Forkeando para procesar mensaje recibido");
        if(fork() == 0)
        {
          MessageProc(pClientSocket, pMsg);
          /* Al terminar el loop se va el hijo */
          exit(0);
        }
      }
      if(wait_to_listen < 60) wait_to_listen++;
      syslog(LOG_ERR, "ERROR: Red no disponible, reintentando en %i segundos", wait_to_listen);
      sleep(wait_to_listen);
    } while(1);
    delete pListenSocket;
    pListenSocket = NULL;
  }
  else if(mode_inetd || mode_xinetd)
  {
    if(verbose) syslog(LOG_DEBUG, "Procesanedo mensaje I/O");
    MessageProc(NULL, pMsg);
  }
  delete pMsg;
  return 0;
}

void SocketError(int sig)
{
  signal(SIGPIPE, SocketError);
  syslog(LOG_ERR, "ERROR en socket, signal %i", sig);
}

void OnClose(int sig)
{
  if(pListenSocket)
  {
    pListenSocket->Close();
  }
  exit(0);
}

void OnChildExit(int sig)
{
  int st;

  if(verbose) syslog(LOG_DEBUG, "child exit, signal %i", sig);
  signal(SIGCHLD, OnChildExit);
  wait(&st);
}

int MessageProc(CTcp* s, CMsg* pmsg)
{
  int rc;
  CGMessage in;
  CGMessage out;
  CGMBuffer ibuff;
  CGMBuffer obuff;
  CGMComm *comm;

  comm = new CGMComm(s);

  alarm(10);
  if(verbose) syslog(LOG_DEBUG, "Inicio de procesamiento de mensaje");
  do
  {
    if(verbose) syslog(LOG_DEBUG, "Esperando header del mensaje...");
    /* leo solamente el header para determinar el tama�o de los datos */
    if((rc = in.SetHeader( comm->Read(in.GetHeaderLen(),500) )) < 0)
    {
      if(verbose) syslog(LOG_DEBUG, "ERROR: %i al leer header", rc);
      break;
    }
    if(verbose) syslog(LOG_DEBUG, "Se recibi� un header v�lido, esperando datos del mensaje...");
    /* leo los datos */
    if(in.TamMensaje() > 0)
    {
      if((rc = in.SetData( comm->Read(in.TamMensaje(),500) )) < 0)
      {
        syslog(LOG_ERR, "ERROR: %i al leer datos de entrada", rc);
        break;
      }
    }
    if(verbose) syslog(LOG_DEBUG, "Se recibieron datos v�lidos");
    LogMessage("IN", &in);
    if(verbose) syslog(LOG_DEBUG, "Enviando a la cola del router y esperando respuesta...");
    /* Hago la consulta a la cola del router */
    ibuff.Set(in.GetMsg(), in.GetMsgLen());
    if(pmsg->Query(pmsg->GetRemoteKey(BIN_MONITOR), &ibuff, &obuff, 3000) > 0)
    {
      out.SetMsg(&obuff);
    }
    else
    {
      syslog(LOG_ERR, "ERROR: al recibir respuesta del router");
      out.SetResponse(&in);
      out.CodigoRetorno(GME_MSGQ_ERROR);
      out.OrigenRespuesta(GM_ORIG_LISTEN);
      out.IdDestino(getpid());
    }
    if(verbose) syslog(LOG_DEBUG, "Se recibi� una respuesta, devolviendo datos al cliente...");
    LogMessage("OUT", &out);
    /* devuelvo el resultado */
    if(out.GetMsgLen() > GM_COMM_MSG_LEN)
    {
      syslog(LOG_ERR, "ERROR: Mensaje de respuesta mayor al soportado.");
      break;
    }
    if((rc = comm->Write(out.GetMsg(), out.GetMsgLen())) < 0)
    {
      syslog(LOG_ERR, "ERROR: %i al escribir datos de respuesta", rc);
    }
    /*break;*/

    /* se debe mantener el loop para permitor los mensajes interactivos */
    if(out.CodigoRetorno() != GME_MORE_DATA) break;
    if(verbose) syslog(LOG_DEBUG, "Mensaje con continuaci?n...");

  } while(1);

  if(verbose) syslog(LOG_DEBUG, "Fin de procesamiento de mensaje");
  delete comm;

  return 0;
}

void LogMessage(const char* label, CGMessage* msg)
{
  char strTmp[60]; /* para mostrar hasta 60 caracteres del mensaje */
  int i;

  if( !verbose) return;

  syslog(LOG_DEBUG, "== %-3.3s ==============================================================", label);
  syslog(LOG_DEBUG, "(%c)%28.28s %05lu Id: Tr/%05u Md/%05u Or/%05u Ds/%05u Orig: Co/%c Re/%c Rc: %03u",
              msg->TipoMensaje(),
              msg->Funcion(),
              msg->GetDataLen(),
              msg->IdTrans(), msg->IdMoreData(), msg->IdOrigen(), msg->IdDestino(),
              msg->OrigenConsulta(), msg->OrigenRespuesta(),
              msg->CodigoRetorno());
  syslog(LOG_DEBUG, "  Tam. Total:     [%lu]", msg->GetMsgLen());
  syslog(LOG_DEBUG, "  Tam. Header:    [%u]",  msg->GetHeaderLen());
  syslog(LOG_DEBUG, "  Tam. Datos:     [%lu]", msg->GetDataLen());
  syslog(LOG_DEBUG, "  Tipo Mensaje:   [%c]",  msg->TipoMensaje());
  syslog(LOG_DEBUG, "  Version Header: [%u]",  msg->VersionHeader());
  syslog(LOG_DEBUG, "  Id Usuario:     [%s]",  msg->IdUsuario());
  syslog(LOG_DEBUG, "  Id Cliente:     [%s]",  msg->IdCliente());
  syslog(LOG_DEBUG, "  Key:            [%s]",  msg->Key());
  syslog(LOG_DEBUG, "  Id Grupo:       [%s]",  msg->IdGrupo());
  syslog(LOG_DEBUG, "  Funcion:        [%s]",  msg->Funcion());
  syslog(LOG_DEBUG, "  IdTrans:        [%u]",  msg->IdTrans());
  syslog(LOG_DEBUG, "  IdCola:         [%i]",  msg->IdCola());
  syslog(LOG_DEBUG, "  IdMoreData:     [%u]",  msg->IdMoreData());
  syslog(LOG_DEBUG, "  Sec. Cons:      [%u]",  msg->SecuenciaConsulta());
  syslog(LOG_DEBUG, "  Sec. Resp:      [%u]",  msg->SecuenciaRespuesta());
  syslog(LOG_DEBUG, "  Orig. Cons.     [%c]",  msg->OrigenConsulta());
  syslog(LOG_DEBUG, "  Orig. Resp.     [%c]",  msg->OrigenRespuesta());
  syslog(LOG_DEBUG, "  Id Origen       [%u]",  msg->IdOrigen());
  syslog(LOG_DEBUG, "  Id Router       [%u]",  msg->IdRouter());
  syslog(LOG_DEBUG, "  Id Destino      [%u]",  msg->IdDestino());
  syslog(LOG_DEBUG, "  TimeStamp:      [%lu]", msg->TimeStamp());
  syslog(LOG_DEBUG, "  IndiceMensaje:  [%lu]", msg->IndiceMensaje());
  syslog(LOG_DEBUG, "  TamMaxMensaje:  [%lu]", msg->TamMaxMensaje());
  syslog(LOG_DEBUG, "  TamTotMensaje:  [%lu]", msg->TamTotMensaje());
  syslog(LOG_DEBUG, "  CodigoRetorno:  [%u]",  msg->CodigoRetorno());

  if(msg->GetDataLen() > 0)
  {
    syslog(LOG_DEBUG, "  CRC Datos:      [%s]", msg->Crc());
    syslog(LOG_DEBUG, "===============================================================================");
    memcpy(strTmp, msg->GetData(), 60);
    for(i = 0; i < 60 && i < (int)msg->GetDataLen(); i++)
    {
      if(strTmp[i] < ' ') strTmp[i]= '.';
      else if(strTmp[i] > 'z') strTmp[i]= '?';
    }
    i--;
    syslog(LOG_DEBUG, "  Datos:          [%-*.*s]", i, i, strTmp);
  }
  syslog(LOG_DEBUG, "===============================================================================");
}
