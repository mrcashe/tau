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

/// @file signal.hh Signals and slots

#ifndef TAU_SIGNAL_HH
#define TAU_SIGNAL_HH

#include <functional>
#include <list>
#include <memory>
#include <tuple>

namespace tau {

template <typename R, typename... Args> class fun_functor;
template <class Class, typename R, typename... Args> class mem_functor;
template <typename R, typename... Args> class slot;
template <typename R, typename... Args> class signal;
template <class Slot, typename R, typename... Args> struct signal_emitter;
template <typename R, typename... Args> struct slot_impl_T;
template <class functor_type, typename R, typename... Args> struct slot_impl_F;

class trackable;

class functor_base {
    trackable * target_ = nullptr;

public:

    virtual bool empty() const { return true; }
    virtual void reset() { target_ = nullptr; }
    virtual trackable * target() { return target_; }

protected:

    functor_base(trackable * target=nullptr):
        target_(target)
    {
    }
};

template<typename R, typename... Args>
class fun_functor<R(Args...)>: public functor_base {
    using function_type = std::function<R(Args...)>;
    function_type fun_;

public:

    using result_type = R;

    fun_functor(const function_type & fun):
        fun_(fun)
    {
    }

    fun_functor(function_type && fun):
        fun_(std::move(fun))
    {
    }

    R operator()(Args... args) const {
        return fun_ ? fun_(args...) : R();
    }

    bool empty() const override {
        return !fun_;
    }

    void reset() override {
        fun_ = nullptr;
        functor_base::reset();
    }
};

template <bool is_trackable, class Class> class mem_functor_base;

template <class Class>
class mem_functor_base<false, Class>: public functor_base {
protected:

    mem_functor_base(Class * obj) {}
};

template <class Class>
class mem_functor_base<true, Class>: public functor_base {
protected:

    mem_functor_base(Class * obj):
        functor_base(obj)
    {
    }
};

template <class Class, typename R, typename... Args>
class mem_functor<Class, R(Args...)>: public mem_functor_base<std::is_base_of<trackable, Class>::value, Class> {
    using base_type = mem_functor_base<std::is_base_of<trackable, Class>::value, Class>;
    using function_type = std::function<R(Class *, Args...)>;

    Class *         obj_;
    function_type   fun_;

public:

    using result_type = R;

    mem_functor(Class * obj, const function_type & fun):
        base_type(obj),
        obj_(obj),
        fun_(fun)
    {
    }

    mem_functor(Class * obj, function_type && fun):
        base_type(obj),
        obj_(obj),
        fun_(std::move(fun))
    {
    }

    R operator()(Args... args) const {
        return fun_ ? fun_(obj_, args...) : R();
    }

    void reset() override {
        base_type::reset();
        fun_ = nullptr;
    }

    bool empty() const override {
        return !fun_;
    }
};

template <class tuple_type, class functor_type>
class bind_functor: public functor_base {
    using R = typename functor_type::result_type;
    tuple_type   tuple_;
    functor_type functor_;

    template<class Tuple, std::size_t... index>
    R call_functor(Tuple && tuple, std::index_sequence<index...>) const {
        return functor_(std::get<index>(std::move(tuple))...);
    }

public:

    bind_functor(const tuple_type & tuple, const functor_type & functor):
        tuple_(tuple),
        functor_(functor)
    {
    }

    bind_functor(tuple_type && tuple, const functor_type & functor):
        tuple_(std::move(tuple)),
        functor_(functor)
    {
    }

    bind_functor(tuple_type & tuple, functor_type && functor):
        tuple_(tuple),
        functor_(std::move(functor))
    {
    }

    bind_functor(tuple_type && tuple, functor_type && functor):
        tuple_(std::move(tuple)),
        functor_(std::move(functor))
    {
    }

    template <typename... Args>
    R operator()(Args... args) const {
        auto tuple = std::tuple_cat(std::make_tuple(args...), tuple_);
        using index_type = std::make_index_sequence<std::tuple_size<decltype(tuple)>::value>;
        return call_functor(tuple, index_type());
    }

    trackable * target() override {
        return functor_.target();
    }

    bool empty() const override {
        return functor_.empty();
    }

    void reset() override {
        functor_.reset();
    }
};

class signal_base;
class connection;
struct slot_impl;
using slot_ptr = std::shared_ptr<slot_impl>;

/// Basic functionality for untyped %slot (abstract).
/// @ingroup signal_group
class slot_base {
    friend slot_impl;
    friend signal_base;

