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

#include "gmconfig.h"

#include <gmerror.h>
#include <gmstring.h>
#include <svcstru.h>

#include <cstdio>
#include <cstdlib>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <syslog.h>

/* FALTA: guardar la configuraci�n en un archivo */

CGMConfig::CGMConfig(CGMServer* pServer, CGLog* plog)
{
  /* No es necesario pasarle el primer par�metro porque se va a
  conectar a una base ya creada */
  m_pDB = new CGMTdb(NULL, MAX_SERVERS, MAX_SERVICES, plog);
  m_pLog = plog;
  m_pDB->Open();
  m_pServer = pServer;
}

CGMConfig::~CGMConfig()
{
  m_pDB->Close();
  delete m_pDB;
}

int CGMConfig::Process( const char *funcion, char typ,
            void* in, unsigned long inlen,
            void* out, unsigned long *outlen,
            CGMServerBase::CLIENT_DATA* /*pClientData*/)
{
  vector <CGMTdb::CSrvTab> v_svr;
  vector <CGMTdb::CFcnTab> v_svc;
  int i,j;
  CGMBuffer b;
  char saf_name[33];
  char svc_name[33];
  int rc = 0;
  int idx = 0; /* secuencia para el nombre */
  const char* p_saf_name;
  ST_SQUEUE p_st_queue;
  char svc_typ;

  *outlen = 0;
  if( !strcmp(funcion, ".suscribe"))
  {
    /* el tipo de servicio es el primer caracter */
    svc_typ = ((char*)in)[0];
    /* los proximos 32 son el nombre del servicio */
    sprintf(svc_name, "%.32s", &(((char*)in)[1]));
    /* limpio los nombres de espacios */
    if(strchr(svc_name, ' ')) strchr(svc_name, ' ')[0] = '\0';
    m_pLog->Add(100, "SUSCRIBE: %s", svc_name);
    do
    {
      sprintf(saf_name, "svc_%.24s_%03i", svc_name, idx);
      /* Agrego el servicio a la lista */
      rc = AddService(svc_name, svc_typ, saf_name);
    } while(rc == (GME_OK+1) && idx < 1000);
    m_pLog->Add(100, "          SAF-NAME: %s", saf_name);
    if(rc == GME_OK)
    {
      /* si se agreg� bien creo la cola de SAF y lo suscribo */
      rc = m_pServer->Notify(".create-queue", saf_name, strlen(saf_name+1));
      if(rc == GME_OK)
      {
        rc = m_pServer->Suscribe(svc_name, svc_typ);
        if(rc != GME_OK)
        {
          m_pServer->Notify(".drop-queue", saf_name, strlen(saf_name+1));
        }
      }
    }
    if(rc == GME_OK)
    {
      m_pLog->Add(100, "  --> Ok");
      /* le devuelvo el nombre de SAF para que consulte directamente a su cola */
      *outlen = strlen(saf_name);
      /* los buffers de salida siempre tienen que alocarse porque despu�s del anv�o se llama a un free() automaticamente */
      memcpy(out, saf_name, *outlen);
      /* FALTA: poner un timer para eliminar este servicio por inactividad */


    }
    else
    {
      m_pLog->Add(100, "  --> Error");
    }
    return rc;
  }
  else if( !strcmp(funcion, ".unsuscibe"))
  {
    /* el tipo de servicio es el primer caracter */
    svc_typ = ((char*)in)[0];
    /* los proximos 32 son el nombre del servicio */
    sprintf(svc_name, "%.32s", &(((char*)in)[1]));
    /* limpio los nombres de espacios */
    if(strchr(svc_name, ' ')) strchr(svc_name, ' ')[0] = '\0';
    m_pLog->Add(100, "UNSUSCRIBE: %s", svc_name);
    sprintf(saf_name, "svc_%.26s_%c", svc_name, svc_typ);
    /* borro el servicio de la lista, lo desuscribo y elimino su SAF */
    if(RmvService(svc_name, svc_typ, saf_name))
    {
      m_pServer->UnSuscribe(svc_name, svc_typ);
      m_pServer->Notify(".drop-queue", saf_name, strlen(saf_name+1));
      m_pLog->Add(100, "  --> Ok");
      return GME_OK;
    }
    m_pLog->Add(100, "  --> Error");
    return GME_SVC_DB_ERROR;
  }
  else if( !strcmp(funcion, ".check_service"))
  {
    m_pLog->Add(100, "CHECK-SERVICE: %s", "[NO IMPLEMENTADO]");

  }
  else if( !strcmp(funcion, ".get_server_list"))
  {
    m_pLog->Add(100, "GET-SERVER-LIST");
    v_svr = m_pDB->ServerList(/*string& servicio, char tipo_mensaje*/NULL, 0);
    b = "- Servers ----------------------------------------------------------------------\n";
    for(i = 0; i < (int)v_svr.size(); i++)
    {
      b.AddFormat(" %-32.32s %i ", v_svr[i].nombre.c_str(), v_svr[i].modo);
      for(j = 0; j < (int)v_svr[i].cola.size(); j++)
      {
        b.AddFormat("0x%08X ", v_svr[i].cola[j]);
      }
      b += "\n";
    }
    b += "--------------------------------------------------------------------------------\n";
    b.AddFormat("  Servers: %i / %i\n", i, MAX_SERVERS);
    b += "--------------------------------------------------------------------------------\n";
    b += "\n";
    *outlen = b.Length();
    memcpy(out, b.C_Str(), *outlen);
    m_pLog->Add(100, "  %i servers, tamano mensaje %lu bytes", i, *outlen);
    return GME_OK;
  }
  else if( !strcmp(funcion, ".get_service_list"))
  {
    m_pLog->Add(100, "GET-SERVICE-LIST");
    v_svc = m_pDB->ServicioList(/*string& server*/NULL);
    b = "- Servicios --------------------------------------------------------------------\n";
    for(i = 0; i < (int)v_svc.size(); i++)
    {
      b.AddFormat(" %-32.32s %c %s\n", v_svc[i].nombre.c_str(), v_svc[i].tipo_mensaje, v_svc[i].server.c_str());
    }
    b += "--------------------------------------------------------------------------------\n";
    b.AddFormat("  Servicios: %i / %i\n", i, MAX_SERVICES);
    b += "--------------------------------------------------------------------------------\n";
    b += "\n";
    *outlen = b.Length();
    memcpy(out, b.C_Str(), *outlen);
    m_pLog->Add(100, "  %i servicios, tamano mensaje %lu bytes", i, *outlen);
    return GME_OK;
  }
  /* si hay un servicio suscripto en forma remota se guarda el mensaje en su SAF */
  else if((p_saf_name = GetSafName(funcion, typ)) != NULL)
  {
    if(inlen > 0)
    {
      //p_st_queue = (ST_SQUEUE*)calloc(sizeof(ST_SQUEUE) + inlen, sizeof(char));
      strcpy(p_st_queue.head.saf_name, p_saf_name);
      p_st_queue.head.len = inlen;
      memcpy(&p_st_queue.data[0], in, inlen);
      rc = m_pServer->Notify(".enqueue", &p_st_queue, sizeof(ST_SQUEUE::head) + inlen);
      //free(p_st_queue);
    }
    return rc;
  }
  m_pLog->Add(50, "Servicio desconocido: %s", funcion);
  return GME_SVC_UNRESOLVED;
}

