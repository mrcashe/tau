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

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <tuple>
#include <vector>
#include <iostream>

namespace tau {

template <typename R, typename... Args> class fun_functor;
template <class Class, typename R, typename... Args> class mem_functor;
template <typename R, typename... Args> class slot;
template <typename R, typename... Args> class signal;
template <class Functor, typename... Bound> class bind_functor;
template <class Slot, typename R, typename... Args> struct signal_emitter;

template <class Functor>
struct functor_trait {
    using functor_type = Functor;
};

template <typename R, typename... Args>
struct functor_trait<R (*)(Args...)> {
    using functor_type = fun_functor<R(Args...)>;
};

template <class Class, typename R, class... Args>
struct functor_trait<R (Class::*)(Args...)> {
    using functor_type = mem_functor<Class, R(Args...)>;
};

class trackable;

class functor_base {
public:

    virtual bool empty() const = 0;
    virtual void reset() = 0;
    virtual trackable * target() = 0;
};

template<typename R, typename... Args>
class fun_functor<R(Args...)>: public functor_base {
public:

    using function_type = std::function<R (Args...)>;
    using result_type = R;

protected:

    function_type fun_;

public:

    explicit fun_functor(const function_type & fun):
        fun_(fun)
    {
    }

    R operator()(Args... args) const {
        return fun_ ? fun_(args...) : R();
    }

    trackable * target() override {
        return nullptr;
    }

    bool empty() const override {
        return !fun_;
    }

    void reset() override {
        fun_ = function_type();
    }
};

template <class Class, bool is_trackable=std::is_base_of<trackable, Class>::value>
class mem_functor_base: public functor_base {
public:

    mem_functor_base(Class * obj) {}

    trackable * target() override {
        return nullptr;
    }

    void reset() override {}
};

template <class Class>
class mem_functor_base<Class, true>: public functor_base {
    trackable * target_;

public:

    mem_functor_base(Class * obj):
        target_(obj)
    {
    }

    trackable * target() override {
        return target_;
    }

    void reset() override {
        target_ = nullptr;
    }
};

template <class Class, typename R, typename... Args>
class mem_functor<Class, R(Args...)>:
    public mem_functor_base<Class> {

public:

    using base_type = mem_functor_base<Class>;
    using result_type = R;
    using function_type = std::function<R(Class *, Args...)>;

protected:

    Class * obj_;
    function_type fun_;

public:

    explicit mem_functor(Class * obj, const function_type & fun):
        base_type(obj),
        obj_(obj)
    {
        fun_ = fun;
    }

    R operator()(Args... args) const {
        return fun_ ? fun_(obj_, args...) : R();
    }

    void reset() override {
        base_type::reset();
        fun_ = nullptr;
    }

    bool empty() const override {
        return false == bool(fun_);
    }
};

template <class Functor, typename... Bound>
class bind_functor: public functor_base {
public:

    using functor_type = typename functor_trait<Functor>::functor_type;
    using result_type = typename functor_type::result_type;
    using bound_type = std::tuple<Bound...>;

protected:

    functor_type functor_;
    bound_type   bound_;

    template <class Tuple, typename... Args>
    auto bind_call(const Tuple & bound, Args... args) const {
        return std::tuple_cat(std::make_tuple(args...), bound);
    }

    template<class Tuple, std::size_t... Index>
    auto call_functor_impl(const Functor & functor, const Tuple & tuple, std::index_sequence<Index...>) const {
        return functor(std::get<Index>(tuple)...);
    }

    template<class Tuple>
    auto call_functor(const Functor & functor, const Tuple & tuple) const {
        using index_type = std::make_index_sequence<std::tuple_size<Tuple>::value>;
        return call_functor_impl(functor, tuple, index_type());
    }

public:

    bind_functor(const functor_type & functor, Bound... bounds):
        functor_(functor),
        bound_(std::make_tuple(bounds...))
    {
    }

    template <typename... Args>
    result_type operator()(Args... args) const {
        auto call = bind_call<bound_type, Args...>(bound_, args...);
        using call_type = decltype(bind_call(std::declval<bound_type>(), args...));
        return call_functor<call_type>(functor_, call);
    }

    result_type operator()() const {
        using call_type = bound_type;
        return call_functor<call_type>(functor_, bound_);
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
    connection conn();

protected:

    slot_ptr      impl_;
    bool          tmp_ = false;
    signal_base * signal_ = nullptr;
    void *        call_ = nullptr;
};

/// An object that tracks signal->slot connections automatically.
/// @ingroup signal_group
class trackable {
    std::list<slot_impl *> slots_;

