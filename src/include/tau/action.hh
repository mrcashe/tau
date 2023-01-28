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

/// @file action.hh Master_action, Action_base, Action and Toggle_action classes.

#ifndef TAU_ACTION_HH
#define TAU_ACTION_HH

#include <tau/types.hh>
#include <tau/accel.hh>
#include <tau/signal.hh>
#include <tau/ustring.hh>
#include <vector>

namespace tau {

/// An action that holds information for other actions.
/// @ingroup action_group
/// @ingroup input_group
/// @sa @ref action_sect
/// @sa @ref kbd_sect
class Master_action: public trackable {
public:

    /// @name Constructors, operators
    /// @{

    /// Default constructor.
    Master_action() = default;

    /// Copy constructor.
    Master_action(const Master_action & other) = default;

    /// Copy operator.
    Master_action & operator=(const Master_action & other) = default;

    /// Constructor with accels.
    /// @param accels space separated list of accelerator specifications.
    /// @overload Master_action(const ustring & accels)
    Master_action(const ustring & accels);

    /// Constructor with accel.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @overload Master_action(char32_t kc, int km)
    Master_action(char32_t kc, int km);

    /// Constructor with accels and label.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @overload Master_action(const ustring & accels, const ustring & label)
    Master_action(const ustring & accels, const ustring & label);

    /// Constructor with accel and label.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @overload Master_action(char32_t kc, int km, const ustring & label)
    Master_action(char32_t kc, int km, const ustring & label);

    /// Constructor with accels, label and icon name.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @overload Master_action(const ustring & accels, const ustring & label, const ustring & icon_name)
    Master_action(const ustring & accels, const ustring & label, const ustring & icon_name);

    /// Constructor with accel, label and icon name.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @overload Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name)
    Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name);

    /// Constructor with accels, label, icon name and tooltip.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @overload Master_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip)
    Master_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip);

    /// Constructor with accel, label, icon name and tooltip.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @overload Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip)
    Master_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip);

    /// @}
    /// Get label.
    ustring label() const;

    /// Set label.
    void set_label(const ustring & label);

    /// Enable action.
    /// %Master_action is enabled (sensitive) by default.
    void enable();

    /// Disable action.
    /// %Master_action is enabled (sensitive) by default.
    void disable();

    /// Determines whether action enabled or not.
    /// %Master_action is enabled (sensitive) by default.
    bool enabled() const;

    /// Show action.
    /// %Master_action is shown by default.
    void show();

    /// Hide action.
    /// %Master_action is shown by default.
    void hide();

    /// Determines whether action visible or not.
    /// %Master_action is visible by default.
    bool visible() const;

    /// Assign an icon name.
    void set_icon_name(const ustring & icon_name);

    /// Get icon name.
    ustring icon_name() const;

    /// Set tooltip.
    void set_tooltip(const ustring & tooltip);

    /// Unset tooltip.
    void unset_tooltip();

    /// Get tooltip.
    ustring tooltip() const;

    /// Add accelerator.
    void add_accel(char32_t kc, int km=0);

    /// Add accelerators.
    /// @param key_specs is a space delimited list of key specifications.
    void add_accels(const ustring & key_specs);

    /// Remove accelerator.
    void remove_accel(char32_t kc, int km=0);

    /// Remove accelerators.
    /// @param key_specs is a space delimited list of key specifications.
    void remove_accels(const ustring & key_specs);

    /// Remove all accelerators.
    void clear_accels();

    /// %List accelerators.
    std::vector<Accel> accels() const;

    /// Signal emitted when action becomes disabled.
    signal<void()> & signal_disable();

    /// Signal emitted when action becomes enabled.
    signal<void()> & signal_enable();

    /// Signal emitted when action becomes hidden.
    signal<void()> & signal_hide();

    /// Signal emitted when action becomes visible using show() method.
    signal<void()> & signal_show();

    /// Signal emitted when new accelerator added using add_accel() method.
    signal<void(const Accel & accel)> & signal_accel_added();

    /// Signal emitted when accelerator removed using remove_accel() method.
    signal<void(const Accel & accel)> & signal_accel_removed();

    /// Signal emitted when label changed using set_label() method.
    signal<void(const ustring &)> & signal_label_changed();

    /// Signal emitted when label changed using set_icon_name() method.
    signal<void(const ustring &)> & signal_icon_changed();

    /// Signal emitted when tooltip changed using one of set_tooltip() methods.
    signal<void(const ustring &)> & signal_tooltip_changed();

