/*
  $Id$

  Copyright (C) 2005  Dmitry V. Levin <ldv@altlinux.org>

  The faketime utility.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <unistd.h>
#include "getdate.h"

static void __attribute__ ((noreturn))
show_usage(const char *str)
{
	fprintf(stderr, "%s: %s\nTry `%s --help' for more information.\n",
		program_invocation_short_name, str,
		program_invocation_short_name);
	exit(EXIT_FAILURE);
}

static void __attribute__ ((noreturn))
print_help(void)
{
	printf("Execute program with changed notion of system time.\n"
	       "\nUsage: %s [options] <timespec> <program>...\n"
	       "\nValid options are:\n"
	       "  --version:\n"
	       "       print program version and exit.\n"
	       "  -h or --help:\n"
	       "       print this help text and exit.\n"
	       "\nTime specification can be in almost any common format.\n",
	       program_invocation_short_name);
	exit(EXIT_SUCCESS);
}

static void __attribute__ ((noreturn))
print_version(void)
{
	printf("%s", PACKAGE_STRING "\n"
	       "\nCopyright (C) 2005  Dmitry V. Levin <ldv@altlinux.org>\n"
	       "\nThis is free software; see the source for copying conditions.  There is NO\n"
	       "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
	       "\nWritten by Dmitry V. Levin <ldv@altlinux.org>\n");
	exit(EXIT_SUCCESS);
}

int
main(int ac, const char **av)
{
	struct timespec now, tp;
	const char *timespec, *preload;
	char   *env;

	if (ac < 2)
		show_usage("insufficient arguments");

	if (!strcmp("-h", av[1]) || !strcmp("--help", av[1]))
		print_help();

	if (!strcmp("--version", av[1]))
		print_version();

	if (ac < 3)
		show_usage("insufficient arguments");

	timespec = (av[1][0] == '-' || av[1][0] == '+') ? av[1] + 1 : av[1];
	memset(&tp, 0, sizeof tp);
	gettime(&now);
	if (!get_date(&tp, timespec, &now))
		error(EXIT_FAILURE, 0, "invalid date: %s", av[1]);

	if (timespec == av[1])
	{
		if (asprintf(&env, "%lu", (unsigned long) tp.tv_sec) < 0)
			error(EXIT_FAILURE, errno, "asprintf");
	} else
	{
		if (asprintf
		    (&env, "%+ld", (long) tp.tv_sec - (long) now.tv_sec) < 0)
			error(EXIT_FAILURE, errno, "asprintf");
	}

	if (setenv("FAKETIME", env, 1))
		error(EXIT_FAILURE, errno, "putenv");
	free(env);

	if ((preload = getenv("LD_PRELOAD")))
	{
		if (asprintf(&env, "%s:%s", "faketime.so", preload) < 0)
			error(EXIT_FAILURE, errno, "asprintf");
		if (setenv("LD_PRELOAD", env, 1))
			error(EXIT_FAILURE, errno, "putenv");
		free(env);
	} else
	{
		if (setenv("LD_PRELOAD", "faketime.so", 1))
			error(EXIT_FAILURE, errno, "putenv");
	}

	execvp(av[2], (char *const *) av + 2);
	error(EXIT_FAILURE, errno, "execvp: %s", av[2]);
	return EXIT_FAILURE;
}
