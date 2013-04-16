dnl Copyright (C) 2005-2013  Dmitry V. Levin <ldv@altlinux.org>

dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
dnl GNU General Public License for more details.

dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([faketime],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[ldv@altlinux.org])
AC_CONFIG_SRCDIR([plugin.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([lib/config.h:lib/config.hin])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_PROG_CC
gl_EARLY
gl_INIT
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile lib/Makefile gnulib-tests/Makefile])
AC_OUTPUT