private:

    using Accels = std::vector<Accel>;

    ustring     label_;
    ustring     icon_name_;
    ustring     tooltip_;
    bool        visible_ = true;
    bool        enabled_ = true;
    Accels      accels_;

    signal<void()>  signal_disable_;
    signal<void()>  signal_enable_;
    signal<void()>  signal_show_;
    signal<void()>  signal_hide_;

    signal<void(const Accel &)> signal_accel_added_;
    signal<void(const Accel &)> signal_accel_removed_;

    signal<void(const ustring &)> signal_label_changed_;
    signal<void(const ustring &)> signal_icon_changed_;
    signal<void(const ustring &)> signal_tooltip_changed_;
};

/// An action which can be activated in some way by keyboard accelerator,
/// menu item or tool button.
/// %Action provides some information how it should be presented, such as:
/// - %Icon.
/// - Label for menu item.
/// - Tooltip to be shown on tool button.
/// - Set of keyboard accelerators associated with an action.
///
/// %Action can be:
/// - Hidden or shown.
/// - Sensitive or insensitive.
/// - Trigger or toggle.
///
/// %Action also provides set of signals to inform user about happening events.
/// @ingroup action_group
/// @ingroup input_group
/// @sa @ref action_sect
/// @sa @ref kbd_sect
class Action_base: public trackable {
public:

    /// @name Constructors, destructor, operators
    /// @{

    /// Default constructor.
    Action_base() = default;

    /// Copy constructor.
    Action_base(const Action_base & other) = default;

    /// Copy operator.
    Action_base & operator=(const Action_base & other) = default;

    /// Constructor with accels.
    /// @param accels space separated list of accelerator specifications.
    /// @overload Action_base(const ustring & accels)
    Action_base(const ustring & accels);

    /// Constructor with accel.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @overload Action_base(char32_t kc, int km)
    Action_base(char32_t kc, int km);

    /// Constructor with accels and label.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @overload Action_base(const ustring & accels, const ustring & label)
    Action_base(const ustring & accels, const ustring & label);

    /// Constructor with accel and label.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @overload Action_base(char32_t kc, int km, const ustring & label)
    Action_base(char32_t kc, int km, const ustring & label);

    /// Constructor with accels, label and icon name.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @overload Action_base(const ustring & accels, const ustring & label, const ustring & icon_name)
    Action_base(const ustring & accels, const ustring & label, const ustring & icon_name);