    friend slot_impl;

    void track(slot_impl * s) {
        auto i = std::find(slots_.begin(), slots_.end(), s);
        if (i == slots_.end()) { slots_.push_back(s); }
    }

    void untrack(slot_impl * s) {
        slots_.remove(s);
    }

public:

    trackable() = default;
    trackable(const trackable & src) {}
    trackable & operator=(const trackable & src) { return *this; }
   ~trackable();
};

struct slot_impl {
    virtual ~slot_impl() {}

    slot_impl(slot_base * slot=nullptr):
        slot_(slot)
    {
    }

    slot_impl(const slot_impl & other) = delete;
    slot_impl(slot_impl && other) = delete;
    slot_impl & operator=(const slot_impl & other) = delete;
    slot_impl & operator=(slot_impl && other) = delete;

    virtual slot_ptr clone(slot_base * slot=nullptr) const = 0;
    virtual void reset() = 0;
    virtual trackable * target() = 0;
    virtual bool empty() const = 0;

    bool blocked() const { return 0 != blocked_; }
    void block() { ++blocked_; }

    void unblock() {
        if (0 != blocked_) {
            --blocked_;
        }
    }

    // Called by owning slot.
    void link(slot_base * slot) { slot_ = slot; }

    // Called by connection and trackable.
    void disconnect() {
        if (slot_) {
            slot_->disconnect();
            slot_ = nullptr;
        }
    }

    void track() {
        if (target()) {
            target()->track(this);
        }
    }

    void untrack() {
        if (target()) {
            target()->untrack(this);
        }
    }

    slot_base * slot_ = nullptr;
    unsigned    blocked_ = 0;
};

inline trackable::~trackable() {
    for (auto s: slots_) { s->reset(); s->disconnect(); }
}

template <class Functor>
struct slot_impl_T: slot_impl {
    using self_type = slot_impl_T<Functor>;
    Functor functor_;

    slot_impl_T(const Functor & functor, slot_base * slot=nullptr):
        slot_impl(slot),
        functor_(functor)
    {
        track();
    }

