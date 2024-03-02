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
#ifndef _GLOG_H_
#define _GLOG_H_

#include <string>
#include <iostream>
using namespace std;

#define GLOG_LEVEL_DEBUG      100
#define GLOG_LEVEL_INFO        50
#define GLOG_LEVEL_WARNING     20
#define GLOG_LEVEL_ERROR       10
#define GLOG_LEVEL_FATAL        1

class CGLog  
{
public:
	CGLog(const char *appname, const char *logpath, unsigned int level = 0);
	virtual ~CGLog();

	void Add(unsigned int level, const char* msg, ...);
	void AddBin(unsigned int level, const char *id, const void *buffer, unsigned int len);
	unsigned int LogLevel();
	void LogLevel(unsigned int level);

protected:
	string m_logpath;
	string m_appname;
	int m_prefijo;
	unsigned int m_log_level;

};

#endif /* _GLOG_H_ */
