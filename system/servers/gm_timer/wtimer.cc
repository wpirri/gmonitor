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

#include "wtimer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GMTIMER_MAX_ID 8192

CWTimer::CWTimer()
{
  m_vTimer.clear();
}

CWTimer::~CWTimer()
{
  while(!m_vTimer.empty())
  {
    if(m_vTimer[m_vTimer.size()-1].buffer)
    {
      delete m_vTimer[m_vTimer.size()-1].buffer;
    }
    m_vTimer.erase(m_vTimer.end());
  }
}

int CWTimer::Ok(unsigned int id)
{
  unsigned int i;

  if(id == 0) return -1;/* fuera de rango */
  for(i = 0; i < m_vTimer.size(); i++)
  {
    if(m_vTimer[i].id == id) break;
  }
  if(i == m_vTimer.size()) return -1; /* no se encontró */
  if(m_vTimer[i].restart > 0)
  {
    /* reprogramo el timer */
    m_vTimer[i].end_t += m_vTimer[i].restart;
  }
  else
  {
    /* limpio el timer */
    if(m_vTimer[i].buffer) delete m_vTimer[i].buffer;
    m_vTimer.erase(m_vTimer.begin() + i);
  }
  return 0;
}

/* Borra un timer por Id, verifica identidad del cliente */
int CWTimer::DelId(unsigned int id,
      const char* user, const char* client,
      const char* key, const char* group)
{
  unsigned int i;

  if(id == 0) return -1;/* fuera de rango */
  for(i = 0; i < m_vTimer.size(); i++)
  {
    if(m_vTimer[i].id == id) break;
  }
  if(i == m_vTimer.size()) return -1; /* no se encontró */
  /* tengo que hacer el control de identidad */


  /* limpio el timer */
  if(m_vTimer[i].buffer) delete m_vTimer[i].buffer;
  m_vTimer.erase(m_vTimer.begin() + i);
  return 0;
}

/* Borra un o varios timer por transacción */
int CWTimer::DelTrans(unsigned int trans)
{
  unsigned int i;

  if(trans == 0) return -1;
  for(i = 0; i < m_vTimer.size(); i++) /* fuera de rango */
  {
    if(m_vTimer[i].trans == trans) break;
  }
  if(i == m_vTimer.size()) return -1; /* no se encontró */
  /* limpio el timer */
  if(m_vTimer[i].buffer) delete m_vTimer[i].buffer;
  m_vTimer.erase(m_vTimer.begin() + i);
  return 0;
}

unsigned int CWTimer::At(unsigned long end, const char* servicio, char tipo,
        unsigned int trans, const char* user,
        const char* client, const char* key, const char* group,
        const void* msg, unsigned long msg_len)
{
  time_t t;
  ST_WTIMER wt;

  /* tomo la hora actual para usar como referencia */
  t = time(&t);
  /* busco un ID libre */
  if((wt.id = GetNewId()) == 0) return 0; /* no se pudo */
  /* completo la estructura antes de agregarla al vector */
  wt.start_t = t;
  wt.end_t = end;
  wt.restart = 0;
  memset(wt.servicio, 0, sizeof(wt.servicio));
  memcpy(wt.servicio,servicio,(strlen(servicio)>32)?32:strlen(servicio));
  wt.tipo = tipo;
  wt.trans = trans;
  wt.user = user;
  wt.client = client;
  wt.key = key;
  wt.group = group;
  /* si hay datos aloco memoria y los copio */
  if(msg && msg_len > 0)
  {
    wt.buffer = new CGMBuffer(msg, msg_len);
  }
  else
  {
    wt.buffer = NULL;
  }
  /* la agrego al vector */
  m_vTimer.push_back(wt);
  return wt.id;

  return 0;
}