    void disconnect();
    void link(signal_base * signal) { signal_ = signal; }

public:

   ~slot_base();
    connection cx();

protected:

    slot_ptr      impl_;
    signal_base * signal_ = nullptr;
};

/// An object that tracks signal->slot connections automatically.
/// @ingroup signal_group
class trackable {
    std::list<slot_impl *> slots_;
    friend slot_impl;

    void track(slot_impl * s);
    void untrack(slot_impl * s);

public:

    trackable() = default;
    trackable(const trackable & src);
    trackable & operator=(const trackable & src);
   ~trackable();
};

struct slot_impl {
    virtual ~slot_impl() {}

    slot_impl(slot_base * slot=nullptr);
    slot_impl(const slot_impl & other) = delete;
    slot_impl(slot_impl && other) = delete;
    slot_impl & operator=(const slot_impl & other) = delete;
    slot_impl & operator=(slot_impl && other) = delete;

    virtual slot_ptr clone(slot_base * slot=nullptr) const = 0;
    virtual void reset() = 0;
    virtual trackable * target() = 0;
    virtual bool empty() const = 0;

    bool blocked() const;
    void block();
    void unblock();
    void link(slot_base * slot);
    void disconnect();
    void track();
    void untrack();

    slot_base * slot_ = nullptr;
    unsigned    blocked_ = 0;
};

template <typename R, typename... Args>
struct slot_impl_T<R(Args...)>: slot_impl {
    slot_impl_T(slot_base * slot):
        slot_impl(slot)
    {
    }

    virtual R operator()(Args... args) = 0;
};

template <class functor_type, typename R, typename... Args>
struct slot_impl_F<functor_type, R(Args...)>: slot_impl_T<R(Args...)> {
    using self_type = slot_impl_F<functor_type, R(Args...)>;
    functor_type functor_;

    slot_impl_F(const functor_type & functor, slot_base * slot=nullptr):
        slot_impl_T<R(Args...)>(slot),
        functor_(functor)
    {
        this->track();
    }

   ~slot_impl_F() {
        this->untrack();
    }

    R operator()(Args... args) override {
        return !this->blocked() ? functor_(args...) : R();
    }

    slot_ptr clone(slot_base * s=nullptr) const override {
        return std::make_shared<self_type>(functor_, s);
    }

    void reset() override {
        functor_.reset();
    }

    trackable * target() override {
        return functor_.target();
    }

    bool empty() const override {
        return functor_.empty();
    }
};

/// Typed %slot.
/// @ingroup signal_group
template <typename R, typename... Args>
class slot<R(Args...)>: public slot_base {
    friend class signal<R(Args...)>;

public:

    /// The default constructor.
    /// Creates an empty %slot.
    slot() = default;

    /// Call functor.
    R operator()(Args... args) const {
        using impl_type = slot_impl_T<R(Args...)>;
        return impl_ ? static_cast<impl_type &>(*impl_)(args...) : R();
    }

    /// Constructor with functor.
    template <class functor_type>
    slot(const functor_type & functor) {
        using impl_type = slot_impl_F<functor_type, R(Args...)>;
        impl_ = std::make_shared<impl_type>(functor, this);
    }

    /// Copy constructor.
    slot(const slot & other) {
        if (other.impl_) {
            impl_ = other.impl_->clone(this);
        }
    }

    /// Move constructor.
    slot(slot && other) {
        impl_ = other.impl_;
        other.impl_.reset();
        if (impl_) { impl_->link(this); }
    }

    /// Copy operator.
    slot & operator=(const slot & other) {
        if (this != &other) {
            impl_.reset();
            if (other.impl_) { impl_ = other.impl_->clone(this); }
        }

        return *this;
    }

    /// Move operator.
    slot & operator=(slot && other) {
        impl_ = other.impl_;
        other.impl_.reset();
        if (impl_) { impl_->link(this); }
        return *this;
    }

    operator bool() const {
        return impl_ && !impl_->empty();
    }
};

/// Holds a %connection between the %signal and %slot.
/// @ingroup signal_group
class connection {
public:

    /// Default constructor.
    connection() = default;

    /// Copy constructor.
    connection(const connection & other) = default;

    /// Copy operator.
    connection & operator=(const connection & other) = default;

    /// @private
    connection(slot_ptr slot);

    /// Disconnect slots from %signal.
    void disconnect();

    /// Test if %connection has been blocked by calling block() method.
    /// By default, the %connection is not blocked.
    bool blocked() const;

