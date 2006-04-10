Name: faketime
Version: 0.1
Release: alt1

Summary: Execute program with changed notion of system time
License: GPL
Group: Development/Other
Packager: Dmitry V. Levin <ldv@altlinux.org>

Source: %name-%version.tar.bz2

BuildPreReq: help2man

%description
The faketime utility helps to execute programs with changed notion of
system time.

%prep
%setup -q

%build
%configure
%make_build

%install
%makeinstall
rm %buildroot%_libdir/*.la

%files
%_bindir/*
%_libdir/*.so
%_man1dir/*

%changelog
* Thu Dec 15 2005 Dmitry V. Levin <ldv@altlinux.org> 0.1-alt1
- Initial revision.