unsigned int CWTimer::Set(unsigned int seconds, const char* servicio, char tipo,
         unsigned int trans, const char* user,
         const char* client, const char* key, const char* group,
         const void* msg, unsigned long msg_len, bool repeat)
{
  time_t t;
  ST_WTIMER wt;

  /* tomo la hora actual para usar como referencia esto puede traer problemas
     de retrasos por la diferencia de tiempo entre la petición del servicio
     y el momento en que se procesa en mensaje de la cola.
     Si se trae desde el cliente una referencia de tiempo se va a cargar con la
           diferencia entre los relojes
     Puede que la mejor opcion sea que el router o l listener pongan un
     timestamp en el header del mensaje para que los servers lo puedan usar
     como referencia ya que se tome del mismo reloj */
  t = time(&t);
  /* busco un ID libre */
  if((wt.id = GetNewId()) == 0) return 0; /* no se pudo */
  /* completo la estructura antes de agregarla al vector */
  wt.start_t = t;
  wt.end_t = t + seconds;
  if(repeat) { wt.restart = seconds; }
  else { wt.restart = 0; }
  memset(wt.servicio, 0, sizeof(wt.servicio));
  memcpy(wt.servicio,servicio,(strlen(servicio)>32)?32:strlen(servicio));
  wt.tipo = tipo;
  wt.trans = trans;
  wt.user = user;
  wt.client = client;
  wt.key = key;
  wt.group = group;
  /* si hay datos aloco memoria y los copio */
  if(msg && msg_len > 0)
  {
    wt.buffer = new CGMBuffer(msg, msg_len);
  }
  else
  {
    wt.buffer = NULL;
  }
  /* la agrego al vector */
  m_vTimer.push_back(wt);
  return wt.id;
}

int CWTimer::Check()
{
  int i;
  time_t t;

  /* si el vector está vacío salgo directamente */
  if(m_vTimer.empty()) return 0;
  /* tomo la hora actual para usar como referencia */
  t = time(&t);
  /* recorro el vector y salgo al primer vencido */
  for(i = 0; i < (int)m_vTimer.size(); i++)
  {
    /* detecto el vencido */
    if(m_vTimer[i].end_t <= t) break;
  }
  /* devuelvo el indice si encontré, sino devuelvo -1 */
  return (i >= (int)m_vTimer.size())?0:m_vTimer[i].id;
}

int CWTimer::Get(unsigned int id, char* servicio, char* tipo,
         unsigned int trans, const char* user,
         const char* client, const char* key, const char* group,
         void** msg, unsigned long* msg_len)
{
  unsigned int i;

  if(id == 0) return -1;
  for(i = 0; i < m_vTimer.size(); i++) /* fuera de rango */
  {
    if(m_vTimer[i].id == id) break;
  }
  if(i == m_vTimer.size()) return -1; /* no se encontró */
  /* paso los datos */
  if(servicio) sprintf(servicio, "%-.32s", m_vTimer[i].servicio);
  if(tipo) *tipo = m_vTimer[i].tipo;
  /*

  Hay que agregar los datos de usuario, terminal, etc...

  */
  if(msg && msg_len)
  {
    if(m_vTimer[i].buffer)
    {
      if(m_vTimer[i].buffer->Length())
      {
        *msg = (void*)m_vTimer[i].buffer->C_Str();
        *msg_len = m_vTimer[i].buffer->Length();
      }
      else
      {
        *msg = NULL;
        *msg_len = 0;
      }
    }
    else
    {
      *msg = NULL;
      *msg_len = 0;
    }
  }
  return 0;
}

/* Calcula el tiempo restante para que se venza un timer */
long CWTimer::Rest(long max_wait)
{
  long rest = -1;
  long dif;
  time_t t;
  int i;

  /* tomo la hora actual para usar como referencia */
  t = time(&t);
  /* recorro el vector buscando el menor tiempo */
  for(i = 0; i < (int)m_vTimer.size(); i++)
  {
    if( (dif = (m_vTimer[i].end_t - t) * 100) > 0 )
    { /* timer no vencido */
      if( dif < rest || rest < 0 )
      {
        rest = dif;
      }
    }
    else
    { /* timer vencido */
      /* 0 y no busco mas */
      rest = 0;
      break;
    }
  }
  if(rest < 0 ) rest = -1;
  if(max_wait > 0 && rest > max_wait) rest = max_wait;
  /* devuelvo el menor tiempo o el default si no hay timers */
  return rest;
}

unsigned int CWTimer::GetNewId()
{
  int i, j;
  unsigned int new_id;

  /* devuelvo el primer id disponible */
  if(m_vTimer.size() > 0)
  {
    for(i = 1; i < GMTIMER_MAX_ID; i++)
    {
      new_id = m_last_id + i;
      if(new_id > GMTIMER_MAX_ID)
      {
        new_id -= GMTIMER_MAX_ID;
      }
      for(j = 0; j < (int)m_vTimer.size(); j++)
      {
        if( m_vTimer[j].id == new_id ) break;
      }
      /* si no encontró id en vector */
      if(j == (int)m_vTimer.size())
      {
        m_last_id = new_id;
        break;
      }
    }
    if(i == GMTIMER_MAX_ID)
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