int CGMConfig::AddService(const char* svc, char typ, const char* saf)
{
  int i;
  int vlen;
  SERVICE_REL svc_rel;

  if((vlen = m_vServiceRel.size()) > 0)
  {
    for(i = 0; i < vlen; i++)
    {
      if( strncmp(m_vServiceRel[i].svc, svc, 32) &&
          strncmp(m_vServiceRel[i].saf, saf, 32) &&
                  m_vServiceRel[i].typ == typ     )

      {
        /* si la relaci�n ya existe ... */
        return (GME_OK+1);
      }
      if( strncmp(m_vServiceRel[i].svc, svc, 32)) return (GME_OK-1);
      if( strncmp(m_vServiceRel[i].saf, saf, 32)) return (GME_OK-2);
    }
  }
  strncpy(svc_rel.svc, svc, 32);
  strncpy(svc_rel.saf, saf, 32);
  svc_rel.typ = typ;
  m_vServiceRel.push_back(svc_rel);
  return GME_OK;
}

int CGMConfig::RmvService(const char* svc, char typ, const char* saf)
{
  int i;
  int vlen;

  if((vlen = m_vServiceRel.size()) > 0)
  {
    for(i = 0; i < vlen; i++)
    {
      if( strncmp(m_vServiceRel[i].svc, svc, 32) &&
          strncmp(m_vServiceRel[i].saf, saf, 32) &&
                  m_vServiceRel[i].typ == typ     )
      {
        m_vServiceRel.erase(m_vServiceRel.begin() + i);
        return 1;
      }
    }
  }
  return 0;
}

const char* CGMConfig::GetSafName(const char* svc, char typ)
{
  int i;
  int vlen;

  if((vlen = m_vServiceRel.size()) > 0)
  {
    for(i = 0; i < vlen; i++)
    {
      if( strncmp(m_vServiceRel[i].svc, svc, 32) &&
                  m_vServiceRel[i].typ == typ     )
      {
        return (const char*)&m_vServiceRel[i].saf[0];
      }
    }
  }
  return NULL;
}
