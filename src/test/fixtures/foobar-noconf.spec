Summary: rpmctl testing file without config files
Name: foobar-noconf
Version: 1.0
Release: 1
License: GPL
Group: Developer/Testing
Packager: dsouza

%description
A package that contains 0 config files.

%prep

%build

%install
install -d -m 0755 $RPM_BUILD_ROOT/etc/foobar
printf "\$(cfg1)" >$RPM_BUILD_ROOT/etc/foobar/cfg1
printf "\$(cfg2)" >$RPM_BUILD_ROOT/etc/foobar/cfg2
printf "\$(cfg3)" >$RPM_BUILD_ROOT/etc/foobar/cfg3

%files
/etc/foobar/cfg1
/etc/foobar/cfg2
/etc/foobar/cfg3
