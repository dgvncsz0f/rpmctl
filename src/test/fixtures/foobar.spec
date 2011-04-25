Summary: rpmctl testing file
Name: foobar
Version: 1.0
Release: 1
License: GPL
Group: Developer/Testing
Packager: dsouza

%description
A package that contains 3 config files.

%prep

%build

%install
install -d -m 0755 $RPM_BUILD_ROOT/etc/foobar
printf "\$(cfg1)" >$RPM_BUILD_ROOT/etc/foobar/cfg1
printf "\$(cfg2)" >$RPM_BUILD_ROOT/etc/foobar/cfg2
printf "\$(cfg3)" >$RPM_BUILD_ROOT/etc/foobar/cfg3

%files
%config /etc/foobar/cfg1
%config /etc/foobar/cfg2
%config /etc/foobar/cfg3
