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
    Funciones varias sacadas de otros proyectos GNU
    En este fuente se encuentran funciones miscelaneas implmentadas en ANSI C
*/

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

/*
	Ejecuta un programa y conecta los pipes con su stdin, stdout y stderr
	El ultimo param debe ser null
	Esta funcion se obtuvo del codigo del webserver Apache
*/
int exec_with_stdio(int* std_in, int* std_out, int* std_err, const char* exe, char* const params[])
{
	int pid;
	int in_fds[2];
	int out_fds[2];
	int err_fds[2];
	int save_errno;

	if(std_in && pipe(in_fds) < 0)
	{
        	return -1;
	}

	if(std_out && pipe(out_fds) < 0)
	{
		save_errno = errno;
		if (std_in)
		{
			close(in_fds[0]);
			close(in_fds[1]);
		}
		errno = save_errno;
		return -1;
	}

	if(std_err && pipe(err_fds) < 0)
	{
		save_errno = errno;
		if(std_in)
		{
			close(in_fds[0]);
			close(in_fds[1]);
		}
		if(std_out)
		{
			close(out_fds[0]);
			close(out_fds[1]);
		}
		errno = save_errno;
		return -1;
	}

	if((pid = fork()) < 0)
	{
		save_errno = errno;
		if(std_in)
		{
			close(in_fds[0]);
			close(in_fds[1]);
		}
		if(std_out)
		{
			close(out_fds[0]);
			close(out_fds[1]);
		}
		if(std_err)
		{
			close(err_fds[0]);
			close(err_fds[1]);
		}
		errno = save_errno;
		return -1;
	}

	if(!pid)
	{
		/* Child process */
		/*RAISE_SIGSTOP(SPAWN_CHILD);*/

		if(std_out)
		{
			close(out_fds[0]);
			dup2(out_fds[1], STDOUT_FILENO);
			close(out_fds[1]);
		}

		if(std_in)
		{
			close(in_fds[1]);
			dup2(in_fds[0], STDIN_FILENO);
			close(in_fds[0]);
		}

		if(std_err)
		{
			close(err_fds[0]);
			dup2(err_fds[1], STDERR_FILENO);
			close(err_fds[1]);
		}

		/* HP-UX SIGCHLD fix goes here, if someone will remind me what it is... */
		signal(SIGCHLD, SIG_DFL);       /* Was that it? */

		execv(exe, params); 

		exit(1);                /* Should only get here if the exec in func() failed */
	}

	/* Parent process */

	if(std_out)
	{
		close(out_fds[1]);
		*std_out = out_fds[0];
	}

	if(std_in)
	{
		close(in_fds[0]);
		*std_in = in_fds[1];
	}

	if(std_err)
	{
		close(err_fds[1]);
		*std_err = err_fds[0];
	}

	return pid;
}

