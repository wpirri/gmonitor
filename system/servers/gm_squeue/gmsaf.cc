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

#include "gmsaf.h"

#include <glog.h>
#include <gmerror.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

extern CGLog* pLog;

CGMSaf::CGMSaf()
{

}

CGMSaf::~CGMSaf()
{
  Close();
}

int CGMSaf::Open(const char *path)
{
  DIR *dir;
  struct dirent *dirlist;
  CGMSafFile* psf;
  char saf_name[FILENAME_MAX];
  char saf_file_name[FILENAME_MAX];

  pLog->Add(100, "CGMSaf::Open( %s )", (path)?path:"NULL");
  m_saf_list.clear();
  if(path)
  {
    strcpy(m_saf_path, path);
    if(m_saf_path[strlen(m_saf_path)-1] != '/')
    {
      strcat(m_saf_path, "/");
    }
  }
  else
  {
    strcpy(m_saf_path, "/");
  }
  pLog->Add(10, "SAF PATH %s", m_saf_path);
  /* si el directorio existe busco archivos de SAF creados previamente
     si no existe lo creo */
  if((dir = opendir(m_saf_path)) != NULL)
  {
    while((dirlist = readdir(dir)) != NULL)
    {
      if(strstr(dirlist->d_name, ".saf"))
      {
        strcpy(saf_name, dirlist->d_name);
        if(strstr(saf_name, ".saf")) ((char*)strstr(saf_name, ".saf"))[0] = '\0';
        sprintf(saf_file_name, "%s%s", m_saf_path, dirlist->d_name);
        psf = new CGMSafFile(saf_file_name, saf_name);
        if(psf->IsOpen())
        {
          m_saf_list.push_back(psf);
          pLog->Add(10, "ARCHIVO %s SAF [%s]", saf_file_name, saf_name);
          pLog->Add(10, "        %lu Registros", psf->GetRecordCount());
          pLog->Add(10, "        %lu Registros activos", psf->GetDataCount());
        }
        else
        {
          delete psf;
          pLog->Add(10, "ARCHIVO %s NO ES SAF", saf_file_name);
        }
      }
    }
    closedir(dir);
  }
  else
  {
    /* el directorio no existe, trato de crearlo */
    if(mkdir(m_saf_path, 0755) != 0) return GME_DIRECTORY_ERROR;
    pLog->Add(10, "SAF PATH %s CREADO", m_saf_path);
  }
  return GME_OK;
}

int CGMSaf::Close(bool drop)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Close( %s )", (drop)?"drop":"no-drop");
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if(drop)
    {
      m_saf_list[i]->Close(true);
    }
    delete m_saf_list[i];
  }
  m_saf_list.clear();
  return GME_OK;
}

int CGMSaf::Create(const char* saf_name)
{
  char saf_file_name[FILENAME_MAX];
  CGMSafFile* psf;

  pLog->Add(100, "CGMSaf::Create( %s )", (saf_name)?saf_name:"NULL");
  if(Exist(saf_name))
  {
    pLog->Add(100, "  Ya existe");
    return GME_FILE_DUPLICATED;
  }
  sprintf(saf_file_name, "%s%s.saf", m_saf_path, saf_name);
  psf = new CGMSafFile(saf_file_name, saf_name);
  if(psf->IsOpen())
  {
    m_saf_list.push_back(psf);
    return GME_OK;
  }
  else
  {
    delete psf;
    pLog->Add(100, "  Error");
    return GME_FILE_NOTOPEN;
  }
}

int CGMSaf::Drop(const char* saf_name)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Drop( %s )", (saf_name)?saf_name:"NULL");
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      m_saf_list[i]->Close(true);
      delete(m_saf_list[i]);
      m_saf_list.erase(m_saf_list.begin() + i);
      return GME_OK;
    }
  }
  pLog->Add(100, "  Error");
  return GME_SAF_NOT_FOUND;
}

bool CGMSaf::Exist(const char* saf_name)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Exist( %s )", (saf_name)?saf_name:"NULL");
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name)) return true;
  }
  pLog->Add(100, "  No existe");
  return false;
}

int CGMSaf::Info(const char* saf_name, CGMSafFile::saf_info *st)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Info( %s )", (saf_name)?saf_name:"NULL");
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      return m_saf_list[i]->Info(st);
    }
  }
  pLog->Add(100, "  Error");
  return GME_SAF_NOT_FOUND;
}

int CGMSaf::Add(const char* saf_name, const void* data,
                unsigned long len, unsigned int trans)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Add( %s, len: %lu )", (saf_name)?saf_name:"NULL", len);
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      return m_saf_list[i]->Add(data, len, trans);
    }
  }
  pLog->Add(100, "  Error");
  return GME_SAF_NOT_FOUND;
}

int CGMSaf::Get(const char* saf_name,
      void* data, unsigned long max_len,
      unsigned long *len, unsigned int trans)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Get( %s, max_len: %lu )", (saf_name)?saf_name:"NULL", max_len);

  vlen = m_saf_list.size();
  pLog->Add(100, "  vlen: %i", vlen);
  for(i = 0; i < vlen; i++)
  {
    pLog->Add(100, "  strcmp: [%s] [%s]", m_saf_list[i]->Name(), saf_name);
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      pLog->Add(100, "  SAF Encontrado");
      return m_saf_list[i]->Get(data, max_len, len, trans);
    }
  }
  pLog->Add(100, "  SAF NO Encontrado");
  return (-1);
}

int CGMSaf::CommitGet(const char* saf_name, unsigned int id, unsigned int trans)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::CommitGet( %s, Id: %li )", (saf_name)?saf_name:"NULL", id);
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      return m_saf_list[i]->CommitGet(id, trans);
    }
  }
  pLog->Add(100, "  Error");
  return GME_SAF_NOT_FOUND;
}

int CGMSaf::AbortGet(const char* saf_name, unsigned int id, unsigned int trans)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::AbortGet( %s, Id: %li )", (saf_name)?saf_name:"NULL", id);
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      return m_saf_list[i]->AbortGet(id, trans);
    }
  }
  pLog->Add(100, "  Error");
  return GME_SAF_NOT_FOUND;
}

int CGMSaf::Delete(const char* saf_name, unsigned int id, unsigned int trans)
{
  int i, vlen;

  pLog->Add(100, "CGMSaf::Delete( %s, Id: %li )", (saf_name)?saf_name:"NULL", id);
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    if( !strcmp(m_saf_list[i]->Name(), saf_name))
    {
      return m_saf_list[i]->Delete(id, trans);
    }
  }
  pLog->Add(100, "  Error");
  return GME_SAF_NOT_FOUND;
}

vector <CGMSafFile::saf_info> CGMSaf::List()
{
  vector <CGMSafFile::saf_info> v;
  CGMSafFile::saf_info si;
  int i, vlen;

  pLog->Add(100, "CGMSaf::List( )");
  v.clear();
  vlen = m_saf_list.size();
  for(i = 0; i < vlen; i++)
  {
    m_saf_list[i]->Info(&si);
    v.push_back(si);
  }
  return v;
}
