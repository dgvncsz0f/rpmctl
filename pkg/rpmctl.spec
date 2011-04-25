%define name         rpmctl
%define version      %(sh ../src/sh/read_version ../src/main/rpmctl/config.hh)
%define source       %{_topdir}/SOURCES/%{name}-%{version}.tar
%define buildroot    %{_tmppath}/%{name}-%{version}

BuildRoot:           %{buildroot}
Name:                %{name}
Summary:             Template system for RPM config files
Version:             %{version}
Release:             1
License:             GPL
Group:               System Environment/Ext
Packager:            dsouza at bitforest.org
Source:              %{source}

%description 
This software aims to provide a minimal templating system for RPM
[config] files. The ideia to to able to change configuration options
without editing the file directly, which might be error-prone.

%prep
%(cd ..; git archive --prefix=%{name}-%{version}/ master >"%{source}")
%setup -q

%build
make CPPFLAGS="$CPPFLAGS -g" CXXFLAGS=-O2 build

%install
install -d -m 0755 %{buildroot}/usr/local/bin
install -d -m 0700 %{buildroot}/var/lib/rpmctl
install -m 0755 dist/bin/rpmctl %{buildroot}/usr/local/bin/rpmctl

%files
%doc README.rst CHANGELOG
%attr(0755,root,root) /usr/local/bin/rpmctl
%attr(0700,root,root) /var/lib/rpmctl
