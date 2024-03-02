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

#include "gmsbuffer.h"

#include <gmerror.h>
#include <gmstring.h>

#include <cstdio>
#include <cstdlib>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include <svcstru.h>

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif /* min */

#define GMBUFFER_MAX_ID 8192

CGMSBuffer::CGMSBuffer(CGMServer* pServer, CGLog* plog)
{
  /* No es necesario pasarle el primer par�metro porque se va a
  conectar a una base ya creada */
  m_pDB = new CGMTdb("", MAX_SERVERS, MAX_SERVICES, plog);
  m_pDB->Open();
  m_buffer_list.clear();
  m_last_id = 0;
  m_pLog = plog;
  m_pServer = pServer;
}

CGMSBuffer::~CGMSBuffer()
{
  /* la lista la tengo que liberar prolijo de a uno para
       liberar la memoria asignada en los buffers */
  while(!m_buffer_list.empty())
  {
    if(m_buffer_list[m_buffer_list.size()-1].buffer)
    {
      delete m_buffer_list[m_buffer_list.size()-1].buffer;
    }
    m_buffer_list.erase(m_buffer_list.end());
  }
  m_pDB->Close();
  delete m_pDB;
}

int CGMSBuffer::Process(const char *funcion,
      void* in, unsigned long inlen,
      void* out, unsigned long *outlen,
      CGMServerBase::CLIENT_DATA* pClientData)
{
  int i;
  ST_BUFFER stb;
  ST_SBUFFER* sbfi = ((ST_SBUFFER*)in);
  ST_SBUFFER* sbfo;
  char buffer[1024];
  CGMServerBase::GMIOS gmio;
  ST_STIMER* pt = (ST_STIMER*)buffer;
  int rc;

  m_pLog->Add(100, "CGMSBuffer::Process(%s, %-16.16s..., %lu)", funcion, in, inlen);

  *outlen = 0;
  //*out = NULL;
  if(inlen < sizeof(ST_SBUFFER))
  {
    m_pLog->Add(10, "ERROR: no hay datos sufifientes de entrada");
    return GME_UNDEFINED;
  }
  if( !strcmp(funcion, ".new_buffer"))
  {
    m_pLog->Add(100, ".new_buffer()");
    if( !sbfi->new_buffer.len)
    {
      m_pLog->Add(10, "ERROR: Buffer con tamano 0");
      return GME_UNDEFINED;
    }
    if((stb.id_buffer = GetNewId()) == 0)
    {
      /* no hay mas id's disponibles */
      m_pLog->Add(10, "ERROR: No hay ID's disponibles");
      return GME_UNDEFINED;
    }
    stb.id_trans = pClientData->m_trans;
    stb.user = pClientData->m_user;
    stb.client = pClientData->m_client;
    stb.key = pClientData->m_key;
    stb.group = pClientData->m_group;
    /* se debe levantar un timer con este buffer para borrarlo por time-out */
    strcpy(pt->set_timer.servicio, ".del_buffer");
    pt->set_timer.modo_servicio = GM_MSG_TYPE_NOT;
    pt->set_timer.delay = 60; /* por ahora le pongo un limite de 60 seg */
    pt->set_timer.tipo_timer = 'U';
    pt->set_timer.len = sizeof(ST_SBUFFER);
    memcpy(&pt->set_timer.data[0], sbfi, sizeof(ST_SBUFFER));
    m_pLog->Add(100, "Se levanta un timer para .del_buffer");
    rc = m_pServer->Call(".set_timer",
          pt, sizeof(ST_STIMER) + sizeof(ST_SBUFFER), &gmio, 3000);
    if(rc != GME_OK)
    {
      /* no hay mas id's disponibles */
      m_pLog->Add(10, "ERROR: Error %i al setear el timer para .del_buffer", rc);
      return GME_UNDEFINED;
    }
    pt = (ST_STIMER*)gmio.data;
    /* me guardo tambien el ID del timer */
    stb.id_timer = pt->set_timer.id;
    //free(gmio.data);
    m_pLog->Add(100, "  Nuevo ID  %u", stb.id_buffer);
    m_pLog->Add(100, "    trans   %u", stb.id_trans);
    m_pLog->Add(100, "    timer   %u", stb.id_timer);
    m_pLog->Add(100, "    user   [%s]", stb.user.c_str());
    m_pLog->Add(100, "    client [%s]", stb.client.c_str());
    m_pLog->Add(100, "    key    [%s]", stb.key.c_str());
    m_pLog->Add(100, "    group  [%s]", stb.group.c_str());
    m_pLog->Add(100, "    len     %u", sbfi->new_buffer.len);
    stb.buffer = new CGMBuffer( &sbfi->new_buffer.data[0], sbfi->new_buffer.len );
    m_buffer_list.push_back(stb);
    m_pLog->Add(100, "    data   [%-*.*s]",
        sbfi->new_buffer.len,
        sbfi->new_buffer.len,
        stb.buffer->C_Str());
    /* Aloco memoria y paso los datos */
    *outlen = sizeof(ST_SBUFFER);
    //*out = (char*)calloc(*outlen, sizeof(char));
    sbfo = (ST_SBUFFER*)out;
    sbfo->new_buffer.id = stb.id_buffer;

    m_pLog->Add(100, "OK: .new_buffer()");
    return GME_OK;
  }
  else if( !strcmp(funcion, ".add_buffer"))
  {
    m_pLog->Add(100, ".add_buffer()");
    if( !sbfi->new_buffer.len)
    {
      m_pLog->Add(10, "ERROR: Buffer con tamano 0");
      return GME_UNDEFINED;
    }
    if( (int)m_buffer_list.size() > 0 )
    {
      for(i = 0; i < (int)m_buffer_list.size(); i++)
      {
        if(m_buffer_list[i].id_buffer == sbfi->add_buffer.id) break;
      }
      /* si no encontr� id en vector */
      if(i == (int)m_buffer_list.size())
      {
        m_pLog->Add(10, "ERROR: El buffer ID %u no existe",
              sbfi->add_buffer.id);
        return GME_UNDEFINED;
      }
      /* verifico la validez del cliente */
      /* este control de va a hacer cuando est� terminado el router
      if( m_buffer_list[i].id_trans != pClientData->m_trans ||
        m_buffer_list[i].user != pClientData->m_user ||
        m_buffer_list[i].client != pClientData->m_client ||
        m_buffer_list[i].key != pClientData->m_key ||
        m_buffer_list[i].group != pClientData->m_group)
      {
        return GME_UNDEFINED;
      }
      */
      m_buffer_list[i].buffer->Add(sbfi->add_buffer.data, sbfi->add_buffer.len);
    }
    else
    {
      m_pLog->Add(10, "ERROR: No hay buffers almacenados");
      return GME_UNDEFINED;
    }
    m_pLog->Add(100, "OK: .add_buffer()");
    return GME_OK;
  }
  else if( !strcmp(funcion, ".del_buffer"))
  {
    m_pLog->Add(100, ".del_buffer()");
    if(DeleteId(sbfi->del_buffer.id, pClientData) != 0) return GME_UNDEFINED;
    m_pLog->Add(100, "OK: .del_buffer()");
    return GME_OK;
  }
  else if( !strcmp(funcion, ".get_buffer"))
  {
    m_pLog->Add(100, ".get_buffer()");
    if( (int)m_buffer_list.size() > 0 )
    {
      for(i = 0; i < (int)m_buffer_list.size(); i++)
      {
        if(m_buffer_list[i].id_buffer == sbfi->get_buffer.id) break;
      }
      /* si no encontr� id en vector */
      if(i == (int)m_buffer_list.size())
      {
        m_pLog->Add(10, "ERROR: .get_buffer() - El buffer ID %u no existe",
              sbfi->get_buffer.id);
        return GME_UNDEFINED;
      }
      m_pLog->Add(100, "Buffer %u encontrado en indice %i", sbfi->get_buffer.id, i);
      /* verifico la validez del cliente */
      /* este control de va a hacer cuando est� terminado el router
      if( m_buffer_list[i].id_trans != pClientData->m_trans ||
        m_buffer_list[i].user != pClientData->m_user ||
        m_buffer_list[i].client != pClientData->m_client ||
        m_buffer_list[i].key != pClientData->m_key ||
        m_buffer_list[i].group != pClientData->m_group)
      {
        return GME_UNDEFINED;
      }
      */
      m_pLog->Add(100, "  buffer->Length(): %lu", m_buffer_list[i].buffer->Length());
      if( !m_buffer_list[i].buffer->Length())
      {
        m_pLog->Add(10, "ERROR: El buffer %u esta vacio", sbfi->get_buffer.id);
        return GME_UNDEFINED;
      }
      if(sbfi->get_buffer.offset >= m_buffer_list[i].buffer->Length())
      {
        m_pLog->Add(10, "ERROR: Offset fuera de rango");
        return GME_UNDEFINED;
      }
      /* Aloco memoria y paso los datos */
      *outlen = min( (m_buffer_list[i].buffer->Length() - sbfi->get_buffer.offset),
          (sbfi->get_buffer.maxlen) );
      *outlen += sizeof(ST_SBUFFER);
      //*out = (char*)calloc(*outlen, sizeof(char));
      sbfo = (ST_SBUFFER*)out;
      sbfo->get_buffer.id = sbfi->del_buffer.id;
      sbfo->get_buffer.offset = sbfi->get_buffer.offset;
      sbfo->get_buffer.maxlen = sbfi->get_buffer.maxlen;
      sbfo->get_buffer.totlen = m_buffer_list[i].buffer->Length();
      sbfo->get_buffer.len = *outlen - sizeof(ST_SBUFFER);
      m_pLog->Add(100, "  Devolviendo ID  %u", sbfo->get_buffer.id);
      m_pLog->Add(100, "    offset  %u", sbfo->get_buffer.offset);
      m_pLog->Add(100, "    maxlen  %u", sbfo->get_buffer.maxlen);
      m_pLog->Add(100, "    len     %u", sbfo->get_buffer.len);
      m_pLog->Add(100, "    tot len %u", sbfo->get_buffer.totlen);
      memcpy(&sbfo->get_buffer.data[0],
        m_buffer_list[i].buffer->At(sbfi->get_buffer.offset),
        sbfo->get_buffer.len);
      m_pLog->Add(100, "    data    [%-*.*s]", sbfo->get_buffer.len,
          sbfo->get_buffer.len,
          &sbfo->get_buffer.data[0]);
      m_pLog->Add(100, "OK: .get_buffer()");
      return GME_OK;
    }
    else
    {
      m_pLog->Add(10, "ERROR: No hay buffers almacenados");
      return GME_UNDEFINED;
    }
  }
  else if( !strcmp(funcion, ".committrans") || !strcmp(funcion, ".aborttrans"))
  {



    return GME_OK;
  }
  m_pLog->Add(10, "[%s] funcion no definida", funcion);
  return GME_SVC_UNRESOLVED;
}

