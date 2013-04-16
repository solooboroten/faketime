/*
  Copyright (C) 2005-2013  Dmitry V. Levin <ldv@altlinux.org>

  The faketime plugin.

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

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <dlfcn.h>

#ifndef HAVE__THREAD
#define __thread
#endif

static int debug = 0;
static __thread sig_atomic_t recursion = 0;
static time_t time_abs = 0, time_off = 0;

static void __attribute__ ((constructor))
init_sym(void)
{
	const char *name = "FAKETIME";
	const char *value = getenv(name) ? : "";
	char   *p = 0;
	unsigned long n;
	time_t  t = 0;

	if (!*value)
	{
		fprintf(stderr, "%s: %s\n", name, strerror(EINVAL));
		exit(1);
	}

	errno = 0;
	n = strtoul(value, &p, 10);
	if (!p || *p)
		errno = EINVAL;
	else
	{
		t = (time_t) n;
		if ((unsigned long) t != n)
			errno = ERANGE;
	}
	if (!p || *p || (n == ULONG_MAX && errno == ERANGE)
	    || ((unsigned long) t != n))
	{
		fprintf(stderr, "%s: %s: %s\n", name, value, strerror(errno));
		exit(1);
	}

	if (value[0] == '-' || value[0] == '+')
		time_off = (time_t) t;
	else
		time_abs = (time_t) t;
}

static void *load_sym(const char *name)
{
	void *addr;
	const char *msg;

	(void) dlerror();
	addr = dlsym(RTLD_NEXT, name);
	if ((msg = dlerror()))
	{
		fprintf(stderr, "dlsym(%s): %s\n", name, msg);
		abort();
	}
	return addr;
}

void    debug_printf(const char *, ...)
	__attribute__ ((__format__(__printf__, 1, 2)));

void __attribute__ ((__format__(__printf__, 1, 2)))
debug_printf(const char *fmt, ...)
{
	va_list args;

	if (!debug)
		return;

	va_start(args, fmt);
	(void) vfprintf(stderr, fmt, args);
	va_end(args);
}

time_t
time(time_t *t)
{
	static  time_t (*next_time) (time_t *);
	time_t  rc;

	if (!next_time)
		next_time = load_sym("time");

	debug_printf("time(%p) = <", t);
	++recursion;
	rc = next_time(t);
	--recursion;
	debug_printf("%lu>\n", (unsigned long) rc);
	if (rc == (time_t) - 1)
		return rc;

	if (!recursion)
	{
		debug_printf("time: %lu -> ", (unsigned long) rc);
		if (time_off)
		{
			rc += time_off;
		} else if (time_abs)
		{
			rc = time_abs;
		}
		debug_printf("%lu\n", (unsigned long) rc);
	}

	if (t)
		*t = rc;

	return rc;
}

int
ftime(struct timeb *tp)
{
	static int (*next_ftime) (struct timeb *);
	int     rc;

	if (!next_ftime)
		next_ftime = load_sym("ftime");

	debug_printf("ftime(%p) = <", tp);
	++recursion;
	rc = next_ftime(tp);
	--recursion;
	debug_printf("%d>\n", rc);
	if (rc)
		return rc;

	if (!recursion && tp)
	{
		debug_printf("ftime: %lu -> ", (unsigned long) tp->time);
		if (time_off)
		{
			tp->time += time_off;
		} else if (time_abs)
		{
			tp->time = time_abs;
			tp->millitm = 0;
		}
		debug_printf("%lu\n", (unsigned long) tp->time);
	}

	return rc;
}

int
gettimeofday(struct timeval *tv, struct timezone *tz)
{
	static int (*next_gettimeofday) (struct timeval *, struct timezone *);
	int     rc;

	if (!next_gettimeofday)
		next_gettimeofday = load_sym("gettimeofday");

	debug_printf("gettimeofday(%p, %p) = <", tv, tz);
	++recursion;
	rc = next_gettimeofday(tv, tz);
	--recursion;
	debug_printf("%d>\n", rc);
	if (rc)
		return rc;

	if (!recursion && tv)
	{
		debug_printf("gettimeofday: %lu -> ",
			     (unsigned long) tv->tv_sec);
		if (time_off)
		{
			tv->tv_sec += time_off;
		} else if (time_abs)
		{
			tv->tv_sec = time_abs;
			tv->tv_usec = 0;
		}
		debug_printf("%lu\n", (unsigned long) tv->tv_sec);
	}

	return rc;
}

int
clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	static int (*next_clock_gettime) (clockid_t, struct timespec *);
	int     rc;

	if (!next_clock_gettime)
		next_clock_gettime = load_sym("clock_gettime");

	debug_printf("clock_gettime(%d, %p) = <", clk_id, tp);
	++recursion;
	rc = next_clock_gettime(clk_id, tp);
	--recursion;
	debug_printf("%d>\n", rc);

	if (rc || clk_id != CLOCK_REALTIME)
		return rc;

	if (!recursion && tp)
	{
		debug_printf("clock_gettime: %lu -> ",
			     (unsigned long) tp->tv_sec);
		if (time_off)
		{
			tp->tv_sec += time_off;
		} else if (time_abs)
		{
			tp->tv_sec = time_abs;
			tp->tv_nsec = 0;
		}
		debug_printf("%lu\n", (unsigned long) tp->tv_sec);
	}

	return rc;
}
