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
#include "wtimer.h"

#include <string.h>

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

CWTimer::CWTimer() { }

CWTimer::~CWTimer() { }

int CWTimer::Add(const void* key, unsigned int key_len, const void* data, unsigned int data_len, unsigned int to)
{
	time_t t;
	ST_WTIMER wt;

	if(Exist(key, key_len)) return -1;
	t = time(&t);
	wt.end_time = (time_t)(t + to); 
	wt.key = new char[key_len];
	memcpy(wt.key, key, key_len);
	wt.key_len = key_len;
	if(data && data_len)
	{
		wt.data = new char[data_len];
		memcpy(wt.data, data, data_len);
		wt.data_len = data_len;
	}
	else
	{
		wt.data = NULL;
		wt.data_len = 0;
	}
	m_vTimer.push_back(wt);
	return 0;
}

bool CWTimer::Exist(const void* key, unsigned int key_len)
{
	unsigned int vlen;
	unsigned int i;

	vlen = m_vTimer.size();
	for(i = 0; i < vlen; i++)
	{
		if(m_vTimer[i].key_len == key_len)
		{
			if( !memcmp(m_vTimer[i].key, key, key_len))
			{
				return true;
			}
		}
	}
	return false;
}

int CWTimer::Get(const void* key, unsigned int key_len, void* data, unsigned int max_len, unsigned int* data_len)
{
	unsigned int vlen;
	unsigned int i;

	vlen = m_vTimer.size();
	for(i = 0; i < vlen; i++)
	{
		if(m_vTimer[i].key_len == key_len)
		{
			if( !memcmp(m_vTimer[i].key, key, key_len))
			{
				if(m_vTimer[i].data_len > max_len) return -1;
				memcpy(data, m_vTimer[i].data, m_vTimer[i].data_len);
				*data_len = m_vTimer[i].data_len;
				return 0;
			}
		}
	}
	return -1;
}

int CWTimer::Del(const void* key, unsigned int key_len)
{
	unsigned int vlen;
	unsigned int i;

	vlen = m_vTimer.size();
	for(i = 0; i < vlen; i++)
	{
		if(m_vTimer[i].key_len == key_len)
		{
			if( !memcmp(m_vTimer[i].key, key, key_len))
			{
				if(m_vTimer[i].data_len) delete (char*)m_vTimer[i].data;
				delete (char*)m_vTimer[i].key;
				m_vTimer.erase(m_vTimer.begin() + i);
				return 0;
			}
		}
	}
	return -1;
}

int CWTimer::Check(void* key, unsigned int max_key_len, unsigned int* key_len,
                   void* data, unsigned int max_data_len, unsigned int* data_len)
{
	unsigned int vlen;
	unsigned int i;
	time_t t;

	t = time(&t);
	vlen = m_vTimer.size();
	for(i = 0; i < vlen; i++)
	{
		if(m_vTimer[i].end_time && m_vTimer[i].end_time <= t)
		{
			if(m_vTimer[i].key_len > max_key_len && max_key_len) return -1;
			if(m_vTimer[i].data_len > max_data_len && max_data_len) return -1;
			
			if(m_vTimer[i].data_len && data && max_data_len)
			{
				memcpy(data, m_vTimer[i].data, m_vTimer[i].data_len);
			}
			if(data_len) *data_len = m_vTimer[i].data_len;
			if(key && max_key_len)
			{
				memcpy(key, m_vTimer[i].key, m_vTimer[i].key_len);
			}
			if(key_len) *key_len = m_vTimer[i].key_len;
			delete (char*)m_vTimer[i].key;
			if(m_vTimer[i].data_len)
			{
				delete (char*)m_vTimer[i].data;
			}
			m_vTimer.erase(m_vTimer.begin() + i);
			return 1;
		}
	}
	return 0;
}

int CWTimer::NextTo(int max_to)
{
	unsigned int vlen;
	unsigned int i;
	time_t t;

	t = time(&t);
	vlen = m_vTimer.size();
	for(i = 0; i < vlen; i++)
	{
		if(m_vTimer[i].end_time && (max_to < 0 || (m_vTimer[i].end_time - t) < max_to))
		{
			max_to = m_vTimer[i].end_time - t;
			if(max_to < 0) max_to = 0;
		}
	}
	return max_to;
}

