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
Version:	__UPSTREAM_VERSION__
Release:	__RELEASE_VERSION__
License:	GPL-2.0
Summary:	LDraw LEGO CAD renderer
Url:		http://ldglite.sourceforge.net/
Group:		Productivity/Graphics/CAD
Source:		ldglite___UPSTREAM_VERSION__.orig.tar.gz
BuildRequires:  ldraw-library >= 2014.02.1
BuildRequires:	freeglut-devel libpng-devel Mesa-devel gcc-c++
Requires:       ldraw-library >= 2014.02.1
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Provides:	ldraw-renderer
__PATCHES_DECLARE__

%description
ldglite is a renderer of LDraw LEGO models.

LDraw is an open standard for LEGO CAD programs that allow the user to create
virtual LEGO models and scenes.

%prep
%setup -q

__PATCHES_APPLY__

%build
make -f makefile.linux ldglite

%install
install -d %{buildroot}/%{_bindir}
install -m 755 ldglite %{buildroot}/%{_bindir}/ldglite.bin
ln -s ldglite.bin %{buildroot}/%{_bindir}/l3glite.bin
ln -s ldglite.bin %{buildroot}/%{_bindir}/l3gledit.bin
ln -s %{_libexecdir}/ldraw-wrapper %{buildroot}/%{_bindir}/ldglite
ln -s %{_libexecdir}/ldraw-wrapper %{buildroot}/%{_bindir}/l3glite
ln -s %{_libexecdir}/ldraw-wrapper %{buildroot}/%{_bindir}/l3gledit


%files
%defattr(-,root,root)
%{_bindir}/ldglite
%{_bindir}/ldglite.bin
%{_bindir}/l3glite
%{_bindir}/l3glite.bin
%{_bindir}/l3gledit
%{_bindir}/l3gledit.bin

%changelog
