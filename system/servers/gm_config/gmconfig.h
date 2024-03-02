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
#ifndef _GMCONFIG_H_
#define _GMCONFIG_H_

#include <gmontdb.h>
#include <msgtype.h>
#include <gms.h>
#include <gmsbase.h>

class CGMConfig
{
public:
  CGMConfig(CGMServer* pServer, CGLog* plog);
  virtual ~CGMConfig();

  int Process(const char *funcion, char typ,
        void* in, unsigned long inlen,
        void* out, unsigned long *outlen,
        CGMServerBase::CLIENT_DATA* pClientData);

protected:
  CGMTdb* m_pDB;
  CGMServer* m_pServer;
  CGLog* m_pLog;
  typedef struct _SERVICE_REL
  {
    char svc[33];
    char typ;
    char saf[33];

  } SERVICE_REL;

  vector<SERVICE_REL> m_vServiceRel;

  int AddService(const char* svc, char typ, const char* saf);
  int RmvService(const char* svc, char typ, const char* saf);
  const char* GetSafName(const char* svc, char typ);

};
#endif /* _GMCONFIG_H_ */

