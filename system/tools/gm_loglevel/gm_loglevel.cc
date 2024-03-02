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
#include "classapp.h"

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void OnChildExit(int /*sig*/)
{
	int st;

	signal(SIGCHLD, OnChildExit);
	wait(&st);
}

int main(int argc, char** argv, char** env)
{
	CClassApp app;

	signal(SIGALRM, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, OnChildExit);

	return app.Main(argc, argv, env);
}
