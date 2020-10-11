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

#include "gmsafile.h"

#include <string.h>

CGMSafFile::CGMSafFile()
{
  memset(&m_info, 0, sizeof(saf_info));
  m_fd = NULL;
  m_saf_index.clear();
  m_last_id = 0;
}

CGMSafFile::CGMSafFile(const char *filename, const char* name)
{
  memset(&m_info, 0, sizeof(saf_info));
  m_fd = NULL;
  /*m_saf_index = NULL;*/
  m_saf_index.clear();
  m_last_id = 0;
  Open(filename, name);
}

CGMSafFile::~CGMSafFile()
{
  Close(false);
}

int CGMSafFile::Open(const char *filename, const char* name)
{
  saf_record_header header;
  if(m_fd) return -1;

  memset(&m_info, 0, sizeof(saf_info));
  /*m_saf_index = NULL;*/
  m_saf_index.clear();
  m_last_id = 0;

  /* abro el archivo si ya existe */
  if((m_fd = fopen(filename, "r+b")) != NULL)
  {
    /* leo el header del primer registro */
    if(fread(&header, sizeof(saf_record_header), 1, m_fd) != 1)
    {
      /* no se lee ná */
      fclose(m_fd);
      m_fd = NULL;
      return -1;
    }
    /* verifico los datos del header */
    if( header.id != 0 ||
        header.record_len != sizeof(saf_record_header) ||
        header.data_len != 0 ||
        header.record_status != 'X' )
    {
      /* no es un archivo de SAF */
      fclose(m_fd);
      m_fd = NULL;
      return -1;
    }
  }
  /* el arcivo no existe, entonces lo creo */
  else if((m_fd = fopen(filename, "w+b")) != NULL)
  {
    /* Grabo el header que lo identifica */
    header.id = 0;
    header.record_len = sizeof(saf_record_header);
    header.data_len = 0;
    header.record_status = 'X';
    fwrite(&header, sizeof(saf_record_header), 1, m_fd);
  }
  /* algo no está andando bien */
  else
  {
    return -1;
  }
  /* sigo con el resto de los tramites */
  strcpy(m_info.filename, filename);
  strcpy(m_info.name, name);
  GetSafInfo();
  return 0;
}

int CGMSafFile::Close(bool drop)
{
  if( !m_fd) return -1;
  fclose(m_fd);
  m_fd = NULL;
  if(drop)
  {
    remove(m_info.filename);
  }
  m_saf_index.clear();
  return 0;
}

bool CGMSafFile::IsOpen()
{
  return (m_fd != NULL);
}

int CGMSafFile::Info(saf_info *st)
{
  if(st)
  {
    memcpy(st, &m_info, sizeof(saf_info));
    return 0;
  }
  return -1;
}

unsigned int CGMSafFile::GenId()
{
  return ++m_last_id;
}

int CGMSafFile::Add(const void* data, unsigned long len, unsigned int trans)
{
  saf_record_header header;
  long pos;
  saf_index si;
  int i, vlen;

  if( !data) return -1;
  if( !len) return -1;
  if( !m_fd) return -1;


  /* busco un registro para reciclar con espacio suficiente */
  vlen = m_saf_index.size();
  for(i = 0; i < vlen; i++)
  {
    if( m_saf_index[i].record_status == 'D' &&
        (m_saf_index[i].record_len - sizeof(saf_record_header) >= len))
    {
      break;
    }
  }
  if(i != vlen)
  {
    /* si lo encontré me posiciono y leo el header */
    fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
    if(fread(&header, sizeof(saf_record_header), 1, m_fd) != 1)
    {
      return -1;
    }
    /* salto al inicio del header */
    fseek(m_fd, sizeof(saf_record_header)*-1, SEEK_CUR);
    /* Actualizo el contador de registros (solo el de datos porque reutilizo) */
    m_info.data_count++;
  }
  else
  {
    /* no encontró ninguno, salto al final del archivo
       para crear uno nuevo a medida */
    fseek(m_fd, 0, SEEK_END);
    header.record_len = len + sizeof(saf_record_header);
    /* Actualizo los contadores de registros */
    m_info.record_count++;
    m_info.data_count++;
  }
  header.id = GenId();
  header.trans = trans;
  header.data_len = len;
  header.record_status = 'N';
  /* me gusrdo la dirección por si tengo que pisar el header  */
  pos = ftell(m_fd);
  if(fwrite( &header, sizeof(saf_record_header), 1, m_fd) != 1) return -1;
  if(fwrite( data, len, 1, m_fd) != 1)
  {
    /*FALTA: todavia no se que voy a hacer acá*/
    return -1;
  }
  /* elimino el indice reutilizado (el nuevo se agrega al final) */
  if(i != vlen)
  {
    m_saf_index.erase(m_saf_index.begin() + i);
  }
  si.id = header.id;
  si.trans = header.trans;
  si.record_len = header.record_len;
  si.record_status = header.record_status;
  si.header_pos = pos;
  m_saf_index.push_back(si);
  return 0;
}

