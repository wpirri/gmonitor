/***************************************************************************
    Copyright (C) 2003  Walter Pirri

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
#ifndef _GMI_H_
#define _GMI_H_

#include <string>
using namespace std;

/* constantes para m_flags */
#define GMFLAG_VERBOSE         0x00000001

class CGMInitData
{
public:
	CGMInitData(CGMInitData *init_data = NULL);
	virtual ~CGMInitData();

	string m_host;
	long m_port;
	string m_user;
	string m_client;
	string m_key;
	string m_group;
	long m_flags;
  
  bool IsVerbose();
};
#endif /* _GMI_H_ */

