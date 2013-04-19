// Taussig
//
// Written in 2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// Interoperation with iterators and containers

#ifndef TAUSSIG_INTEROP_HPP
#define TAUSSIG_INTEROP_HPP

#include <taussig/primitive.h++>
#include <taussig/detail/is_decayed.h++>
#include <taussig/detail/optional.h++>

#include <wheels/meta.h++>

#include <iterator>
#include <type_traits>
#include <cstddef>

namespace seq {
    namespace detail {
        //! {class}
        //! An iterator over sequences.
        //! *Requires*: `Sequence` is a [concept:Sequence] [hard];
        //!             and is decayed [hard].
        //! *Note*: these iterators are not fully-featured and are meant only for simple iteration over a sequence;
        //!         use sequences operations if you need more flexibility.
        template <typename Sequence>
        struct sequence_iterator {
            //! *Invariant*: `is_end()` or `seq` contains a non-empty sequence.

            static_assert(is_sequence<Sequence>(), "sequence_iterator requires a Sequence");
            static_assert(detail::is_decayed<Sequence>(), "sequence_iterator requires a decayed type");

            using iterator_category = std::forward_iterator_tag;
            using value_type = seq::Value<Sequence>;
            using reference = seq::Reference<Sequence>;
            using difference_type = std::ptrdiff_t;
            using pointer = wheels::RemoveReference<value_type>*;

            //! {constructor}
            //! *Constructs*: an end iterator.
            //! *Ensures*: `is_end()`.
            sequence_iterator() = default;

            //! {constructor}
            //! *Constructs*: an iterator to the beginning of `seq`.
            //! *Ensures*: `!is_end()`.
            sequence_iterator(Sequence seq) : seq(std::move(seq)) {}

            //! {function}
            //! *Requires*: `!is_end()`
            //! *Returns*: the element this iterator points at.
            reference operator*() const { return seq::front(*seq); }

            //! {function}
            //! *Requires*: `!is_end()`
            //! *Effects*: advances this iterator to the next element.
            //! *Returns*: `*this`.
            sequence_iterator& operator++() {
                seq::pop_front(*seq);
                return *this;
            }
            //! {function}
            //! *Requires*: `!is_end()`
            //! *Effects*: advances this iterator to the next element.
            //! *Returns*: the old value of `*this`.
            sequence_iterator operator++(int) {
                sequence_iterator copy(*this);
                ++*this;
                return copy;
            }
            //! {function}
            //! *Returns*: `true` if both `*this` and `that` are end iterators.
            bool operator==(sequence_iterator const& that) const {
                return is_end() && that.is_end();
            }

            //! {function}
            //! *Returns*: `false` if one of `*this` and `that` is not an end iterator.
            bool operator!=(sequence_iterator const& that) const {
                return !(*this == that);
            }
        private:
            //! {function}
            //! *Returns*: `true` if this is an end iterator.
            bool is_end() const {
                return !seq || seq::empty(*seq);
            }

            detail::optional<Sequence> seq;
        };

        //! {function}
        //! *Requires*: `S` is a [concept:Sequence] [soft].
        //! *Returns*: an iterator to the start of `s`.
        template <typename S,
                  wheels::EnableIf<is_sequence<S>>...>
        sequence_iterator<wheels::Decay<S>> begin(S&& s) { return { std::forward<S>(s) }; }
        //! {function}
        //! *Requires*: `S` is a [concept:Sequence] [soft].
        //! *Returns*: an iterator to the end of `s`.
        template <typename S,
                  wheels::EnableIf<is_sequence<S>>...>
        sequence_iterator<wheels::Decay<S>> end(S&&) { return {}; }
    } // namespace detail

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft].
    //! *Returns*: an iterator to the start of `s`.
    template <typename S,
                wheels::EnableIf<is_sequence<S>>...>
    detail::sequence_iterator<wheels::Decay<S>> begin(S&& s) { return detail::begin(s); }
    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft].
    //! *Returns*: an iterator to the end of `s`.
    template <typename S,
                wheels::EnableIf<is_sequence<S>>...>
    detail::sequence_iterator<wheels::Decay<S>> end(S&& s) { return detail::end(s); }

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft];
    //!             `C` is a [concept:Container].
    //! *Returns*: a sequence in the current state 
    template <typename C, typename S,
                wheels::EnableIf<is_sequence<S>>...>
    C materialize(S&& s) {
        return C(seq::begin(s), seq::end(s));
    }

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft];
    //!             `C<seq::Value<S>>` is a [concept:Container].
    //! *Returns*: a sequence in the current state 
    template <template <typename...> class C, typename S,
                wheels::EnableIf<is_sequence<S>>...>
    C<Value<S>> materialize(S&& s) {
        return materialize<C<Value<S>>>(std::forward<S>(s));
    }
} // namespace seq

#endif // TAUSSIG_INTEROP_HPP
