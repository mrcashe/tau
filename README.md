# TAU Library (README)

**tau** is an open source cross-platform library for creating
graphical user interfaces (GUI) in C++ programming language

### Features

-   Pure C++ design
-   Uses C++14 standard
-   Uses exceptions for error handling
-   A small number of external dependencies
-   Set of widgets for GUI building
-   Internationalization support
-   Built-in font engine
-   Cross-platform file and directory management functions and classes
-   Path and URI management functions
-   Classes and functions for UTF-8 encoding management
-   Built-in pixmap (bitmap) managing classes and functions provides import from
    BMP, ICO, ANI, XPM, CUR, Xcursor graphics formats
-   Import from PNG graphics format done with using of libpng library
-   Built-in signal/slot support for event handling (derived from libsigc++-2.0)
-   The building system is [**GNU Make**](https://www.gnu.org/software/make/).

### Host platforms

These are platforms on which the generation of target files is possible

-   Linux
-   FreeBSD

### Target platforms

These are platforms for which file generation is possible

-   Linux
-   FreeBSD
-   Windows (using M cross-platform environment ([**MXE**](https://mxe.cc))).
    MXE uses [**MinGW-w64**](https://www.mingw-w64.org) toolchain. Supported MXE
    targets at the moment are:
    +   i686-w64-mingw32.static (32 bit)
    +   x86_64-w64-mingw32.static (64 bit)
    +   i686-w64-mingw32.shared (32 bit)
    +   x86_64-w64-mingw32.shared (64 bit)

### Generating output

-   Static library for the host system (optional).
-   Shared library for the host system (default), but generation can be switched off.
-   `.pc` files for `pkg-config(1)` utility.
-   Demonstration executable binary files linked against any of generated
    shared or static library.
-   Static library for supported MXE targets (optional).
-   C++ header files (optional).
-   [**Doxygen**](https://doxygen.nl/) documentation in HTML format (optional).

### API versioning

**tau** uses classical three-component versioning scheme: *Major_.Minor_.Micro_*.
I use underscore character ('_') here because library source code declares that
variables this manner. The actual version number can be obtained from *VERSION* file
in project's root directory.

---

## Getting Started

This section provides information only for a quick start,
more detailed information can be found in the documentation
generated by Doxygen.

#### Tools used during build process

It would be a good idea to check the availability of the
necessary software tools before downloading code.
Most of that tools are part of the base system installation,
but some can be missing on your system.

##### Programs from the Unix base system

Must present if you have working system. Presence of the following
programs is mandatory or build process will fail otherwise.
Note that, the building system does not support native **FreeBSD**
`make` and `csh`, so if you are FreeBSD user, you may require to run:

`[~]$ sudo pkg install gmake bash pkgconf`


- basename
- bash
- cat
- cp
- dirname
- envsubst
- find
- grep
- ln
- ls
- mkdir
- mktemp
- realpath
- rm
- sed
- tr
- uname

##### C++ compiler, binutils and gmake

May not be on your system, so you may require installation of additional packages.

- c++ or g++ or clang++
- ar
- strip
- pkg-config
- gmake

##### Optional programs

Can be missing.

- doxygen

#### Software packages used during build process

Note that, the following packages must have installed development files
(typically, C++ headers). Some Linux distros (like Ubuntu, Mageia) provide
separated packages for runtime and development files. But some others
(like ArchLinux, FreeBSD) provides monolithic packages including both runtime and
development files. In any case, the configure script will inform you if any
package is missing.

-   libstdc++ (or libc++) (All platforms)

    Provided by C++ compiler, so if any of C++ compilers found, *libstdc++* accessible too.

-   libpng (All platforms) >= 1.6.28

    I found it already installed in my FreeBSD distro.
    Named *libpng-dev* on Ubuntu.

-   libinotify (FreeBSD only)

    I found it already installed in my FreeBSD distro.

-   On POSIX platforms:

    + libxkbcommon-x11 >= 0.5.0

        I found it already installed in my FreeBSD distro.
        Named *libxkbcommon-x11-dev* on Ubuntu.

    + libxcb >= 1.11.1

        I found it already installed in my FreeBSD distro.
        Named *libxcb1-dev* on Ubuntu.

    + libxcb-cursor >= 0.1.2

        I found it already installed in my FreeBSD distro.
        Named *libxcb-cursor-dev* on Ubuntu.

    + libxcb-icccm >= 0.4.1

        I found it already installed in my FreeBSD distro.
        Named *libxcb-icccm4-dev* on Ubuntu.

    + libxcb-renderutil >= 0.3.9

        I found it already installed in my FreeBSD distro.
        Named *libxcb-render-util0-dev* on Ubuntu.

    + libxcb-screensaver >= 1.11.1

        I found it already installed in my FreeBSD distro.
        Named *libxcb-screensaver0-dev* on Ubuntu.

    + libxcb-sync >= 1.11.1

        I found it already installed in my FreeBSD distro.
        Named *libxcb-sync-dev* on Ubuntu.

    + libxcb-xfixes >= 1.11.1

        I found it already installed in my FreeBSD distro.
        Named *libxcb-xfixes0-dev* on Ubuntu.

> Also, on Linux, the header **inotify.h** must be accessible. Since it is part
> of the **Glibc** library, there should be no problems with its availability,
> however, the installation of the Linux Kernel headers may be required.

### Download
Download can be done from the [**GitHub**](https://github.com/) repository using
`git` command. Therefore, **git** should be installed on your system.

Choose directory where to clone remote directory and type in the console:

`[~]$ git clone https://github.com/mrcashe/tau.git`


As a result, the directory named '**tau**' will appear.

#### Source Tree

The typical layout of downloaded directory looks similar to:

~~~
doc/
share/
src/
sup/
AUTHORS
configure
COPYING
Makefile
README.md
VERSION
~~~

This is so-called **Source Tree**.
The most important files for you are *Makefile* and *configure*.

#### Build Tree

The **Build Tree** consists of several subdirectories:

-   *conf/* is a result of configuration process and holds control data for build process.
-   *build/* where object and library files collected during build process.
-   *bin/* is a place where demonstration and test/demo binary executable files (a result of build process) will be located.

The Build Tree is fully independent of Source Tree and may be located at any place within file system.
The Source Tree doesn't modified during Configure, Build or Install processed.

## Configure

Despite the fact that the library does not use the **GNU Autotools**,
the **configure** script is provided. In contrast to *automake's* generated
scripts, this script is hand-made.

At this point, you can run `configure` script with default arguments:
~~~
[~]$ ./configure
~~~

As a result, the subdirectory named *./conf* must be created and filled within files.
Alternatively, you can run `make` (or `gmake`) without any arguments:
~~~
[~]$ gmake
~~~

`gmake` will run `configure` by itself, because subdirectory ./conf does not exists
and will start building process immediately.

You can also use configure options, the brief list is:
-   *--prefix*=PREFIX the install prefix where built files will be copied.
-   *--enable-static* enable static library building.

One more feature of `configure` script is it can be called from any place but not only
from project's root directory. Suppose, you downloaded source into ~/build/tau/ directory
and do not want to build there. You can make another directory, say ~/build/tau-build/,
`cd` to it and enter:

~~~
[~]$ ../tau/configure
~~~

...and build process will happen within `~/build/tau-build/` directory. Same way, you may
specify an absolute path to the `configure` script:

~~~
[~]$ /home/user/build/tau/configure
~~~

...and the result will be the same. This way you can build library with unlimited count of
different configurations. You even can write protect the source tree:

~~~
[~]$ chmod 0500 ~/build/tau
~~~

## Make

The standard make targets are provided:

-   **all** means usual action: to build everything that enabled;
-   **clean** also has obvious meaning: remove all built object and library files;
-   **install** install enabled components using defined install ***PREFIX***;
-   **uninstall** uninstall previously installed package using defined install ***PREFIX***.

And one more non-standard is:

-   **rm** remove entire **Build Tree** completely, the `configure` call needed to resume build process.

> The complete list of all available targets will be published in Doxygen documentation.

To build the library, run `gmake` without arguments or run `gmake all`. The building
process will start, the generated files will be placed into `./build/` subdirectory
and built demo programs will be placed into `./bin/` subdirectory.

## Install

After `gmake` succeed, you may install generated files using `gmake install` command.
The default install ***PREFIX*** is `/usr/local` and you should use `sudo` or `su`
command to install files. Good alternative is specify ***$HOME*** variable as
***PREFIX*** during configure stage, but you should to setup some environment
variables in your `~/.profile`, `~/.bash_profile` or `~/.csh_profile` file,
such as ***PATH***, ***LD_LIBRARY_PATH*** and ***PKG_CONFIG_PATH***.

#### What will be installed and where

-   Shared library (and static, if enabled, too) will be installed
    into ***PREFIX***/lib/ subdirectory.
-   `pkg-config` file(s) will be installed into ***PREFIX***/$(libdata)/pkgconfig/
    subdirectory, where $(libdata) is *lib* for **Linux** and *libdata* for **FreeBSD**.
-   C++ header files will be installed into ***PREFIX***/include/tau-Major_.Minor_/
    subdirectory, where *Major_* and *Minor_* are 1st and 2nd API version
    components, see above section about API versioning.
-   Binary executable files will be installed into ***PREFIX***/bin/
    subdirectory.
-   Various data files will be installed into ***PREFIX***/share/tau-Major_.Minor_/
    subdirectory, where *Major_* and *Minor_* are 1st and 2nd API version
    components, see above section about API versioning.

> Despite libtau has builtin support for [XDG Icon Themes](https://www.freedesktop.org/wiki/Specifications/icon-theme-spec/),
> SVG graphics format not yet realized in library, so I have to bundle fallback
> icon theme derived from Oxygen icon theme that consists of PNG files.
> Its name is 'Tau' and if you choose `/usr/local` or `/usr` as ***PREFIX***
> directory, that theme may be shown by other programs.

## Using without Install

If you do not planning to use built libraries, you may not install the package,
all binaries are capable to run from the `./bin/` subdirectory.
> If you didn't run `make install` and using separate build directory, the
> fallback icon theme will not be accessible by test applications so you will
> see the black boxes on the buttons instead of icons. To resolve this, you
> can create symbolic link to the `./share/` subdirectory within Source Tree manually:
> `ln -s /path/to/tau/share share`.

## Using Built Libraries

To have access to the generated `.pc` file(s), you should run `gmake install`.
The manual intervention may be required to update `ldconfig(8)` database. Run
`sudo /sbin/ldconfig` after package install if you enabled shared library
building.

The `pkg-config` package name for tau library is ***tau-Major_.Minor_***. Here is a
sample how to use pkg-config to link against shared *libtau*.

~~~
# Makefile
# Using only first two version components.
tau_version = 0.3
tau_pc = tau-$(tau_version)
CXXFLAGS += $(shell pkg-config --cflags $(tau_pc))
LDFLAGS += $(shell pkg-config --libs $(tau_pc))
~~~

For static library, pkg-config file provides variable named *'a'*.
Here is an example how to link against static library.

~~~
# Makefile
# Using only first two version components.
tau_version = 0.3
tau_pc = tau-$(tau_version)
CXXFLAGS += $(shell pkg-config --cflags $(tau_pc))
tau_static = $(shell pkg-config --variable=a $(tau_pc))
~~~