unsigned int CGMSBuffer::GetNewId()
{
  int i, j;
  unsigned int new_id;

  /* devuelvo el primer id disponible */
  if(m_buffer_list.size() > 0)
  {
    for(i = 1; i < GMBUFFER_MAX_ID; i++)
    {
      new_id = m_last_id + i;
      if(new_id > GMBUFFER_MAX_ID)
      {
        new_id -= GMBUFFER_MAX_ID;
      }
      for(j = 0; j < (int)m_buffer_list.size(); j++)
      {
        if( m_buffer_list[j].id_buffer == new_id ) break;
      }
      /* si no encontr� id en vector */
      if(j == (int)m_buffer_list.size())
      {
        m_last_id = new_id;
        break;
      }
    }
    if(i == GMBUFFER_MAX_ID)
    {
      m_last_id = 0;
    }
  }
  else
  {
    m_last_id = 1;
  }
  return m_last_id;
}

int CGMSBuffer::DeleteId(unsigned int id, CGMServerBase::CLIENT_DATA* /*pClientData*/)
{
  int i;
  ST_STIMER timer;

  if( (int)m_buffer_list.size() > 0 )
  {
    for(i = 0; i < (int)m_buffer_list.size(); i++)
    {
      if(m_buffer_list[i].id_buffer == id) break;
    }
    /* si no encontr� id en vector */
    if(i == (int)m_buffer_list.size()) return -1;
    /* verifico la validez del cliente */
    /* este control de va a hacer cuando est� terminado el router
    if( m_buffer_list[i].id_trans != pClientData->m_trans ||
      m_buffer_list[i].user != pClientData->m_user ||
      m_buffer_list[i].client != pClientData->m_client ||
      m_buffer_list[i].key != pClientData->m_key ||
      m_buffer_list[i].group != pClientData->m_group)
    {
      return -1;
    }
    */
    /* borro el timer del buffer */
    timer.kill_timer.id = m_buffer_list[i].id_timer;
    m_pLog->Add(100, "Eliminando timer %i", m_buffer_list[i].id_timer);
    m_pServer->Notify(".kill_timer", &timer, sizeof(ST_STIMER));
    /* lo borro, primero el buffer y luego el vector */
    if(m_buffer_list[i].buffer)
    {
      delete m_buffer_list[i].buffer;
    }
    m_buffer_list.erase(m_buffer_list.begin() + i);
    if(m_buffer_list.size() == 0) m_last_id = 0;
  }
  else
  {
    return -1;
  }
  return 0;
}

int CGMSBuffer::DeleteTrans(unsigned int trans)
{
  int i;

  if(trans == 0) return -1;
  if( (int)m_buffer_list.size() > 0 )
  {
    for(i = 0; i < (int)m_buffer_list.size(); i++)
    {
      if(m_buffer_list[i].id_trans == trans) break;
    }
    /* si no encontr� id en vector */
    if(i == (int)m_buffer_list.size()) return -1;
    /* ac� no hace falta borra el timer porque se deberia borrar por la transa */
    /* lo borro, primero el buffer y luego el vector */
    if(m_buffer_list[i].buffer)
    {
      delete m_buffer_list[i].buffer;
    }
    m_buffer_list.erase(m_buffer_list.begin() + i);
  }
  else
  {
    return -1;
  }
  return 0;
}
