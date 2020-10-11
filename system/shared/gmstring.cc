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

/*
    Funciones para el manejo de cadenas de caracteres
*/
#include "gmstring.h"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
using namespace std;

#include <string.h>

char* substring(char* out, const void* in, int from, unsigned int len)
{
	if(!out || !in) return NULL;
	if(from < 0 || len <= 0) return NULL;

	memcpy(out,(char*)in + from,len);
	out[len] = 0;
	return out;
}

int subint(const void* str, int from, unsigned int len)
{
	char s[10240];
	if(substring(s,str,from,len) != NULL)
	{
		return atoi(s);
	}
	else
	{
		return 0;
	}
}

long sublong(const void* str, int from, unsigned int len)
{
	char s[10240];
	if(substring(s,str,from,len) != NULL)
	{
		return atol(s);
	}
	else
	{
		return 0;
	}
}

char* memprint(char* str, const char* fmt, ...)
{
	va_list arg;
	char s[10240];

	va_start(arg,fmt);
	vsprintf(s, fmt, arg);
	va_end(arg);
	memcpy(str,s,strlen(s));
	return str;
}

vector<string> split(const char* s, const char sep)
{
	int i, len;
	char *ss;
	char *ps;
	vector<string> vs;

	len = strlen(s);
	ps = new char[len+1];
	strcpy(ps, s);
	ss = ps;
	for(i = 0; i < len; i++)
	{
		if(ps[i] == sep)
		{
			ps[i] = '\0';
			vs.push_back(string(ss));
			ss = &ps[i+1];
		}
	}
	vs.push_back(string(ss));
	delete ps;
	return vs;
}

vector<string> split(string s, const char sep)
{
	return split(s.c_str(),sep);
}
