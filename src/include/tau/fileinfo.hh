// ----------------------------------------------------------------------------
// Copyright © 2014-2023 Konstantin Shmelkov <mrcashe@gmail.com>.
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

#ifndef TAU_FILEINFO_HH
#define TAU_FILEINFO_HH

#include <tau/types.hh>
#include <tau/signal.hh>
#include <tau/timeval.hh>

/// @file fileinfo.hh Fileinfo (file information) class.

namespace tau {

/// Information about certain file.
///
/// @note This class is a wrapper around its implementation shared pointer.
///
/// @ingroup file_group
class Fileinfo {
public:

    /// Default constructor.
    Fileinfo();

    /// Copy constructor.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Fileinfo(const Fileinfo & other) = default;

    /// Copy operator.
    ///
    /// @note This class is a wrapper around its implementation shared pointer,
    /// so copying it just increasing implementation pointer use count, but isn't
    /// really copies the object. The underlying implementation is not copyable.
    Fileinfo & operator=(const Fileinfo & other) = default;

    /// Constructor with path.
    Fileinfo(const ustring & uri);

    /// Test if file exists.
    bool exists() const;

    /// Test is directory.
    bool is_dir() const;

    /// Test is symbolic link.
    bool is_link() const;

    /// Test is regular.
    bool is_regular() const;

    /// Test if character device file.
    bool is_char() const;

    /// Test if block device file.
    bool is_block() const;

    /// Test if pipe.
    bool is_fifo() const;

    /// Test if socket.
    bool is_socket() const;

    /// Test if executable.
    bool is_exec() const;

    /// Test if hidden.
    bool is_hidden() const;

    /// Test if removable.
    bool is_removable() const;

    /// Get file size in bytes.
    uintmax_t bytes() const;

    /// Get file last access time.
    Timeval atime() const;

    /// Get file creation time.
    Timeval ctime() const;

    /// Get file last modification time.
    Timeval mtime() const;

    /// Remove file (unlink).
    /// @param opts the options (reserved but not yet impelmented).
    /// @param slot_async if not pure, remove file asynchronously (reserved but not yet implemented).
    /// @throw sys_error in case of operating system error.
    void rm(int opts=0, slot<void(int)> slot_async=slot<void(int)>());

    /// Signal watch.
    /// Used for file event monitoring.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// void on_watch(int event_mask, const ustring & path);
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// The meaning of @b event_mask bits can be found in
    /// #File_flags enum (enums.hh header file).
    /// @param event_mask same meaning as above.
    signal<void(int, const ustring &)> & signal_watch(int event_mask);

private:

    Fileinfo_ptr impl;
};

} // namespace tau

#endif // TAU_FILEINFO_HH