unsigned int CGMSafFile::Get(void* data, unsigned long max_len,
          unsigned long *len, unsigned int trans)
{
  /* entrego el primer item que no hay sido entregado ya */
  saf_record_header header;
  int i, vlen;

  if( !data) return 0;
  if( !len) return 0;
  if( !m_fd) return 0;

  /* Busco el primer registro no enviado */
  vlen = m_saf_index.size();
  for(i = 0; i < vlen; i++)
  {
    if( m_saf_index[i].record_status == 'N' ||
        m_saf_index[i].record_status == 'F'  )
    {
      break;
    }
  }
  if(i == vlen) return 0; /* no encontró */
  /* me paro al inicio del header... */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  /* ...y lo leo */
  if(fread(&header, sizeof(saf_record_header), 1, m_fd) != 1)
  {
    return 0;
  }
  /* leo el dato hasta donde la variable me lo permite */
  if( (*len = fread(data, sizeof(char), min(max_len, header.data_len), m_fd)) <= 0)
  {
    return 0;
  }
  header.record_status = 'S';
  header.trans = trans;
  /* me paro al inicio del header para actualizarlo */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  fwrite(&header, sizeof(saf_record_header), 1, m_fd);
  m_saf_index[i].record_status = header.record_status;
  m_saf_index[i].trans = header.trans;
  return header.id;
}

int CGMSafFile::CommitGet(unsigned int id, unsigned int trans)
{
  saf_record_header header;
  int i, vlen;
  char filename[FILENAME_MAX];
  char name[FILENAME_MAX];

  if( !m_fd) return -1;
  if( !id && !trans) return -1;

  vlen = m_saf_index.size();
  if(id)
  { /* busco por ID */
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].id == id )
      {
        break;
      }
    }
  }
  else
  { /* busco por transacción */
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].trans == trans )
      {
        break;
      }
    }
  }
  if(i == vlen) return -1; /* no encontró */
  /* Verifico que haya sido enviado */
  if( m_saf_index[i].record_status != 'S') return -1;
  /* me paro al inicio del header... */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  /* ...y lo leo */
  if(fread(&header, sizeof(saf_record_header), 1, m_fd) != 1)
  {
    return -1;
  }
  /* actualizo el estado */
  header.record_status = 'D';
  header.trans = 0;
  /* me paro al inicio del header para actualizarlo */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  fwrite(&header, sizeof(saf_record_header), 1, m_fd);
  m_saf_index[i].record_status = header.record_status;
  m_saf_index[i].trans = header.trans;
  m_info.data_count--;
  /* Si la cantidad de registros llega a 0 aprovecho para limpiar el archivo */
  if(m_info.data_count == 0)
  {
    strcpy(filename, m_info.filename);
    strcpy(name, m_info.name);
    Close(true);
    Open(filename, name);
  }
  return 0;
}

