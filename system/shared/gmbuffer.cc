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

#include "gmbuffer.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
using namespace std;

#include <string.h>

CGMBuffer::CGMBuffer()
{
	m_len = 0;
	m_buffer = NULL;
}

CGMBuffer::CGMBuffer(const char* str)
{
	if(str)
	{
		m_len = strlen(str);
		m_buffer = (char*)calloc(m_len + 1, sizeof(char));
		strcpy(m_buffer, str);
	}
	else
	{
		m_len = 0;
		m_buffer = NULL;
	}
}

CGMBuffer::CGMBuffer(const void* ptr, unsigned long len)
{
	if(ptr && len)
	{
		m_len = len;
		m_buffer = (char*)calloc(m_len+1, sizeof(char));
		memcpy(m_buffer, ptr, m_len);
    m_buffer[m_len] = '\0';
	}
	else
	{
		m_len = 0;
		m_buffer = NULL;
	}
}

CGMBuffer::CGMBuffer(string s)
{
	m_len = s.length();
	m_buffer = (char*)calloc(m_len + 1, sizeof(char));
	memcpy(m_buffer, s.c_str(), m_len);
	m_buffer[m_len] = '\0';
}

CGMBuffer::~CGMBuffer()
{
	Clear();
}

void CGMBuffer::Clear()
{
	if(m_buffer)
	{
		m_len = 0;
		free(m_buffer);
		m_buffer = NULL;
	}
}

const char* CGMBuffer::operator=(const char *str)
{
	return Set(str);
}

const char* CGMBuffer::operator=(CGMBuffer buffer)
{
	return Set(buffer);
}

const char* CGMBuffer::operator=(string s)
{
	return Set(s);
}

const char* CGMBuffer::operator+=(const char *str)
{
	return Add(str);
}

const char* CGMBuffer::operator+=(CGMBuffer buffer)
{
	return Add(buffer);
}

const char* CGMBuffer::operator+=(string s)
{
	return Add(s);
}

const char* CGMBuffer::Set(const char *str)
{
	Clear();
	return Add(str);
}

const char* CGMBuffer::Set(const void *ptr, unsigned long len)
{
	Clear();
	return Add(ptr, len);
}

const char* CGMBuffer::Set(CGMBuffer buffer)
{
	Clear();
	return Add(buffer);
}

const char* CGMBuffer::Set(string s)
{
	Clear();
	return Add(s);
}

const char* CGMBuffer::Add(const char *str)
{
	return Add(str, strlen(str));
}

const char* CGMBuffer::Add(const void *ptr, unsigned long len)
{
	char *b;

	if(ptr && len)
	{
		if(m_buffer)
		{
			b = (char*)realloc(m_buffer, m_len + len + 1);
			m_buffer = b;
			memcpy((char*)(m_buffer + m_len), ptr, len);
			m_len += len;
		}
		else
		{
			m_len = len;
			m_buffer = (char*)calloc(m_len + 1, sizeof(char));
			memcpy(m_buffer, ptr, m_len);
		}
    m_buffer[m_len] = '\0';
	}
	return (const char*)m_buffer;
}

const char* CGMBuffer::Add(CGMBuffer buffer)
{
	return Add(buffer.C_Str(), buffer.Length());
}

const char* CGMBuffer::Add(string s)
{
	return Add(s.c_str());
}

const char* CGMBuffer::Format(const char* fmt, ...)
{
	char str[10240];
	va_list arg;

	va_start(arg, fmt);
	vsprintf(str, fmt, arg);
	va_end(arg);
	Clear();
	return Add(str);
}

const char* CGMBuffer::AddFormat(const char* fmt, ...)
{
	char str[10240];
	va_list arg;

	va_start(arg, fmt);
	vsprintf(str, fmt, arg);
	va_end(arg);
	return Add(str);
}

const char* CGMBuffer::Data()
{
	return (const char*)m_buffer;
}

const char* CGMBuffer::C_Str()
{
	return (const char*)m_buffer;
}

unsigned long CGMBuffer::Length()
{
	return m_len;
}

string CGMBuffer::String()
{
	return string(m_buffer, m_len);
}

const char* CGMBuffer::At(unsigned long idx)
{
	return (const char*)&m_buffer[idx];
}