    /// Constructor with accel, label and icon name.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @overload Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name)
    Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name);

    /// Constructor with accels, label, icon name and tooltip.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @overload Action_base(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip)
    Action_base(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip);

    /// Constructor with accel, label, icon name and tooltip.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @overload Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip)
    Action_base(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip);

    /// Constructor with master action.
    /// @param master_action the Master_action.
    /// @overload Action_base(Master_action & master_action)
    Action_base(Master_action & master_action);

    /// @private
    virtual ~Action_base();

    /// @}

    /// Enable action.
    /// %Action is enabled (sensitive) by default.
    void enable();

    /// Disable action.
    /// %Action is enabled (sensitive) by default.
    void disable();

    /// Determines whether action enabled or not.
    /// %Action is enabled (sensitive) by default.
    bool enabled() const;

    /// Show action.
    /// %Action is shown by default.
    void show();

    /// Hide action.
    /// %Action is shown by default.
    void hide();

    /// Determines whether action visible or not.
    /// %Action is visible by default.
    bool visible() const;

    /// Connect master action.
    void connect_master_action(Master_action & master_action);

    /// Connect master action with checking pointer to @b nullptr.
    void connect_master_action(Master_action * master_action);

    /// Connect master action by name.
    void connect_master_action(const ustring & name);

    /// Lookup accelerators.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifiers, see #Key_modifiers enum.
    /// @return @b this pointer if action contains such accelerator or @b nullptr if not found.
    Action_base * lookup(char32_t kc, int km);

    /// Get label.
    ustring label() const;

    /// Set label.
    void set_label(const ustring & label);

    /// Assign an icon name.
    void set_icon_name(const ustring & icon_name);

    /// Get icon name.
    ustring icon_name() const;

    /// Set tooltip.
    void set_tooltip(const ustring & tooltip_text);

    /// Unset tooltip.
    void unset_tooltip();

    /// Get tooltip.
    ustring tooltip() const;

    /// Add accelerator.
    void add_accel(char32_t kc, int km=0);

    /// Add accelerators.
    /// @param key_specs is a space delimited list of key specifications.
    void add_accels(const ustring & key_specs);

    /// Remove accelerator.
    void remove_accel(char32_t kc, int km=0);

    /// Remove accelerators.
    /// @param key_specs is a space delimited list of key specifications.
    void remove_accels(const ustring & key_specs);

    /// Remove all accels.
    void clear_accels();

    /// %List accels.
    std::vector<Accel> & accels();

    /// Signal emitted when action becomes disabled.
    signal<void()> & signal_disable();

    /// Signal emitted when action becomes enabled.
    signal<void()> & signal_enable();

    /// Signal emitted when action becomes hidden.
    signal<void()> & signal_hide();

    /// Signal emitted when action becomes visible using show() method.
    signal<void()> & signal_show();

    /// Signal emitted when new accelerator added using add_accel() method.
    signal<void(Accel &)> & signal_accel_added();

    /// Signal emitted when accelerator removed using remove_accel() method.
    /// Slot prototype:
    /// ~~~~~~~~~~~~~~~
    /// void on_accel_removed(char32_t key_code, int key_modifier);
    /// ~~~~~~~~~~~~~~~
    signal<void(Accel &)> & signal_accel_removed();

    /// Signal emitted when label changed using set_label() method.
    signal<void(const ustring &)> & signal_label_changed();

    /// Signal emitted when label changed using set_icon_name() method.
    signal<void(const ustring &)> & signal_icon_changed();

    /// Signal emitted when tooltip changed using one of set_tooltip() methods.
    signal<void(const ustring &)> & signal_tooltip_changed();

    /// Signal emitted when action destroyed.
    signal<void()> & signal_destroy();

protected:

    using Accels = std::vector<Accel>;

    bool                  disabled_ = false;
    bool                  frozen_ = false;
    bool                  hidden_ = false;
    bool                  disappeared_ = false;
    ustring               label_;
    ustring               icon_name_;
    ustring               tooltip_;
    Accels                accels_;

    signal<void()>        signal_disable_;
    signal<void()>        signal_enable_;
    signal<void()>        signal_show_;
    signal<void()>        signal_hide_;
    signal<void(Accel &)> signal_accel_added_;
    signal<void(Accel &)> signal_accel_removed_;
    signal<void(const ustring &)> signal_label_changed_;
    signal<void(const ustring &)> signal_icon_changed_;
    signal<void(const ustring &)> signal_tooltip_changed_;
    signal<void()>        signal_destroy_;

protected:

    virtual bool on_accel() = 0;

private:

    void freeze();
    void thaw();
    void disappear();
    void appear();

    void on_enable();
    void on_disable();
    void on_accel_added(const Accel & accel);
    void on_accel_removed(const Accel & accel);
};

/// A simple event-like action.
/// @ingroup action_group
/// @ingroup input_group
/// @sa @ref action_sect
/// @sa @ref kbd_sect
class Action: public Action_base {
public:

    /// @name Constructors, operators
    /// @{

    /// Default constructor.
    Action() = default;

    /// Copy constructor.
    Action(const Action & other) = default;

    /// Copy operator.
    Action & operator=(const Action & other) = default;

    /// Constructor with slot.
    /// @param slot_activate slot to be connected using connect() method.
    /// @overload Action(slot<void()> slot_activate)
    Action(slot<void()> slot_activate);