    ~slot_impl_T() {
        untrack();
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

    template <class Functor>
    struct call {
        inline static R call_it(slot_ptr impl, Args... args) {
            return (std::static_pointer_cast<slot_impl_T<Functor>>(impl))->functor_(args...);
        }

        inline static void * address() {
            return reinterpret_cast<void *>(&call_it);
        }
    };

    using call_type = R (*)(slot_ptr, Args...);

public:

    /// The default constructor.
    /// Creates an empty %slot.
    slot() = default;

    /// Constructor with functor.
    template <class Functor>
    slot(const Functor & functor) {
        using impl_type = slot_impl_T<Functor>;
        impl_ = std::make_shared<impl_type>(functor, this);
        call_ = call<Functor>::address();
    }

    /// Copy constructor.
    slot(const slot & other) {
        call_ = other.call_;

        if (!other.tmp_) {
            if (other.impl_) { impl_ = other.impl_->clone(this); }
        }

        else {
            impl_ = other.impl_;
            tmp_ = true;
        }
    }

    /// Move constructor.
    slot(slot && other) {
        call_ = other.call_;

        if (!other.tmp_) {
            impl_ = other.impl_;
            other.impl_.reset();
            if (impl_) { impl_->link(this); }
        }

        else {
            impl_ = other.impl_;
            tmp_ = true;
        }
    }

    /// @private
    slot(const slot & other, std::nullptr_t p) {
        call_ = other.call_;
        impl_ = other.impl_;
        tmp_ = true;
    }

    /// Copy operator.
    slot & operator=(const slot & other) {
        if (this != &other) {
            call_ = other.call_;
            tmp_ = other.tmp_;

            if (!tmp_) {
                if (other.impl_) { impl_ = other.impl_->clone(this); }
            }

            else {
                impl_ = other.impl_;
            }
        }

        return *this;
    }

    /// Move operator.
    slot & operator=(slot && other) {
        impl_ = other.impl_;
        call_ = other.call_;
        tmp_ = other.tmp_;

        if (!tmp_) {
            other.impl_.reset();
            if (impl_) { impl_->link(this); }
        }

        return *this;
    }

    operator bool() const {
        return impl_ && !impl_->empty();
    }

    /// Call holding function.
    R operator()(Args... args) const {
        if (impl_ && !impl_->blocked()) {
            return reinterpret_cast<call_type>(call_)(impl_, args...);
        }

        return R();
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
    connection(slot_ptr slot): slot_(slot) {}

    /// Disconnect slots from %signal.
    void disconnect() {
        if (slot_) {
            slot_->untrack();
            slot_->disconnect();
            slot_->reset();
            slot_.reset();
        }
    }

    /// Test if %connection has been blocked by calling block() method.
    /// By default, the %connection is not blocked.
    bool blocked() const { return slot_ && slot_->blocked(); }

    /// Block the %connection.
    /// If %connection has been blocked, the %signal emition does not happens.
    /// Such functionality can be useful in the event that it is necessary
    /// to temporarily prevent a particular slot from being called.
    /// By default, the %connection is not blocked.
    /// @see unblock()
    void block() {
        if (slot_) {
            slot_->block();
        }
    }

    /// Unblock the %connection.
    /// By default, the %connection is not blocked.
    /// @see block()
    void unblock() {
        if (slot_) {
            slot_->unblock();
        }
    }

    /// Test if empty.
    /// The %connection is empty when constructed using default constructor
    /// or after disconnect() has been called.
    bool empty() const { return !slot_ || slot_->empty(); }

private:

    slot_ptr slot_;
};

/// Basic functionality for untyped %signal (abstract).
/// @ingroup signal_group
class signal_base: public trackable {
public:

    virtual void erase(slot_base * slot) = 0;
    virtual ~signal_base() {}

protected:

    signal_base() = default;
    connection link(slot_base & slot) { slot.link(this); return slot.conn(); }
};

inline slot_base::~slot_base() {
    if (impl_) {
        impl_->link(nullptr);
    }
}

inline void slot_base::disconnect() {
    if (signal_) {
        signal_->erase(this);
        signal_ = nullptr;
    }
}

inline connection slot_base::conn() {
    return connection(impl_);
}

template <class Slot, typename R, typename... Args>
struct signal_emitter<Slot, R(Args...)> {
    Slot * p;
    std::size_t n;

    signal_emitter(Slot * sp, std::size_t s): p(sp), n(s) {}

    R operator()(Args... args) {
        R last = R();

        while (n--) {
            last = (*p++)(args...);
            if (last) { break; }
        }

        return last;
    }
};

template <class Slot, typename... Args>
struct signal_emitter<Slot, void(Args...)> {
    Slot * p;
    std::size_t n;

    signal_emitter(Slot * sp, std::size_t s): p(sp), n(s) {}

    void operator()(Args... args) {
        while (n--) {
            (*p++)(args...);
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
public:

    using slot_type = slot<R(Args...)>;
    using emitter_type = signal_emitter<slot_type, R(Args...)>;

private:

    std::vector<slot_type> slots_;

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
        slots_.clear();
        slots_ = std::move(other.slots);
        return *this;
    }

    /// Merge signals.
    void operator+=(const signal & other) {
        if (this != &other) {
            for (auto & slot: other.slots_) {
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

    /// Reserve space used for slots.
    /// The resulting size calculated relatively to the current slot count.
    void reserve(std::size_t n) {
        slots_.reserve(n+size());
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
            slots_.emplace(slots_.begin(), slot);
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
            slots_.emplace(slots_.begin(), std::move(slot));
            return link(slots_.front());
        }
    }

    /// Emit the %signal.
    R operator()(Args... args) {
        std::size_t n = slots_.size(), nn = n;

        if (0 != n) {
            slot_type tmp[n], * d = tmp;
            for (const slot_type * s = slots_.data(); nn; --nn, ++s) { *(d++) = slot_type(*s, nullptr); }
            return emitter_type(tmp, n)(args...);
        }

        return R();
    }

    /// @private
    void erase(slot_base * slot) override {
        std::size_t n = slots_.size(), i = 0;

        for (const slot_type * s = slots_.data(); n; --n, ++s, ++i) {
            if (s == slot) {
                slots_.erase(slots_.begin()+i);
                return;
            }

        }
    }
};

/// Create functor from pointer to static function.
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
template <class Functor, typename... Bound>
inline bind_functor<Functor, Bound...>
bind(const Functor & functor, Bound... bounds) {
    return bind_functor<Functor, Bound...>(functor, bounds...);
}

} // namespace tau

#endif // TAU_SIGNAL_HH
