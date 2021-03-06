Motivation
==========

The problem we are trying to solve is very specific to our needs. We use RPM to deploy software, and during development there are a couple different of environments to which a package should be installed on. Currently changing config files is a tedious and error prone process. This softwares aims to provide a cheap solution to this problem.

1 Minute Tutorial
=================

This software provides a simple templating system that can be used to customize configuration files. For instance, suppose you have a package, say *foobar*, which installs a config file under */etc/foobar/file.conf* [#]_ with the following content::

  hostname: $(hostname)
  chroot: $(foobar::chroot)

The file above defined two variables, which are enclosed between `$(` and `)` characters, namely `$(hostname)` and `$(foobar::chroot)`. The former uses the package name implicitly while the later uses the package name explicitly, allowing you to refer to variables in different packages. The first time this package is installed, these variables will probably be undefined. To define them::

  $ rpmctl put -n foobar -k hostname -v 127.0.0.1
  $ rpmctl put -n foobar -k chroot -v /var/jails/0

This will define these two variables, which will be persisted into a local database. Invoking the command *apply* should expand this variables, replacing them by the values you have just defined::

  $ rpmctl apply -n foobar -f /etc/foobar/file.conf
  # alternatively:
  $ rpmctl apply -p /path/to/foobar.rpm -f /etc/foobar/file.conf

The difference between the `-n` and `-p` switches is that the later reads the file directly from the RPM package, using the name declared in the RPM as the default namespace.

The config file should now look like this::

  $ cat /etc/foobar/file.conf
  hostname: 127.0.0.1
  chroot: /var/jails/0

You may do these steps again (*put* + *apply*) and the new variables will be redefined.

There are more commands available, other than *put* and *apply*, but this should give you a good overview of what it does.

Dependencies
============

+--------------+------------------------+
| *dependency* | *min (tested) version* |
+==============+========================+
| icu          | 3.6                    |
+--------------+------------------------+
| popt         | 1.10.2.3               |
+--------------+------------------------+
| libarchive   | 2.8.4                  |
+--------------+------------------------+
| db_cxx       | 4.3.29                 |
+--------------+------------------------+
| rpm          | 4.4.2.3                |
+--------------+------------------------+

Testing
-------

  The same as the main program plus:

+------------------+------------------------+
| *dependency*     | *min (tested) version* |
+==================+========================+
| boost filesystem | 1.42                   |
+------------------+------------------------+
| unittest++       | 1.4.0                  |
+------------------+------------------------+
        
Compiling
=========

::

  $ make build

If using an old version of `librpm (<=4.4)`, perform as follows::

  $ make CPPFLAGS=-D_RPM_4_4_COMPAT build

Binaries will be available under `dist/bin` directory.

Testing
=======

::

  $ make tests

Alternatively you may invoke::

  $ make coverage

And code coverage (using gcov + lcov) data, in HTML format, will available at dist/coverage directory.

TODO
====

  * write the man page;
  * write a YUM plugin, for 2 reasons:
    1. make it easier to use as YUM is usually available in rhel machines;
    2. to work around the fact that the RPM database contains only headers. By
       writing such a plugin, rpmctl might get access to the payload, without
       the need to cache the file locally;
  * change parser interface to use a data source instead of receiving a file
    directly: this is required so we can parse the RPM config while extracting
    it from the package;
  * multiple keys in put/get commands;
  * support for rpm relocation feature;
  * fix the compatibility problems with big/little endian architectures;

References
==========

This has been heavily inspired by *yinst*, the official Yahoo! package manager [as of early 2011]. This functionality is built-in into the tool, which is something I considered to be a very useful featured for a package manager to have.

.. [#] Bear in mind that you can use this in any *text file* owned by a RPM package.
