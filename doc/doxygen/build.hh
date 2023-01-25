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

/**

@page Build
The @b tau build system allows to build several targets using single `gmake` invocation.
Up to six targets can be allowed by either `configure` script or `gmake` itself. The 3 targets
for host system and the same 3 targets for cross-compile using M Cross Environment (MXE):
- build shared library (default)
- build static library (optional)
- build test suite (optional)

For addition, development option can be enabled or disabled either by `configure` or `gmake`.
The development option includes:
- C++ header files installation/deinstallation.
- pkg-config *.pc files installation/deinstallation.
- Doxygen documentation can be switched on/off independently.

@section configure_sect Configure
Detailed information about configure process

@note the relative paths looks like `./something..` originated from the project's root directory

`configure` script is a hand-written `bash(1)` script that generates configuration files used by
@ref make_sect "Make", @ref install_sect "Install" and @ref uninstall_sect "Uninstall" processes.
The root project directory contains a bootstrap script named `./configure` that calls real script
`./sup/configure.bash` which really works. Such a two-level design is dictated by the need to
launch the script on the FreeBSD system, where default shell is `csh` but not `bash`, so up level
script which capable to run under both shells, calls `bash` on either Linux or FreeBSD.

The generated configuration files collected within `./conf` subdirectory.

@subsection configure_opts Configure script options summary
<table>
<tr><th>Option                                 <th>Meaning
<tr><td><em>\--help, -h                   </em><td>show help message and exit
<tr><td><em>\--prefix=\<PREFIX\>          </em><td>@ref prefix_subsect "set install prefix"
<tr><td><em>\--conf-targets=\<TARGETS\>   </em><td>@ref post_subsect "set post-configure targets"
<tr><td><em>\--mxe-prefix=\<PREFIX\>      </em><td>@ref mxepfx_subsect "set MXE prefix"
<tr><td><em>\--mxe-target=\<TARGET\>      </em><td>@ref mxetrg_subsect "set MXE target"
<tr><td><em>\--disable-mxe                </em><td>@ref mxedis_subsect "disable MXE building"
<tr><td><em>\--disable-doc                </em><td>@ref docdis_subsect "disable documentation generation"
<tr><td><em>\--enable-static              </em><td>@ref ena_subsect "enable static library building"
<tr><td><em>\--enable-test                </em><td>@ref entest_subsect "enable test suite building"
<tr><td><em>\--enable-devel               </em><td>@ref endevel_subsect "enable development files creation and install"
</table>

@subsection prefix_subsect \--prefix=\<PREFIX\>
Set install prefix

By default, the install prefix is `/usr/local` which is traditional for Unix systems. Using
this options, you can redefine the prefix. The value of prefix used by @ref install_sect "Install"
and @ref uninstall_sect "Uninstall" stages. The program code does not keep this value and do
not use it.

@subsection post_subsect \--conf-targets=\<TARGETS\>
Set post-configure targets

This is convenience options helps to automate post-configuration behaviour of `gmake`.
Targets, mentioned within this option, will be automatically built without user intervention.
For example, I added targets for test suite building (which is disabled by default) and
sometimes use targets enabling static library generation. For more information see
@ref make_targets "make targets".

@subsection mxepfx_subsect \--mxe-prefix=\<PREFIX\>
Set MXE prefix

This option sets so-call MXE prefix, which is not an install prefix but prefix where
MXE itself installed. Actually, the `configure` can automatically detect this prefix,
but only when MXE binaries can be found using @b PATH environment variable. But if
@b PATH is not pointing to MXE binaries, using this option is required.

@subsection mxetrg_subsect \--mxe-target=\<TARGET\>
Set MXE target

The default target, provided by MXE package, is 'i686-w64-mingw32.static'. To enable
another target, additional actions are required, so @b tau also uses above target as
default. If you configured MXE installation for using other targets than default one,
you may specify desired target using this option. The complete list of all
available targets, provided by M Cross Environment, are follows:
- i686-w64-mingw32.static (32-bit, static additional libraries used)
- i686-w64-mingw32.shared (32-bit, shared additional libraries used)
- x86_64-w64-mingw32.static (64-bit, static additional libraries used)
- x86_64-w64-mingw32.shared (64-bit, shared additional libraries used)

@subsection mxedis_subsect \--disable-mxe
Disable MXE building

This options defeat MXE autodetection and completely disables MXE building.

@subsection docdis_subsect \--disable-doc
Disable documentation generation

As mentioned above, generating of Doxygen documentation can be switched on/off.
The default behaviour of both `configure` and `gmake` is to autodetect Doxygen
existance and enable documentation generation in case user enables development
option. The --disable-doc option completely disables doxygen usage.

@subsection ena_subsect \--enable-static
Enable static library building

By default, the static library building is disabled. Despite that fact that
`gmake` capable itself to enable/disable static library building, this option
also can be useful.

@note This option enables static library building for both @b host and @b MXE targets
(if MXE found and enabled).

@subsection entest_subsect \--enable-test
Enable test suite building

By default, the test suite building is disabled. The test suite consists of several
binary executables (currently 4 files) that used as a test bed and demo programs same time.
@note This option enables both @b host and @b MXE test binaries buikding (if MXE found and
enabled).

@subsection endevel_subsect \--enable-devel
Enable development files creation and install

By default, the development option is switched off. It means, the C++ header files,
pkg-config *.pc files and Doxygen documentation will not be installed. Using this
option, you can switch development option on.

@subsection argv_subsect configure.argv file
`configure` script reads this file on startup before some actions will be performed.

You can fill this file with any above options to automate configure process.
@note The location of `configure.argv` file determined by using @b $PWD environment
variable (see `man 1 pwd`) but not from directory where `configure` is located.

Here is the contents of my `configure.argv` file:
~~~
--prefix=$HOME
--conf-targets=en-host-test-so
--conf-targets=en-mxe-test-so
--enable-devel
~~~

@note here is in the example you can see the `--conf-prefix=` option used twice. This is
because `bash` splits given arguments by spaces and newlines, so if you want to assign more
than one post-configure target, use `--conf-prefix=` option repeatedly with only one argument.

@section make_sect Make
Detailed information about make process

@subsection make_targets Make targets summary

@section install_sect Install
Detailed information about installation process

@section uninstall_sect Uninstall
Detailed information about deinstallation process

**/