    /// Constructor with accels and optional slot.
    /// @param accels space separated list of accelerator specifications.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(const ustring & accels, slot<void()> slot_activate)
    Action(const ustring & accels, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accel and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(char32_t kc, int km, slot<void()> slot_activate)
    Action(char32_t kc, int km, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accels, label and optional slot.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(const ustring & accels, const ustring & label, slot<void()> slot_activate)
    Action(const ustring & accels, const ustring & label, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accel, label and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(char32_t kc, int km, const ustring & label, slot<void()> slot_activate)
    Action(char32_t kc, int km, const ustring & label, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accels, label, icon name and optional slot.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(const ustring & accels, const ustring & label, const ustring & icon_name, slot<void()> slot_activate)
    Action(const ustring & accels, const ustring & label, const ustring & icon_name, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accel, label, icon name and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(char32_t kc, int km, const ustring & label, const ustring & icon_name, slot<void()> slot_activate)
    Action(char32_t kc, int km, const ustring & label, const ustring & icon_name, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accels, label, icon name, tooltip and optional slot.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void()> slot_activate)
    Action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void()> slot_activate=slot<void()>());

    /// Constructor with accel, label, icon name, tooltip and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void()> slot_activate)
    Action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void()> slot_activate=slot<void()>());

    /// Constructor with master action and optional slot.
    /// @param master_action the Master_action.
    /// @param slot_activate [optional] slot to be connected using connect() method.
    /// @overload Action(Master_action & master_action, slot<void()> slot_activate)
    Action(Master_action & master_action, slot<void()> slot_activate=slot<void()>());

    /// @}

    /// Execute the action.
    void exec();

    /// Connect slot.
    connection connect(slot<void()> slot_activate, bool prepend=false);

protected:

    // Overrides pure Action_base.
    bool on_accel() override;

private:

    signal<void()> signal_activate_;
};

/// A toggle action.
/// @ingroup action_group
/// @ingroup input_group
/// @sa @ref action_sect
/// @sa @ref kbd_sect
class Toggle_action: public Action_base {
public:

    /// @name Constructors, operators
    /// @{

    /// Default constructor.
    Toggle_action() = default;

    /// Copy constructor.
    Toggle_action(const Toggle_action & other) = default;

    /// Copy operator.
    Toggle_action & operator=(const Toggle_action & other) = default;

    /// Constructor with slot.
    /// @param slot_toggle slot to be connected using connect() method.
    /// @overload Toggle_action(slot<void(bool)> slot_toggle)
    Toggle_action(slot<void(bool)> slot_toggle);

    /// Constructor with accels and optional slot.
    Toggle_action(const ustring & accels, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accel and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param slot_toggle [optional] slot to be connected using connect() method.
    /// @overload Toggle_action(char32_t kc, int km, slot<void(bool)> slot_toggle)
    Toggle_action(char32_t kc, int km, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accels and label and optional slot.
    Toggle_action(const ustring & accels, const ustring & label, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accel, label and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param slot_toggle [optional] slot to be connected using connect() method.
    /// @overload Toggle_action(char32_t kc, int km, const ustring & label, slot<void(bool)> slot_toggle)
    Toggle_action(char32_t kc, int km, const ustring & label, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accels, label and icon name and optional slot.
    Toggle_action(const ustring & accels, const ustring & label, const ustring & icon_name, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accel, label and icon name and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param slot_toggle [optional] slot to be connected using connect() method.
    /// @overload Toggle_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, slot<void(bool)> slot_toggle)
    Toggle_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accels, label, icon name,tooltip and optional slot.
    /// @param accels space separated list of accelerator specifications.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @param slot_toggle [optional] slot to be connected using connect() method.
    /// @overload Toggle_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void(bool)> slot_toggle)
    Toggle_action(const ustring & accels, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with accel, label, icon name and tooltip and optional slot.
    /// @param kc the key code, see #Key_codes enum.
    /// @param km the key modifier, see #Key_modifiers enum.
    /// @param label the label.
    /// @param icon_name the icon name.
    /// @param tooltip the tooltip.
    /// @param slot_toggle [optional] slot to be connected using connect() method.
    /// @overload Toggle_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void(bool)> slot_toggle)
    Toggle_action(char32_t kc, int km, const ustring & label, const ustring & icon_name, const ustring & tooltip, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// Constructor with master action and optional slot.
    /// @param master_action the Master_action.
    /// @param slot_toggle [optional] slot to be connected using connect() method.
    /// @overload Toggle_action(Master_action & master_action, slot<void(bool)> slot_toggle)
    Toggle_action(Master_action & master_action, slot<void(bool)> slot_toggle=slot<void(bool)>());

    /// @}

    /// Toggle the action.
    void toggle();

    /// Test if toggled (get state).
    bool get() const;

    /// Set state.
    void set(bool state);

    /// Connect slot.
    connection connect(slot<void(bool)> slot_toggle, bool prepend=false);

protected:

    // Overrides pure Action_base.
    bool on_accel() override;

private:

    bool                state_ = false;
    signal<void(bool)>  signal_toggle_;
};

} // namespace tau

#endif // TAU_ACTION_HH
