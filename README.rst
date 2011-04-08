Motivation
==========

The problem we are trying to solve is very specific to our needs. We use RPM to deploy software, and during development there are a couple different of environments to which a package should be installed on. Currently changing config files is a tedious and error prone process. This softwares aims to provide a cheap solution to this problem.

1 Minute Tutorial
=================

!!! N.B.: This is a future spec. Currently this interface does not exist. !!!

All actions are done through a program called *rpmctl*. This software provides functionality to read/write variables and to apply them to config files. For instance, suppose you have a package, say *foobar*, which installs a config file under */etc/foobar/file.conf* with the following content:

  hostname: $(hostname)
  chroot: $(foobar::chroot)

Keep in mind that this file is just an example. It could be an *Apache* config file, a *XML*, *YAML* or any other text file you might have.

The file above defined two variables, which are enclosed between `$(` and `)` characters, namely `$(hostname)` and `$(foobar::chroot)`. The former uses the package name implicitly while the later uses the package name explicitly, allowing you to refer to variables in different packages. The first time this package is installed, these variables will probably be undefined. To define them::

  $ rpmctl set foobar::hostname 127.0.0.1
  $ rpmctl set foobar::chroot /var/jails/0

This will define these two variables, which will be persisted into a local database. Invoking the command *apply* should expand this variables, replacing them by the values you have just defined:

  $ rpmctl apply foobar

The config file should now look like something like this:

  $ cat /etc/foobar/file.conf
  hostname: 127.0.0.1
  chroot: /var/jails/0

You may do these steps again (*set* + *apply*) and the new variables will be redefined. Rpmctl accomplishes this by reading the default config file from the package before applying the changes.

There are more commands available, other than *set* and *apply*, but this should give a good overview of what it does.

Dependencies
============

  * icu [http://site.icu-project.org/]
  * popt [http://rpm5.org/]
  * Berkeley DB [http://www.oracle.com/technetwork/database/berkeleydb/overview/index.html]

Testing
-------
  The same as the main program plus:

  * boost_filesystem [http://www.boost.org/doc/libs/]
  * unittest++ [http://unittest-cpp.sourceforge.net/]

Compiling
=========

  $ make build

Binaries will be available under `dist/bin` directory.

Testing
=======

  $ make tests

TODO
====

  * finish the *RPM* integration;
  * provide the *CLI* as stated previously;
  * fix the compatibility problems with big/little endian architectures;

References
==========

This has been heavily inspired by *yinst*, the official Yahoo! package manager [as of early 2011]. This functionality is built-in into the tool, which is something I considered to be a very useful featured for a package manager to have.
