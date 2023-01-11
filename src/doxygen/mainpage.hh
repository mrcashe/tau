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
/// libtau is an open source cross-platform library for creating
/// graphical user interfaces (GUI) in C++ programming language
/// ### AUTHORS ###
/// @include AUTHORS
///
/// ### COPYING ###
/// The 2-Clause BSD License (https://opensource.org/licenses/BSD-2-Clause)
///
/// @include COPYING
///
/// ### Features ###
/// - Pure C++ design
/// - C++14 standard is used
/// - Wide usage of Standard Template Library (STL)
/// - Exceptions used for error handling
/// - A small number of external dependencies
/// - Set of widgets for GUI building
/// - Internationalization tools
/// - Built-in font engine
/// - Cross-platform file and directory management functions and classes
/// - Path and URI management functions
/// - Classes and functions for UTF-8 encoding management
/// - Built-in signal/slot support for event handling
///
/// ### Supported platforms ###
/// - Linux
/// - FreeBSD
/// - Windows 32-bit (using MXE cross-platform environment cross compiler)
///
/// ### Mandatory dependencies ###
/// - libstdc++ (All platforms)
/// - libpng (All platforms)
/// - gettext (All platforms)
/// - libinotify (FreeBSD)
/// - On POSIX platforms:
///   + libxkbcommon-x11
///   + libxcb
///   + libxcb-cursor
///   + libxcb-icccm
///   + libxcb-renderutil
///   + libxcb-screensaver
///   + libxcb-sync
///   + libxcb-xfixes