    /// Block the %connection.
    /// If %connection has been blocked, the %signal emition does not happens.
    /// Such functionality can be useful in the event that it is necessary
    /// to temporarily prevent a particular slot from being called.
    /// By default, the %connection is not blocked.
    /// @see unblock()
    void block();

    /// Unblock the %connection.
    /// By default, the %connection is not blocked.
    /// @see block()
    void unblock();

    /// Test if empty.
    /// The %connection is empty when constructed using default constructor
    /// or after disconnect() has been called.
    bool empty() const;

private:

    slot_ptr slot_;
};

/// Basic functionality for untyped %signal (abstract).
/// @ingroup signal_group
class signal_base: public trackable {
    virtual void erase(slot_base * slot) = 0;
    friend slot_base;

protected:

    virtual ~signal_base();
    signal_base() = default;
    connection link(slot_base & slot) { slot.link(this); return slot.cx(); }
};

template <class Slot, typename R, typename... Args>
struct signal_emitter<Slot, R(Args...)> {
    R operator()(std::shared_ptr<Slot> * p, std::size_t n, Args... args) {
        R last = R();

        while (n--) {
            last = (*p++)->operator()(args...);
            if (last) { break; }
        }

        return last;
    }
};

template <class Slot, typename... Args>
struct signal_emitter<Slot, void(Args...)> {
    void operator()(std::shared_ptr<Slot> * p, std::size_t n, Args... args) {
        while (n--) {
            (*p++)->operator()(args...);
        }
    }
};

/// Typed signal.
/// Signal is a list of slots. You may add slots at any time or
/// you can remove slots by using saved connection object.
/// The %signal derives trackable functionality, so when signal going out
/// of scope, all connections to it properly disconnected.
/// You can also connect two signals one after one.
/// @ingroup signal_group
template<typename R, typename... Args>
class signal<R(Args...)>: public signal_base {
    using slot_type = slot<R(Args...)>;
    using impl_type = slot_impl_T<R(Args...)>;
    using emitter_type = signal_emitter<impl_type, R(Args...)>;
    std::list<slot_type> slots_;

    void erase(slot_base * s) override {
        slots_.remove(*static_cast<slot_type *>(s));
    }

public:

    /// Default constructor.
    /// Creates an empty %signal.
    signal() = default;

    /// Copy constructor.
    signal(const signal & other) = default;

    /// Copy operator.
    signal & operator=(const signal & other) = default;

    /// Move constructor.
    signal(signal && other):
        slots_(std::move(other.slots_))
    {
    }

    /// Move operator.
    signal & operator=(signal && other) {
        slots_ = std::move(other.slots_);
        return *this;
    }

    /// Merge signals.
    void operator+=(const signal & other) {
        if (this != &other) {
            for (const slot_type & slot: other.slots_) {
                slots_.emplace_back(slot);
            }
        }
    }

    /// Test if empty.
    bool empty() const {
        return slots_.empty();
    }

    /// Count slots connected.
    /// @return connected slot count.
    std::size_t size() const {
        return slots_.size();
    }

    /// Connect slot using %slot's copy constructor.
    /// @param slot the %slot to be connected.
    /// @param prepend pass @b true to push front %slot and @b false to push back.
    /// @return the connection object holding information about %connection.
    connection connect(const slot_type & slot, bool prepend=false) {
        if (!prepend) {
            slots_.emplace_back(slot);
            return link(slots_.back());
        }

        else {
            slots_.emplace_front(slot);
            return link(slots_.front());
        }
    }

    /// Connect slot using %slot's move constructor.
    /// @param slot the %slot to be connected.
    /// @param prepend pass @b true to push front %slot and @b false to push back.
    /// @return the connection object holding information about %connection.
    connection connect(slot_type && slot, bool prepend=false) {
        if (!prepend) {
            slots_.emplace_back(std::move(slot));
            return link(slots_.back());
        }

        else {
            slots_.emplace_front(std::move(slot));
            return link(slots_.front());
        }
    }

    /// Emit the %signal.
    R operator()(Args... args) {
        if (std::size_t n = slots_.size()) {
            using ptr_type = std::shared_ptr<impl_type>;
            n = std::min(n, (512*1024U)/sizeof(ptr_type));
            ptr_type tmp[n], * d = tmp;
            std::size_t nn = n;

            for (auto i = slots_.begin(); nn && i != slots_.end(); ++i, --nn) {
                *(d++) = std::static_pointer_cast<impl_type>(i->impl_);
            }

            return emitter_type()(tmp, n, args...);
        }

        return R();
    }
};

/// Create functor from pointer to the static function.
/// @ingroup signal_group
template <typename R, typename... Args>
inline fun_functor<R(Args...)>
fun(R (*fun)(Args...)) {
    return fun_functor<R(Args...)>(fun);
}

/// Create functor from @b std::function object.
/// @ingroup signal_group
template <typename R, typename... Args>
inline fun_functor<R(Args...)>
fun(std::function<R(Args...)> fun) {
    return fun_functor<R(Args...)>(fun);
}

/// Create functor from pointer to object of some class and method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj * obj, R (Class::*fun)(Args...)) {
    return mem_functor<Obj, R(Args...)>(obj, fun);
}

