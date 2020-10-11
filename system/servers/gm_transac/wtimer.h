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
#ifndef _WTIMER_H_
#define _WTIMER_H_

#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include <time.h>

class CWTimer
{
public:
	CWTimer();
	virtual ~CWTimer();

	int Add(const void* key, unsigned int key_len, const void* data, unsigned int data_len, unsigned int to);
	bool Exist(const void* key, unsigned int key_len);
	int Get(const void* key, unsigned int key_len, void* data, unsigned int max_len, unsigned int* data_len);
	int Del(const void* key, unsigned int key_len);
	int Check(void* key, unsigned int max_key_len, unsigned int* key_len,
                  void* data, unsigned int max_data_len, unsigned int* data_len);
	int NextTo(int max_to = -1);

private:
	typedef struct _ST_WTIMER
	{
		time_t	end_time;
		void*		key;
		unsigned int	key_len;
		void*		data;
		unsigned int	data_len;
	} ST_WTIMER;
	vector <ST_WTIMER> m_vTimer;
};

#endif /* _WTIMER_H_ */
