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

/// @file accel.hh The Accel class.

#ifndef TAU_ACCEL_HH
#define TAU_ACCEL_HH

#include <tau/types.hh>
#include <tau/input.hh>
#include <tau/signal.hh>
#include <tau/ustring.hh>

namespace tau {

/// A keyboard accelerator.
/// @ingroup input_group
/// @sa @ref kbd_sect
class Accel: public trackable {
public:

    /// @name Constructors, operators
    /// @{

    /// Default constructor.
    Accel() = default;

    /// Copy constructor.
    Accel(const Accel & other) = default;

    /// Copy operator.
    Accel & operator=(const Accel & other) = default;

    /// Constructor with key code and key modifier.
    /// @param kc key code, see #Key_codes enum.
    /// @param km key modifier, see #Key_modifiers enum.
    /// @overload Accel(char32_t kc, int km)
    Accel(char32_t kc, int km=KM_NONE);

    /// Constructor with key code, key modifier and slot.
    /// @param kc key code, see #Key_codes enum.
    /// @param km key modifier, see #Key_modifiers enum.
    /// @param slot_activate slot to be connected.
    /// @overload Accel(char32_t kc, int km, slot<bool()> slot_activate)
    Accel(char32_t kc, int km, slot<bool()> slot_activate);

    /// Constructor with string representation.
    /// @param spec the key specification
    /// @overload Accel(const ustring & spec)
    Accel(const ustring & spec);

    /// Constructor with string representation and slot.
    /// @param spec the key specification
    /// @param slot_activate slot to be connected to signal_activated()
    /// @overload Accel(const ustring & spec, slot<bool()> slot_activate)
    Accel(const ustring & spec, slot<bool()> slot_activate);

    /// @}
    /// Test if empty (unassigned).
    bool empty() const;

    /// Test if not empty (assigned).
    operator bool() const;

    /// Compare.
    /// @param kc key code, see #Key_codes enum.
    /// @param km key modifier, see #Key_modifiers enum.
    /// @overload bool equals(char32_t kc, int km) const
    bool equals(char32_t kc, int km) const;

    /// Compare.
    /// @overload bool equals(const ustring & spec) const
    bool equals(const ustring & spec) const;

    /// Compare.
    bool operator==(const Accel & other);

    /// Compare.
    bool operator!=(const Accel & other);

    /// Assign new key code and modifiers.
    /// @param kc key code, see #Key_codes enum.
    /// @param km key modifier, see #Key_modifiers enum.
    /// @overload void assign(char32_t kc, int km)
    void assign(char32_t kc, int km=0);

    /// Set new key code and modifiers from string.
    /// @param spec the key specification.
    /// @overload void assign(const ustring & spec)
    void assign(const ustring & spec);

    /// Gets key code.
    char32_t key_code() const;

    /// Gets key modifier.
    int key_modifier() const;

    /// Gets assigned key codes.
    /// @param[out] kc key code, see #Key_codes enum.
    /// @param[out] km key modifier, see #Key_modifiers enum.
    void get_keys(char32_t & kc, int & km) const;

    /// Gets specification of assigned key.
    /// @sa key_spec_to_string()
    ustring spec() const;

    /// Gets human readable label.
    /// @sa key_spec_to_label()
    ustring label() const;

    /// Enable accelerator.
    void enable();

    /// Disable accelerator.
    void disable();

    /// Determines whether accelerator disabled.
    bool enabled() const;

    /// @private
    bool handle_accel(char32_t kc, int km);

    /// Connect slot to the internal signal.
    connection connect(slot<bool()> slot_activate);

    /// Signal emitted when key value changed.
    /// Slot prototype:
    /// void on_change();
    signal<void()> & signal_changed();

    /// Signal emitted when accelerator becomes enabled.
    /// Slot prototype:
    /// void on_accel_enable();
    signal<void()> & signal_enable();

    /// Signal emitted when accelerator becomes disabled.
    /// Slot prototype:
    /// void on_accel_disable();
    signal<void()> & signal_disable();

private:

    char32_t        kc_ = 0;
    int             km_ = 0;
    bool            disabled_ = false;

    signal<bool()>  signal_activate_;
    signal<void()>  signal_changed_;
    signal<void()>  signal_enable_;
    signal<void()>  signal_disable_;
};

} // namespace tau

#endif // TAU_ACCEL_HH