/// Create functor from shared pointer to object of some class and method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(std::shared_ptr<Obj> obj_ptr, R (Class::*fun)(Args...)) {
    return mem_functor<Obj, R(Args...)>(obj_ptr.get(), fun);
}

/// Create functor from link to object of some class and method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj & obj, R (Class::*fun)(Args...)) {
    return mem_functor<Obj, R(Args...)>(&obj, fun);
}

/// Create functor from pointer to object of some class and const-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj * obj, R (Class::*fun)(Args...) const) {
    return mem_functor<Obj, R(Args...)>(obj, fun);
}

/// Create functor from shared pointer to object of some class and const-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(std::shared_ptr<Obj> obj_ptr, R (Class::*fun)(Args...) const) {
    return mem_functor<Obj, R(Args...)>(obj_ptr.get(), fun);
}

/// Create functor from link to object of some class and const-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj & obj, R (Class::*fun)(Args...) const) {
    return mem_functor<Obj, R(Args...)>(&obj, fun);
}

/// Create functor from pointer to object of some class and volatile-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R, Args...>
fun(Obj * obj, R (Class::*fun)(Args...) volatile) {
    return mem_functor<Obj, R(Args...)>(obj, fun);
}

/// Create functor from shared pointer to object of some class and volatile-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R, Args...>
fun(std::shared_ptr<Obj> obj_ptr, R (Class::*fun)(Args...) volatile) {
    return mem_functor<Obj, R(Args...)>(obj_ptr.get(), fun);
}

/// Create functor from link to object of some class and volatile-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj & obj, R (Class::*fun)(Args...) volatile) {
    return mem_functor<Obj, R(Args...)>(&obj, fun);
}

/// Create functor from pointer to object of some class and const-volatile-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj * obj, R (Class::*fun)(Args...) const volatile) {
    return mem_functor<Obj, R(Args...)>(obj, fun);
}

/// Create functor from shared pointer to object of some class and const-volatile-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(std::shared_ptr<Obj> obj_ptr, R (Class::*fun)(Args...) const volatile) {
    return mem_functor<Obj, R(Args...)>(obj_ptr.get(), fun);
}

/// Create functor from link to object of some class and const-volatile-qualified method of that class.
/// @ingroup signal_group
template <typename R, typename Obj, class Class, typename... Args>
inline mem_functor<Obj, R(Args...)>
fun(Obj & obj, R (Class::*fun)(Args...) const volatile) {
    return mem_functor<Obj, R(Args...)>(&obj, fun);
}

/// Create functor from a reference to the signal.
/// @ingroup signal_group
template <typename R, typename... Args>
inline mem_functor<signal<R(Args...)>, R(Args...)>
fun(signal<R(Args...)> & sig) {
    return mem_functor<signal<R(Args...)>, R(Args...)>(&sig, &signal<R(Args...)>::operator());
}

/// Create functor from a pointer to the signal.
/// @ingroup signal_group
template <typename R, typename... Args>
inline mem_functor<signal<R(Args...)>, R(Args...)>
fun(signal<R(Args...)> * sig) {
    return mem_functor<signal<R(Args...)>, R(Args...)>(sig, &signal<R(Args...)>::operator());
}

/// Create functor with bound parameters.
/// @ingroup signal_group
template <class functor_type, typename... Bound>
inline bind_functor<std::tuple<Bound...>, functor_type>
bind(const functor_type & functor, Bound... bounds) {
    return bind_functor<std::tuple<Bound...>, functor_type>(std::make_tuple(bounds...), functor);
}

/// Create functor with bound parameters.
/// @ingroup signal_group
template <class functor_type, typename... Bound>
inline bind_functor<std::tuple<Bound...>, functor_type>
bind(functor_type && functor, Bound... bounds) {
    return bind_functor<std::tuple<Bound...>, functor_type>(std::make_tuple(bounds...), std::move(functor));
}

} // namespace tau

#endif // TAU_SIGNAL_HH
