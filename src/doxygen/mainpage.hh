// ----------------------------------------------------------------------------
// Copyright © 2014-2022 Konstantin Shmelkov <mrcashe@gmail.com>.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/// @mainpage TAU Library
///
/// @b tau is an open source cross-platform library for creating
/// graphical user interfaces (GUI) in C++ programming language
///
/// ### AUTHORS
///
/// @include AUTHORS
///
/// ### COPYING
///
/// The 2-Clause BSD License (https://opensource.org/licenses/BSD-2-Clause)
///
/// @include COPYING
///
/// ### Features
///
/// - Pure C++ design
/// - C++14 standard is used
/// - Exceptions used for error handling
/// - A small number of external dependencies
/// - Set of widgets for GUI building
/// - Internationalization support
/// - Built-in font engine
/// - Cross-platform file and directory management functions and classes
/// - Path and URI management functions
/// - Classes and functions for UTF-8 encoding management
/// - Built-in signal/slot support for event handling
/// - The building system is [**GNU Make**](https://www.gnu.org/software/make/).
///
/// ### Host platforms
///
/// These are platforms on which the generation of target files is possible
///
/// -   Linux
/// -   FreeBSD
///
/// ### Target platforms
/// 
/// These are platforms for which file generation is possible
///
/// -   Linux
/// -   FreeBSD
/// -   Windows (using M cross-platform environment ([**MXE**](https://mxe.cc)).
///     MXE uses [**MinGW-w64**](https://www.mingw-w64.org) toolchain. Supported MXE
///     targets at the moment are:
///
///     +   i686-w64-mingw32.static (32 bit)
///
/// ### Generating output
/// -   Static library for the host system @a (optional).
/// -   Shared library for the host system @b (default)*,
///     but generation can be switched off.
/// -   @a .pc files for pkg-config(1) utility.
/// -   Demonstration executable files linked against any of generated
///     shared or static library.
/// -   Static library for supported MXE targets @a (optional).
/// -   C++ header files @a (optional).
/// -   [**Doxygen**](https://doxygen.nl/) documentation in HTML format @a (optional).
///
/// ### API versioning
/// 
/// **tau** uses classical three-component versioning scheme: @a Major_.Minor_.Micro_.
/// I use underscrore character '_' here because library source code declares that
/// variables this manner: see tau::Major_, tau::Minor_ and tau::Micro_. 
/// The actual version number can be obtained from @a VERSION file in project's root directory.
/// 
/// ---
///
/// ## Getting Started
///
/// This section provides information only for a quick start.
///
/// #### Tools used during build process
///
/// It would be a good idea to check the availability of the
/// necessary software tools before downloading code.
/// Most of that tools are part of the base system installation,
/// but some can be missing on your system.
///
/// ##### Programs from the Unix basesystem
///
/// Must present if you have working system. Presence of the following
/// programs is mandatory or build process will fail otherwise.
/// Note that, the building system does not support native @b FreeBSD
/// `make` and `csh`, so if you are FreeBSD user, you may require to run:
///
/// ~~~
/// [~]$ sudo pkg install gmake bash pkgconf
/// ~~~
///
/// - basename
/// - bash
/// - cat
/// - cmp
/// - cp
/// - dirname
/// - envsubst
/// - find
/// - ln
/// - ls
/// - mkdir
/// - mktemp
/// - realpath
/// - rm
/// - sed
/// - tr
/// - uname
///
/// ##### C++ compiler, binutils and gmake
/// 
/// May not be on your system, so you may require installation of additional packages.
/// 
/// - c++ or g++ or clang++
/// - ar
/// - strip
/// - pkg-config
/// - gmake
/// 
/// ##### Optional programs
/// 
/// Can be missing.
/// 
/// - doxygen
///
/// #### Software packages used during build process
/// 
/// Note that, the following packages must have installed development files
/// (typically, C++ headers). Some Linux distros (like Ubuntu, Mageia) provides
/// separated packages for runtime and development files. But some others
/// (like ArchLinux, FreeBSD) provides monolithic packages including both runtime and
/// development files. In any case, the configure script will inform you if any
/// package is missing.
/// 
/// -   libstdc++ (All platforms)
/// 
///     Provided by C++ compiler, so if any of C++ compilers found, *libstdc++* accessible too.
/// 
/// -   libpng (All platforms)
/// 
///     I found it already installed in my FreeBSD distro.  
///     Named *libpng-dev* on Ubuntu.
/// 
/// -   libinotify (FreeBSD only)
/// 
///     I found it already installed in my FreeBSD distro.
/// 
/// -   On POSIX platforms:
/// 
///     + libxkbcommon-x11
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxkbcommon-x11-dev* on Ubuntu.
/// 
///     + libxcb
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb1-dev* on Ubuntu.
/// 
///     + libxcb-cursor
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb-cursor-dev* on Ubuntu.
/// 
///     + libxcb-icccm
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb-icccm4-dev* on Ubuntu.
/// 
///     + libxcb-renderutil
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb-render-util0-dev* on Ubuntu.
/// 
///     + libxcb-screensaver
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb-screensaver0-dev* on Ubuntu.
/// 
///     + libxcb-sync
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb-sync-dev* on Ubuntu.
/// 
///     + libxcb-xfixes
/// 
///         I found it already installed in my FreeBSD distro.  
///         Named *libxcb-xfixes0-dev* on Ubuntu.
/// 
/// >   Also, on Linux, the header **inotify.h** must be accessible. Since it is part
/// >   of the **Glibc** library, there should be no problems with its availability,
/// >   however, the installation of the Linux Kernel headers may be required.
/// 
/// ## Download
/// Download can be done from the [**GitHub**](https://github.com/) repository using
/// @c git command. Therefore, @b git should be installed on your system.
/// 
/// Choose directory where to clone remote directory and type in the console:
/// 
/// ~~~
/// [~]$ git clone https://github.com/mrcashe/tau.git
/// ~~~
/// 
/// As a result, the directory named @b 'tau' will appear. The typical layout of
/// downloaded directory looks similar to:
/// ~~~
/// doc/
/// share/
/// src/
/// sup/
/// AUTHORS
/// configure
/// COPYING
/// Makefile
/// README.md
/// VERSION
/// ~~~
/// 
/// The most important files for you are *Makefile* and *configure*.
/// 
/// ## Configure
/// 
/// Despite the fact that the library does not use the **GNU Autotools**,
/// the **configure** script is provided. In contrast to *automake's* generated
/// files, this file is hand-made.
/// 
/// At this point, you can run configure script with default arguments:
/// ~~~
/// [~]$ ./configure
/// ~~~
/// 
/// As a result, the subdirectory named @a ./conf must be created and filled with files.
/// Alternatively, you can run `make` (or `gmake`) without any arguments:
/// ~~~
/// [~]$ gmake
/// ~~~
/// 
/// `gmake` will run `configure` by itself, because subdirectory @a ./conf does not exists
/// and will start building process immediately.
/// 
/// You can also use configure options, the brief list is:
/// -   <em>--prefix=<b>PREFIX</b></em> the install prefix where built files will be copied.
/// -   <em>--enable-static</em> enable static library building.
/// 
/// ## Make
/// 
/// To build the library, run `gmake` without arguments or run `gmake all`. The building
/// process will start, the generated files will be placed into @a ./build/ subdirectory
/// and built demo programs will be placed into @a ./bin/ subdirectory.
/// 
/// ## Install
/// 
/// After `gmake` succeed, you may install generated files using `gmake install` command.
/// The default install <em><b>PREFIX</b></em> is @a /usr/local and you should use `sudo` or `su`
/// command to install files. Good alternative is specify <em><b>$HOME</b></em> variable as
/// <em><b>PREFIX</b></em> during configure stage, but you should to setup some environment
/// variables in your <em>~/.profile</em>, <em>~/.bash_profile</em> or  <em>~/.csh_profile</em> file,
/// such as ***PATH***, ***LD_LIBRARY_PATH*** and ***PKG_CONFIG_PATH***.
/// 
/// #### What will be installed and where
/// 
/// -   Shared library (and static, if enabled, too) will be installed
///     into <em><b>PREFIX</b>/lib/</em> subdirectory.
/// -   pkg-config file(s) will be installed into <em><b>PREFIX</b>/lib/pkgconfig/</em>
///     subdirectory.
/// -   C++ header files will be installed into <em><b>PREFIX</b>/include/tau-Major_.Minor_/</em>
///     subdirectory, where @a Major_ and @a Minor_ are 1st and 2nd API version
///     components, see above section about API versioning.
/// -   Binary executable files will be installed into <em><b>PREFIX</b>bin/</em>
///     subdirectory.
/// -   Various data files will be installed into <em><b>PREFIX</b>share/tau-Major_.Minor_/</em>
///     subdirectory, where @a Major_ and @a Minor_ are 1st and 2nd API version
///     components, see above section about API versioning.
/// 
/// > Despite libtau has builtin support for [XDG Icon Themes](https://www.freedesktop.org/wiki/Specifications/icon-theme-spec/),
/// > SVG graphics format not yet realized in library, so I have to bundle fallback
/// > icon theme derived from Oxygen icon theme that consists of PNG files.
/// > Its name is 'tau' and if you choose @a /usr/local or @a /usr as <em><b>PREFIX</b></em>
/// > directory, that theme may be shown by other programs.
/// 
/// ## Do not Install
/// 
/// If you do not planning to use generated libraries, you may not install the package,
/// all binaries are capable to run from the @a ./bin/ subdirectory.
/// 
/// ## Using Generated Libraries
/// 
/// To have access to the generated @a .pc file(s), you should run `gmake install`.
/// The manual intervention may be required to update <tt>ldconfig(8)</tt> database. Run
/// `sudo /sbin/ldconfig` after package install if you enabled shared library building.
/// 
/// The pkg-config package name for tau library is <em><b>tau-Major_.Minor_</b></em>.
/// Here is a sample how to use pkg-config to link against shared @a libtau.
/// 
/// ~~~
/// # Makefile
/// # Using only first two version components.
/// tau_version = 0.3
/// tau_pc = tau-$(tau_version)
/// CXXFLAGS += $(shell pkg-config --cflags $(tau_pc))
/// LDFLAGS += $(shell pkg-config --libs $(tau_pc))
/// ~~~
/// 
/// For static library, pkg-config file provides variable named @a 'a'.
/// Here is an example how to link against static library.
/// 
/// ~~~
/// # Makefile
/// # Using only first two version components.
/// tau_version = 0.3
/// tau_pc = tau-$(tau_version)
/// CXXFLAGS += $(shell pkg-config --cflags $(tau_pc))
/// tau_static = $(shell pkg-config --variable=a $(tau_pc))
/// ~~~
/// 