int CGMSafFile::AbortGet(unsigned int id, unsigned int trans)
{
  saf_record_header header;
  int i, vlen;
  char filename[FILENAME_MAX];
  char name[FILENAME_MAX];

  if( !m_fd) return -1;
  if( !id && !trans) return -1;

  vlen = m_saf_index.size();
  if(id)
  { /* busco por ID */
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].id == id )
      {
        break;
      }
    }
  }
  else
  { /* busco por transacción */
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].trans == trans )
      {
        break;
      }
    }
  }
  if(i == vlen) return -1; /* no encontró */
  /* Verifico que haya sido enviado */
  if( m_saf_index[i].record_status != 'S') return -1;
  /* me paro al inicio del header... */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  /* ...y lo leo */
  if(fread(&header, sizeof(saf_record_header), 1, m_fd) != 1)
  {
    return -1;
  }
  /* actualizo el estado */
  header.record_status = 'F';
  header.trans = 0;
  /* me paro al inicio del header para actualizarlo */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  fwrite(&header, sizeof(saf_record_header), 1, m_fd);
  m_saf_index[i].record_status = header.record_status;
  m_saf_index[i].trans = header.trans;
  m_info.data_count--;
  /* Si la cantidad de registros llega a o aprovecho para limpiar el archivo */
  if(m_info.data_count == 0)
  {
    strcpy(filename, m_info.filename);
    strcpy(name, m_info.name);
    Close(true);
    Open(filename, name);
  }
  return 0;
}

int CGMSafFile::Delete(unsigned int id, unsigned int trans)
{
  saf_record_header header;
  int i, vlen;
  char filename[FILENAME_MAX];
  char name[FILENAME_MAX];

  if( !m_fd) return -1;
  if( !id && !trans) return -1;

  vlen = m_saf_index.size();
  if(id)
  { /* busco por ID */
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].id == id )
      {
        break;
      }
    }
  }
  else
  { /* busco por transacción */
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].trans == trans )
      {
        break;
      }
    }
  }
  if(i == vlen) return -1; /* no encontró */
  /* Verifico que no haya sido borrado ya */
  if(m_saf_index[i].record_status == 'D') return -1;
  /* Verifico que no sea nuevo (para borarlo debe ser enviado primero) */
  /*if(si->record_status == 'N') return -1;*/
  /* me paro al inicio del header... */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  /* ...y lo leo */
  if(fread(&header, sizeof(saf_record_header), 1, m_fd) != 1)
  {
    return -1;
  }
  /* actualizo el estado */
  header.record_status = 'D';
  header.trans = 0;
  /* me paro al inicio del header para actualizarlo */
  fseek(m_fd, m_saf_index[i].header_pos, SEEK_SET);
  fwrite(&header, sizeof(saf_record_header), 1, m_fd);
  m_saf_index[i].record_status = header.record_status;
  m_saf_index[i].trans = header.trans;
  m_info.data_count--;
  /* Si la cantidad de registros llega a 0 aprovecho para limpiar el archivo */
  if(m_info.data_count == 0)
  {
    strcpy(filename, m_info.filename);
    strcpy(name, m_info.name);
    Close(true);
    Open(filename, name);
  }
  return 0;
}

int CGMSafFile::GetSafInfo()
{
  saf_record_header header;
  long pos;
  saf_index si;
  int i, vlen;

  fseek(m_fd, 0, SEEK_SET);
  m_info.record_count = 0;
  m_info.data_count = 0;
  pos = 0;

  while(fread(&header, sizeof(saf_record_header), 1, m_fd))
  {
    m_info.record_count++;
    if(header.record_status != 'D' && header.record_status != 'X') m_info.data_count++;
    /* Actualizo el contador del generador de indices */
    if(header.id > m_last_id) m_last_id = header.id;
    /* agrego un item a la lista de forma ordenada de menor a mayor */
    si.id = header.id;
    si.trans = header.trans;
    si.record_len = header.record_len;
    si.record_status = header.record_status;
    si.header_pos = pos;
    /* le busco su lugar en el vector */
    vlen = m_saf_index.size();
    for(i = 0; i < vlen; i++)
    {
      if( m_saf_index[i].id > si.id )
      {
        break;
      }
    }
    if(i == vlen)
    {
      /* va al final */
      m_saf_index.push_back(si);
    }
    else
    {
      /* va intercalado */
      m_saf_index.insert((m_saf_index.begin() + i), si);
    }
    /* salto al proximo header */
    fseek(m_fd, header.record_len - sizeof(saf_record_header), SEEK_CUR);
    pos = ftell(m_fd);
  }
  return 0;
}

const char* CGMSafFile::Name()
{
  return (const char*) &m_info.name[0];
}

const char* CGMSafFile::FileName()
{
  return (const char*) &m_info.filename[0];
}

unsigned long CGMSafFile::GetRecordCount()
{
  return m_info.record_count;
}

unsigned long CGMSafFile::GetDataCount()
{
  return m_info.data_count;
}
