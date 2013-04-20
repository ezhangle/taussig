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

// Interoperation with iterators

#ifndef TAUSSIG_INTEROP_BEGIN_END_HPP
#define TAUSSIG_INTEROP_BEGIN_END_HPP

#include <taussig/traits/is_sequence.h++>
#include <taussig/traits/value_type.h++>
#include <taussig/traits/reference_type.h++>

#include <taussig/primitives/front.h++>
#include <taussig/primitives/pop_front.h++>
#include <taussig/primitives/empty.h++>

#include <taussig/detail/optional.h++>

#include <wheels/meta.h++> // RemoveReference, EnableIf, Decay

#include <iterator> // forward_iterator_tag
#include <utility> // move, forward
#include <cstddef> // ptrdiff_t

// ---

#include <type_traits>

namespace seq {
    namespace detail {
        //! {class}
        //! An iterator over sequences.
        //! *Requires*: `Sequence` is a sequence [hard].
        //! *Note*: these iterators are not fully-featured and are meant only for simple iteration over a sequence;
        //!         use sequences operations if you need more flexibility.
        template <typename Sequence>
        struct sequence_iterator {
            //! *Invariant*: `is_end()` or `seq` contains a non-empty sequence.

            static_assert(is_sequence<Sequence>(), "sequence_iterator requires a sequence");

            using iterator_category = std::forward_iterator_tag;
            using value_type = ValueType<Sequence>;
            using reference = ReferenceType<Sequence>;
            using difference_type = std::ptrdiff_t;
            using pointer = wheels::RemoveReference<value_type>*;

            //! {constructor}
            //! *Constructs*: an end iterator.
            //! *Ensures*: `is_end()`.
            sequence_iterator() = default;

            //! {constructor}
            //! *Constructs*: an iterator to the beginning of `seq`.
            //! *Ensures*: `!is_end()`.
            sequence_iterator(Sequence s) : s(std::move(s)) {}

            //! {function}
            //! *Requires*: `!is_end()`
            //! *Returns*: the element this iterator points at.
            reference operator*() const { return seq::front(*s); }

            //! {function}
            //! *Requires*: `!is_end()`
            //! *Effects*: advances this iterator to the next element.
            //! *Returns*: `*this`.
            sequence_iterator& operator++() {
                seq::pop_front(*s);
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
                return !s || seq::empty(*s);
            }

            detail::optional<Sequence> s;
        };
    } // namespace detail

    //! {function}
    //! *Requires*: `S` is a sequence [soft].
    //! *Returns*: an iterator to the start of `s`.
    template <typename S,
              wheels::EnableIf<is_sequence<S>>...>
    detail::sequence_iterator<wheels::Decay<S>> begin(S&& s) {
        return { std::forward<S>(s) };
    }
    //! {function}
    //! *Requires*: `S` is a sequence [soft].
    //! *Returns*: an iterator to the end of `s`.
    template <typename S,
              wheels::EnableIf<is_sequence<S>>...>
    detail::sequence_iterator<wheels::Decay<S>> end(S&&) {
        return {};
    }
} // namespace seq

#endif // TAUSSIG_INTEROP_BEGIN_END_HPP

