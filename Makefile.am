# Copyright (C) 2005-2013  Dmitry V. Levin <ldv@altlinux.org>
#
# Makefile for the faketime project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

SUBDIRS = lib gnulib-tests

WARNINGS = -W -Wall -Waggregate-return -Wcast-align -Wconversion \
	-Wdisabled-optimization -Wmissing-declarations \
	-Wmissing-format-attribute -Wmissing-noreturn \
	-Wmissing-prototypes -Wpointer-arith -Wshadow \
	-Wstrict-prototypes -Wwrite-strings -Werror

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -std=gnu99 $(WARNINGS)
AM_LDFLAGS = -no-undefined -avoid-version -module \
	-export-symbols $(top_srcdir)/plugin.sym -ldl
INCLUDES = -I$(top_srcdir)/lib

lib_LTLIBRARIES = faketime.la
faketime_la_SOURCES = plugin.c

bin_PROGRAMS = faketime
faketime_SOURCES = util.c
faketime_LDADD = $(top_builddir)/lib/libfaketime.la @LIB_CLOCK_GETTIME@

man_MANS = faketime.1
HELP2MAN1 = help2man -N -s1

%.1: % %.1.inc
	$(HELP2MAN1) --include=$(top_srcdir)/$<.1.inc $(top_builddir)/$< --output=$@

EXTRA_DIST = faketime.1.inc faketime.spec plugin.sym
CLEANFILES = $(man_MANS) *~
