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
  Implementación de archivo de Store & Forward
*/
#ifndef _GMSAF_FILE_H_
#define _GMSAF_FILE_H_

#include <vector>
using namespace std;

#include <stdio.h>

#define SAFNAME_MAX 32

class CGMSafFile
{
public:
  CGMSafFile();
  CGMSafFile(const char *filename, const char* name);
  virtual ~CGMSafFile();

  typedef struct _saf_info
  {
    char filename[FILENAME_MAX];
    char name[SAFNAME_MAX];
    unsigned long record_count;
    unsigned long data_count;
  } saf_info;

  /*
    Open: Inicializa la abstracción de store & forward
    parametro filename: nombre completo del archivo de SAF
  */
  int Open(const char *filename, const char* name);
  /*
    Drop: Elimina un archivo de SAF y todo su contenido
  */
  int Close(bool drop = false);

  int Add(const void* data, unsigned long len, unsigned int trans = 0);
  /*
  Devuelve:
    0  Error
    >0  ID del registro devuelto
 */
 /*
  En caso de error devuelvo 0 que no es un valor valido
  porque es el ID delregistro inicial de control
*/
  unsigned int Get(void* data, unsigned long max_len,
        unsigned long *len, unsigned int trans = 0);
  /* es igual que el Delete pero verifica que esté en estado S */
  int CommitGet(unsigned int id, unsigned int trans = 0);
  int AbortGet(unsigned int id, unsigned int trans = 0);
  int Delete(unsigned int id, unsigned int trans = 0);
  int Info(saf_info *st);
  bool IsOpen();
  const char* Name();
  const char* FileName();
  unsigned long GetRecordCount();
  unsigned long GetDataCount();

protected:
  typedef struct _saf_record_header
  {
    unsigned int id;
    unsigned int trans;
    unsigned long record_len; /* va a contener el tamaño del registro
               que puede ser igual o mayor a los datos cuando el registro
               haya sido reutilizado */
    unsigned long data_len; /* contienen el valor del dato en el registro */
    /*  N: dato nuevo
        S: Enviado
        F: Enviado Fallido
        D: borrado
        X: registro de control
    */
    char record_status;
  } saf_record_header;

  typedef struct _saf_index
  {
    unsigned int id;
    unsigned int trans;
    unsigned long record_len;
    char record_status;
    long header_pos;
  } saf_index;

  saf_info m_info;
  FILE *m_fd;
  unsigned int m_last_id;
  vector <saf_index> m_saf_index;

  unsigned int GenId();
  int GetSafInfo();

};
#endif /* _GMSAF_FILE_H_ */
