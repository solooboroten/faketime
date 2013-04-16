/*
  Copyright (C) 2005-2013  Dmitry V. Levin <ldv@altlinux.org>

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
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include "parse-datetime.h"
#include "posixtm.h"
#include "timespec.h"

static void __attribute__ ((noreturn))
show_usage(const char *str)
{
	if (str)
		fprintf(stderr, "%s: %s\n",
			program_invocation_short_name, str);
	fprintf(stderr, "Try `%s --help' for more information.\n",
		program_invocation_short_name);
	exit(EXIT_FAILURE);
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

static void __attribute__ ((noreturn))
print_help(void)
{
	printf("Execute program with changed notion of system time.\n"
	       "\nUsage: %s [options] <program>...\n"
	       "\nValid options are:\n"
	       "  -d STRING:\n"
	       "       use time described by STRING\n"
	       "  -r FILE:\n"
	       "       use the last modification time of FILE\n"
	       "  -t STAMP:\n"
	       "       use timestamp in [[CC]YY]MMDDhhmm[.ss] format\n"
	       "  -i:\n"
	       "       do not freeze the time, increment it in usual way\n"
	       "  -V, --version:\n"
	       "       print program version and exit\n"
	       "  -h, --help:\n"
	       "       print this help text and exit\n"
	       "\nTime specifications for the -d and -t options have\n"
	       "the same format as in date(1) and touch(1) utilities.\n",
	       program_invocation_short_name);
	exit(EXIT_SUCCESS);
}

static char const short_options[] = "+d:r:t:iVh";

static struct option const long_options[] = {
	{"version", no_argument, NULL, 'V'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

int
main(int ac, char **av)
{
	int     optc;
	int     relative = 0;
	struct timespec now, when;
	const char *d_str = 0, *r_str = 0, *t_str = 0;
	const char *preload;
	char   *env;

	if (ac < 2)
		show_usage("insufficient arguments");

	while ((optc =
		getopt_long(ac, av, short_options, long_options, NULL)) != -1)
	{
		switch (optc)
		{
			case 'd':
				d_str = optarg;
				break;
			case 'r':
				r_str = optarg;
				break;
			case 't':
				t_str = optarg;
				break;
			case 'i':
				relative = 1;
				break;
			case 'V':
				print_version();
				break;
			case 'h':
				print_help();
				break;
			default:
				show_usage(NULL);
				break;
		}
	}

	if (optind >= ac)
		show_usage("insufficient arguments");

	if (t_str && (r_str || d_str))
		show_usage("cannot specify times from more than one source");

	gettime(&now);
	memset(&when, 0, sizeof when);

	if (t_str)
	{
		if (!posixtime
		    (&when.tv_sec, t_str,
		     PDS_LEADING_YEAR | PDS_CENTURY | PDS_SECONDS))
			error(EXIT_FAILURE, 0, "%s: invalid date format",
			      t_str);
	} else if (r_str)
	{
		struct stat stb;

		if (stat(r_str, &stb))
			error(EXIT_FAILURE, errno,
			      "%s: failed to get attributes", r_str);
		when.tv_sec = stb.st_mtime;
		if (d_str && !parse_datetime(&when, d_str, &when))
			error(EXIT_FAILURE, 0, "%s: invalid date format",
			      d_str);
	} else if (d_str)
	{
		if (!parse_datetime(&when, d_str, NULL))
			error(EXIT_FAILURE, 0, "%s: invalid date format",
			      d_str);
	} else
		show_usage("no time source specified");

	if (relative)
	{
		if (asprintf
		    (&env, "%+ld",
		     (long) when.tv_sec - (long) now.tv_sec) < 0)
			error(EXIT_FAILURE, errno, "asprintf");
	} else
	{
		if (asprintf(&env, "%lu", (unsigned long) when.tv_sec) < 0)
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

	execvp(av[optind], (char *const *) av + optind);
	error(EXIT_FAILURE, errno, "execvp: %s", av[optind]);
	return EXIT_FAILURE;
}
