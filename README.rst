=============================================================
 rpmctl: extremely lightweight configuration management tool
=============================================================

Motivation
==========

The problem we are trying to solve is very specific to our needs. We use RPM to deploy software, and during development there are a couple different of environments to which a package should be installed on. Currently changing config files is a tedious and error prone process. This softwares aims to provide a cheap solution to this problem.

1 Minute Tutorial
=================

!!! N.B.: This is a future spec. Currently this interface does not exist. !!!

All actions are done through a program called _rpmctl_. This software provides functionality to read/write variables and to apply them to config files. For instance, suppose you have a package, say *foobar*, which installs a config file under _/etc/foobar/file.conf_ with the following content:

  hostname: $(hostname)
  chroot: $(chroot)

Keep in mind that this file is just an example. It could be an _Apache_ config file, a _XML_, _YAML_ or any other text file you might have.

The file above defined two variables, namely `$(hostname)` and `$(chroot)`. Variables are enclosed by `$(` and `)` characters. The first time this package is installed, these variables will probably be undefined. To define them::

  $ rpmctl set foobar.hostname 127.0.0.1
  $ rpmctl set foobar.chroot /var/jails/0

This will define these two variables, which will be persisted into a local database. Invoking the command _apply_ should expand this variables, replacing them by the values you have just defined:

  $ rpmctl apply foobar

The config file should now look like something like this:

  $ cat /etc/foobar/file.conf
  hostname: 127.0.0.1
  chroot: /var/jails/0

You may do these steps again (_set_ + _apply_) and the new variables will be redefined. Rpmctl accomplishes this by reading the default config file from the package before applying the changes.

There are more commands available, other than _set_ and _apply_, but this should give a good overview of what it does.

TODO
====

  * finish the _RPM_ integration;
  * provide the _CLI_ mentioned previously;
  * fix the compatibility problems with big/little endian architectures;

References
==========

This has been heavily inspired by _yinst_, the official Yahoo! package manager (as far as I know). This functionality is built-in into the tool.
