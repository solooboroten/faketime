Name: faketime
Version: 0.2.5
Release: alt1

Summary: Execute program with changed notion of system time
License: GPLv3+
Group: Development/Other

# git://git.altlinux.org/gears/f/faketime.git
Source: %name-%version.tar

BuildRequires: help2man, gnulib >= 0.0.7902.92f3a4c

%description
The faketime utility helps to execute programs with changed notion of
system time.

%prep
%setup
install -pm755 %_datadir/gnulib/build-aux/bootstrap .
echo -n %version > .tarball-version
mkdir build-aux
# remove timespectod() to avoid compilation warnings in unused code.
sed '/Return an approximation/,/}$/d' < %_datadir/gnulib/lib/timespec.h > timespec.h

%build
./bootstrap --gnulib-srcdir=%_datadir/gnulib --no-git --skip-po --force
%configure
%make_build

%install
%makeinstall_std
rm %buildroot%_libdir/*.la

%check
%make_build -k check

%files
%_bindir/*
%_libdir/*.so
%_man1dir/*

%changelog
* Wed Apr 17 2013 Dmitry V. Levin <ldv@altlinux.org> 0.2.5-alt1
- Converted from getdate to parse_datetime gnulib module.
- Enabled gnulib testsuite.
- Built with system gnulib v0.0.7902-g92f3a4c.

* Sun Nov 20 2011 Dmitry V. Levin <ldv@altlinux.org> 0.2.4-alt1
- Updated gnulib to v0.0-6453-g6a4c64c.

* Fri Feb 12 2010 Dmitry V. Levin <ldv@altlinux.org> 0.2.3-alt1
- Updated to fresh gnulib.

* Tue Oct 28 2008 Dmitry V. Levin <ldv@altlinux.org> 0.2.2-alt1
- Fixed build with fresh gcc.

* Sat Aug 30 2008 Dmitry V. Levin <ldv@altlinux.org> 0.2.1-alt1
- Updated gnulib.

* Sun Apr 15 2007 Dmitry V. Levin <ldv@altlinux.org> 0.2-alt1
- Changed option processing to stop after the first non-option argument (at@).
- Fixed to build with current gnulib.

* Thu Dec 15 2005 Dmitry V. Levin <ldv@altlinux.org> 0.1-alt1
- Initial revision.