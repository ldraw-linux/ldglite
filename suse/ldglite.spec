# spec file for package ldglite
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name:           ldglite
Version:	__VERSION__
Release:	0
License:	GPL-2.0
Summary:	LDraw LEGO CAD renderer
Url:		http://ldglite.sourceforge.net/
Group:		Productivity/Graphics/CAD
Source:		ldglite.tar.bz2
BuildRequires:	freeglut-devel libpng-devel Mesa-devel gcc-c++
Requires:	ldraw-library
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Provides:	ldraw-renderer

%description
ldglite is a renderer of LDraw LEGO models.

LDraw is an open standard for LEGO CAD programs that allow the user to create
virtual LEGO models and scenes.

%prep
%setup -q -n ldglite

%build
make -f makefile.linux LDRAWDIR=/usr/share/ldraw

%install
install -d %{buildroot}/%{_bindir}
install -m 755 ldglite %{buildroot}/%{_bindir}/ldglite

%files
%defattr(-,root,root)
%{_bindir}/ldglite

%changelog
