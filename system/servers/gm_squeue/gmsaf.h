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
/*
  Implementaci�n de Store & Forward
*/
#ifndef _GMSAF_H_
#define _GMSAF_H_

#include "gmsafile.h"

class CGMSaf
{
public:
  CGMSaf();
  virtual ~CGMSaf();

  /*
    Open: Inicializa la abstracci�n de store & forward
    parametro path: directorio base para la creaci�n de los archivos de SAF
  */
  int Open(const char *path = NULL);
  int Close(bool drop = false);

  /*
    Create: crea un archivo de SAF con un nombre determinado
    parametro saf_name: nombre del archivo de SAF a crear
  */
  int Create(const char* saf_name);
  /*
    Drop: Elimina un archivo de SAF y todo su contenido
  */
  int Drop(const char* saf_name);
  bool Exist(const char* saf_name);
  int Info(const char* saf_name, CGMSafFile::saf_info *st);
  int Add(const char* saf_name, const void* data,
          unsigned long len, unsigned int trans = 0);
  int Get(const char* saf_name,
        void* data, unsigned long max_len,
        unsigned long *len, unsigned int trans = 0);
  int CommitGet(const char* saf_name, unsigned int id, unsigned int trans = 0);
  int AbortGet(const char* saf_name, unsigned int id, unsigned int trans = 0);
  int Delete(const char* saf_name, unsigned int id, unsigned int trans = 0);
  vector <CGMSafFile::saf_info> List();

protected:
  vector <CGMSafFile*> m_saf_list;
  char m_saf_path[FILENAME_MAX];

};

#endif /* _GMSAF_H_ */
