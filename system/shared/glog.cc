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
    Logger
*/
#include <glog.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <csignal>
#include <cstdarg>
using namespace std;

#include <unistd.h>
#include <sys/types.h>


CGLog::CGLog(const char *appname, const char *logpath, unsigned int level)
{
	if(logpath) { m_logpath = logpath; }
	else { m_logpath = "/var/log"; }
	if(appname) { m_appname = appname; }
	else { m_appname = "general"; }
	m_prefijo = getpid();
	m_log_level = level;

	Add(1, "********** Log Start Level [%i] **********", m_log_level);
}

CGLog::~CGLog()
{

}

void CGLog::Add(unsigned int level, const char *msg, ...)
{
	FILE *arch;
	char filename[FILENAME_MAX];
	time_t t;
	struct tm *local;
	va_list arg;

	if(level > m_log_level) return ;

	t = time(&t);
	local = localtime(&t);
	sprintf(filename, "%s/%s-%04i%02i%02i.log", 
			m_logpath.c_str(), 
			m_appname.c_str(),
			(local->tm_year + 1900),
			local->tm_mon + 1,
			local->tm_mday);
	if((arch = fopen(filename, "ab")) != NULL)
	{
		// guardo fecha y hora
		fprintf(arch, "%06i|%02i%02i%02i|",
				/*m_prefijo*/getpid(),/*ver si se usa para algo prefijo, sino sacarlo*/
				local->tm_hour,
				local->tm_min,
				local->tm_sec);
		va_start(arg, msg);
		vfprintf(arch, msg, arg);
		va_end(arg);
		fprintf(arch, "\n");
		fclose(arch);
	}
}

void CGLog::AddBin(unsigned int level, const char *id, const void *buffer, unsigned int len)
{
        FILE *arch;
        char filename[64];
        time_t t;
        struct tm *local;

	if(level > m_log_level) return ;

        t = time(&t);
        local = localtime(&t);
	sprintf(filename, "%s/%s-%04i%02i%02i.log", 
			m_logpath.c_str(), 
			m_appname.c_str(),
			(local->tm_year + 1900),
			local->tm_mon + 1,
			local->tm_mday);
	if((arch = fopen(filename, "ab")) != NULL)
        {
                // guardo la hora
                fprintf(arch, "%02i:%02i:%02i %s [",
                                        local->tm_hour,
                                        local->tm_min,
                                        local->tm_sec,
					id);
		fflush(arch);
		fwrite(buffer, sizeof(char), len, arch);
                fprintf(arch, "] [len=%i]\r\n", len);
                fclose(arch);
        }
}

void CGLog::Clean(unsigned int days)
{

}

unsigned int CGLog::LogLevel()
{
	return m_log_level;
}

void CGLog::LogLevel(unsigned int level)
{
	Add(1, "LogLevel [%i] -> [%i]", m_log_level, level);
	m_log_level = level;
}

