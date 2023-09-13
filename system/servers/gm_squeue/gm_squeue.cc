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

#include <gmerror.h>
#include <gmconst.h>
#include <gmbuffer.h>
#include <cmsg.h>
#include <gmisc.h>
#include <gmstring.h>
#include <glog.h>
#include <svcstru.h>

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cerrno>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <syslog.h>
#include <fcntl.h>

#include "gmsaf.h"

/*
        La variable 'void* m_gptr' es un puntero para uso generico por el server
        es el unico puntero miembro de la clase que puede ser utilizado libremente
        al realizar el programa server.
        La variable 'CGMInitData m_ClientData' se completar�con los valores del
        cliente que solicit�el servicio antes del llamado a la funci� PreMain()
        y mantendr�este valos para ser utilizado por el server si es necesario
        hasta el final del servicio.
*/

extern CGLog* pLog;

CGMSaf* pSaf;

typedef struct _squeue_trans
{
  unsigned int trans;
  vector <string> vsaf;
} squeue_trans;

vector <squeue_trans> vtrans;


CGMServer::CGMServer()
{
  pSaf = new CGMSaf();
  vtrans.clear();
}

CGMServer::~CGMServer()
{
  delete pSaf;
  vtrans.clear();
}

/* Colocar en esta funcion lo que se necesite correr al levantar el server */
int CGMServer::Init()
{
  pLog->Add(1, "Iniciando server");
  if(Suscribe(".enqueue", GM_MSG_TYPE_NOT) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .enqueue");
  if(Suscribe(".dequeue", GM_MSG_TYPE_CR) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .dequeue");
  if(Suscribe(".create-queue", GM_MSG_TYPE_NOT) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .create-queue");
  if(Suscribe(".drop-queue", GM_MSG_TYPE_NOT) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .drop-queue");
  if(Suscribe(".clear-queue", GM_MSG_TYPE_NOT) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .clear-queue");
  if(Suscribe(".clear-all-queue", GM_MSG_TYPE_NOT) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .clear-all-queue");;
  if(Suscribe(".info-queue", GM_MSG_TYPE_CR) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .info-queue");;
  if(Suscribe(".list-queue", GM_MSG_TYPE_INT) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .list-queue INT");;
  if(Suscribe(".list-queue", GM_MSG_TYPE_CR) != GME_OK)
    pLog->Add(1, "ERROR al suscribir a .list-queue CR");
  /* Inicio en modo transaccional */
  /*SetTransMode(true);  *** lo saco hasta que funcione *** */

  pSaf->Open(SAF_FILES);

  return 0;
}

/* Colocar en esta funcion lo que se necesite correr al bajar el serer */
int CGMServer::Exit()
{
  pLog->Add(1, "Terminando server");
  UnSuscribe(".list-queue", GM_MSG_TYPE_INT);
  UnSuscribe(".info-queue", GM_MSG_TYPE_CR);
  UnSuscribe(".clear-all-queue", GM_MSG_TYPE_NOT);
  UnSuscribe(".clear-queue", GM_MSG_TYPE_NOT);
  UnSuscribe(".drop-queue", GM_MSG_TYPE_NOT);
  UnSuscribe(".create-queue", GM_MSG_TYPE_NOT);
  UnSuscribe(".dequeue", GM_MSG_TYPE_CR);
  UnSuscribe(".enqueue", GM_MSG_TYPE_NOT);
  UnSuscribe(".enqueue", GM_MSG_TYPE_CR);

  pSaf->Close(false);

  return 0;
}

/* Estas rutinas son llamadas para el manejo de transaccion se debe colocar
  en ellas el c�igo necesario para cada uno de los procesos */
int CGMServer::BeginTrans(unsigned int trans)
{
  int i, vlen;
  squeue_trans sqt;

  pLog->Add(100, "INICIO DE TRANSACCION %u", trans);

  vlen = vtrans.size();
  for(i = 0; i < vlen; i++)
  {
    if( vtrans[i].trans == trans)
    {
      /* transa repetida */
      return -1;
    }
  }
  sqt.trans = trans;
  sqt.vsaf.clear();
  vtrans.push_back(sqt);
  return 0;
}

int CGMServer::CommitTrans(unsigned int trans)
{
  int i1, vlen1;
  int i2, vlen2;
  squeue_trans sqt;

  pLog->Add(100, "COMMIT DE TRANSACCION %u", trans);

  /* busco la transacci�n en el vector */
  vlen1 = vtrans.size();
  for(i1 = 0; i1 < vlen1; i1++)
  {
    if( vtrans[i1].trans == trans)
    {
      vlen2 = vtrans[i1].vsaf.size();
      for(i2 = 0; i2 < vlen2; i2++)
      {
        pSaf->CommitGet(vtrans[i1].vsaf[i2].c_str(), 0, trans);
      }
      /* limpio el vector de nombre */
      vtrans[i1].vsaf.clear();
      /* limpio el item de la transacci�n */
      vtrans.erase(vtrans.begin() + i1);
      break;
    }
  }
  return 0;
}
int CGMServer::RollbackTrans(unsigned int trans)
{
  int i1, vlen1;
  int i2, vlen2;
  squeue_trans sqt;

  pLog->Add(100, "ROLLBACK DE TRANSACCION %u", trans);

  /* busco la transacci�n en el vector */
  vlen1 = vtrans.size();
  for(i1 = 0; i1 < vlen1; i1++)
  {
    if( vtrans[i1].trans == trans)
    {
      vlen2 = vtrans[i1].vsaf.size();
      for(i2 = 0; i2 < vlen2; i2++)
      {
        pSaf->AbortGet(vtrans[i1].vsaf[i2].c_str(), 0, trans);
      }
      /* limpio el vector de nombre */
      vtrans[i1].vsaf.clear();
      /* limpio el item de la transacci�n */
      vtrans.erase(vtrans.begin() + i1);
      break;
    }
  }
  return 0;
}

/* estas rutinas se llaman antes y despu�s de la de procesamiento de mensaje */
int CGMServer::PreMain()
{
  return 0;
}
int CGMServer::PosMain()
{
  return 0;
}

/* Colocar en esta funcion el proceso que intepreta el mensaje recibido */
int CGMServer::Main(const char *funcion, char typ,
      void* in, unsigned long inlen,
      void** out, unsigned long *outlen)
{
  ST_SQUEUE *st;
  CGMSafFile::saf_info *pinfo;
  int saf_id;
  int i, vlen;
  int max_len;
  CGMBuffer buff;
  vector <CGMSafFile::saf_info> vslist;
  int rc;

  pLog->Add(100, "CGMServer::Main(%s, 0x%08X, %lu)", funcion, in, inlen);

  *outlen = 0;
  *out = NULL;
  st = (ST_SQUEUE*)in;
  if( !strcmp(funcion, ".enqueue")) /* GM_MSG_TYPE_NOT */
  {
    if((rc = pSaf->Add(st->saf_name, &st->data[0], st->len, m_ClientData.m_trans)) == GME_OK)
    {
      return GME_OK;
    }
    else
    {
      return rc;
    }
  }
  else if( !strcmp(funcion, ".dequeue")) /* GM_MSG_TYPE_CR */
  {
    /* hay que alocar espacio para el retorno m�ximo */
    if(st->len)
    {
      max_len = st->len;
    }
    else
    {
      max_len = 4096;
    }
    *out = calloc(sizeof(char), max_len);
    saf_id = pSaf->Get(st->saf_name, *out, max_len,
                       outlen, m_ClientData.m_trans);
    if(saf_id > 0)
    {
      /* si no hay una transa de por medio ya le mando el commit */
      if(!m_ClientData.m_trans)
      {
        pSaf->CommitGet(st->saf_name, saf_id);
      }
      else
      {
        /* lo agrego a la lista de transacciones */
        vlen = vtrans.size();
        for(i = 0; i < vlen; i++)
        {
          if( vtrans[i].trans == m_ClientData.m_trans)
          {
            /* con esto me aseguro que el string no se va a pasar de largo */
            st->len = 0;
            vtrans[i].vsaf.push_back(string(st->saf_name));
            break;
          }
        }
      }
      return GME_OK;
    }
    else if(saf_id < 0)
    {
      return GME_SAF_NOT_FOUND;
    }
    else
    {
      /* SAF vacío */
      return GME_NO_DATA;
    }
  }
  else if(!strcmp(funcion, ".create-queue"))
  {
    if((rc = pSaf->Create((const char*)in)) == GME_OK)
    {
      pLog->Add(10, "CREANDO SAF %s OK", (char*)in);
      return GME_OK;
    }
    else
    {
      pLog->Add(10, "ERROR %i AL CREAR SAF %s", rc, (char*)in);
      return rc;
    }
  }
  else if(!strcmp(funcion, ".drop-queue"))
  {
    if((rc = pSaf->Drop((const char*)in)) == GME_OK)
    {
      pLog->Add(10, "ELIMINANDO SAF %s OK", (char*)in);
      return GME_OK;
    }
    else
    {
      pLog->Add(10, "ERROR %i AL ELIMINAR SAF %s", rc, (char*)in);
      return rc;
    }
  }
  else if(!strcmp(funcion, ".info-queue"))
  {
    pinfo = (CGMSafFile::saf_info*)calloc(sizeof(char), sizeof(CGMSafFile::saf_info));
    if((rc = pSaf->Info((const char*)in, pinfo)) == GME_OK)
    {
      pLog->Add(10, "INFORMACION DE SAF %s OK", (char*)in);
      pLog->Add(10, "  pinfo.filename:     %s", pinfo->filename);
      pLog->Add(10, "  pinfo.name:         %s", pinfo->name);
      pLog->Add(10, "  pinfo.record_count: %lu", pinfo->record_count);
      pLog->Add(10, "  pinfo.data_count:   %lu", pinfo->data_count);

      *out = pinfo;
      *outlen = sizeof(CGMSafFile::saf_info);
      return GME_OK;
    }
    else
    {
      pLog->Add(10, "ERROR %i DE INFORMACION DE SAF %s", rc, (char*)in);
      return rc;
    }
  }
  else if(!strcmp(funcion, ".list-queue"))
  {
    vslist = pSaf->List();
    if((vlen = vslist.size()) > 0)
    {
      buff.Clear();
      /*buff.Add("<?xml version=\"1.0\" standalone=\"yes\"?>\n");*/
      buff.Add("<queues>\n");
      for(i = 0; i < vlen; i++)
      {
        buff.Add("<queue>\n");
        buff.AddFormat("<filename>%s</filename>\n", vslist[i].filename);
        buff.AddFormat("<name>%s</name>\n", vslist[i].name);
        buff.AddFormat("<record_count>%li</record_count>\n", vslist[i].record_count);
        buff.AddFormat("<data_count>%li</data_count>\n", vslist[i].data_count);
        buff.Add("</queue>\n");
      }
      buff.Add("</queues>\n");
      *outlen = buff.Length();
      *out = calloc(sizeof(char), *outlen);
      memcpy(*out, buff.C_Str(), *outlen);
    }
    return GME_OK;
  }
  else if(!strcmp(funcion, ".clear-queue"))
  {

    return GME_OK;
  }
  else if(!strcmp(funcion, ".clear-all-queue"))
  {

    return GME_OK;
  }
  return GME_SVC_UNRESOLVED;
}
