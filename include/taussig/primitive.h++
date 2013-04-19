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

// Primitive sequence operations

#ifndef TAUSSIG_PRIMITIVE_HPP
#define TAUSSIG_PRIMITIVE_HPP

#include <wheels/meta.h++>

#include <iterator>
#include <type_traits>
#include <cstddef>

#include <cassert>

namespace seq {
    //! {concept}
    //! A source of values for a sequence.
    // concept SequenceSource<S> {
        //! {function}
        //! *Returns*: a [concept:Sequence] with elements from `s`.
        // nonmember Sequence<Seq> as_sequence(SequenceSource<S> const& s);
    // };

    //! {concept}
    //! A sequence with a smart interface based on Andrei Alexandrescu's ideas.
    // concept Sequence<S> requires Copyable<S> {
        //! {trait}
        //! The type of values in this sequence.
        // nonmember typename seq::Value<Sequence<S>>;

        //! {trait}
        //! The type of references to this sequence.
        //! *Note*: this type may not be a reference if the sequence is not persistent.
        // nonmember typename seq::Reference<Sequence<S>>;

        //! {function}
        //! *Returns*: `true` if the sequence `s` has no elements.
        // nonmember bool seq::empty(Sequence<S> const& s);

        //! {function}
        //! *Requires*: `!empty()`.
        //! *Returns*: the first element of the sequence `s`.
        // nonmember reference seq::front(Sequence<S> const &);

        //! {function}
        //! *Requires*: `!empty()`.
        //! *Effects*: skips the first element in the sequence `s`.
        // nonmember void seq::pop_front(Sequence<S>& s);
    // };

    //! {tag}
    //! *Effects*: marks a derived type as a [concept:Sequence] with native operations.
    template <typename... Props>
    struct native_sequence {
        struct is_native_sequence : std::true_type {};
    };
    namespace detail {
        struct native_sequence_test {
            template <typename T>
            wheels::All<
                typename wheels::Unqualified<T>::is_native_sequence,
                std::is_convertible<decltype(std::declval<T>().empty()), bool>,
                std::is_convertible<decltype(std::declval<T>().front()), typename T::reference>,
                std::is_void<decltype(std::declval<T>().pop_front())>,
                std::is_same<decltype(std::declval<T>().before(std::declval<T>())), T>
            > static test(int);
            template <typename...>
            std::false_type static test(...);
        };
    } // namespace detail

    //! {trait}
    //! *Returns*: `true` if `S` is a [concept:Sequence] with native operations;
    //             `false` otherwise.
    template <typename T>
    using is_native_sequence = wheels::TraitOf<detail::native_sequence_test, T>;

    namespace detail {
        //! {traits}
        //! *Note*: implementation backend for [traits:sequence_ops].
        template <typename S,
                  bool = is_native_sequence<S>()>
        struct sequence_ops_impl {};

        //! {specialization:1}
        template <typename S>
        struct sequence_ops_impl<S, true> {
            //! The type of values in the sequence `S`.
            using value_type = typename S::value_type;
            //! {trait}
            //! The type of references to the sequence `S`.
            //! *Note*: this type may not be a reference if the sequence `S` is not persistent.
            using reference = typename S::reference;

            //! {function}
            //! *Returns*: `true` if the sequence `s` has no elements.
            static bool empty(S const& s) { return s.empty(); }
            //! {function}
            //! *Requires*: `!empty(s)`.
            //! *Returns*: the first element of the sequence `s`.
            static reference front(S const& s) { return s.front(); }
            //! {function}
            //! *Requires*: `!empty(s)`.
            //! *Effects*: skips the first element in the sequence `s`.
            static void pop_front(S& s) { s.pop_front(); }
            //! {function}
            static S before(S const& whole, S const& part) { return whole.before(part); }
        };

        //! {specialization}
        template <typename Iterator>
        struct sequence_ops_impl<std::pair<Iterator, Iterator>, false> {
        private:
            using sequence_type = std::pair<Iterator, Iterator>;
        public:
            using value_type = typename std::iterator_traits<Iterator>::value_type;
            using reference = typename std::iterator_traits<Iterator>::reference;

            static bool empty(sequence_type const& its) { return its.first == its.second; }
            static reference front(sequence_type const& its) { return *its.first; }
            static void pop_front(sequence_type& its) { ++its.first; }
            static sequence_type before(sequence_type const& whole, sequence_type const& part) { return { whole.first, part.first }; }
        };

        //! {traits}
        //! Provides a unified interface for using models of [concept:SequenceSource].
        //! *Requires*: `S` is a [concept:Sequence] [soft].
        template <typename S>
        struct sequence_ops : sequence_ops_impl<wheels::Unqualified<S>> {};
    } // namespace detail

    //! {metafunction}
    //! *Requires*: `S` is a [concept:Sequence] [soft].
    //! *Returns*: the type of values in the sequence`S`.
    template <typename S>
    using Value = typename detail::sequence_ops<S>::value_type;
    template <typename S>
    struct value {
        using type = Value<S>;
    };

    //! {trait}
    //! *Requires*: `S` is a [concept:Sequence] [soft].
    //! *Returns*: The type of references to the sequence `S`.
    //! *Note*: this type may not be a reference if the sequence `S` is not persistent.
    template <typename S>
    using Reference = typename detail::sequence_ops<S>::reference;
    template <typename S>
    struct reference {
        using type = Reference<S>;
    };

    namespace detail {
        struct is_sequence_test {
            template <typename T>
            wheels::Bool<true, seq::Value<T>> static test(int);
            template <typename...>
            std::false_type static test(...);
        };
    } // namespace detail
    //! {trait}
    //! *Returns*: `true` if `S` is a [concept:Sequence]; `false` otherwise.
    template <typename S>
    struct is_sequence : wheels::TraitOf<detail::is_sequence_test, S> {};

    namespace detail {
        struct is_sequence_of_test {
            template <typename T, typename U>
            std::is_same<U, seq::Value<T>> static test(int);
            template <typename...>
            std::false_type static test(...);
        };
    } // namespace detail
    //! {trait}
    //! *Returns*: `true` if `S` is a [concept:Sequence] with value `V`; `false` otherwise.
    template <typename S, typename V>
    struct is_sequence_of : wheels::TraitOf<detail::is_sequence_of_test, S, V> {};

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft].
    //! *Returns*: `true` if the sequence `s` has no elements; `false` otherwise.
    template <typename S,
                wheels::EnableIf<is_sequence<S>>...>
    bool empty(S const& s) { return detail::sequence_ops<S>::empty(s); }

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft]; and `!empty(s)`.
    //! *Returns*: the first element of the sequence `s`.
    template <typename S,
                wheels::EnableIf<is_sequence<S>>...>
    Reference<S> front(S const& s) {
        assert(!seq::empty(s));
        return detail::sequence_ops<S>::front(s);
    }

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft]; and `!empty(s)`.
    //! *Effects*: skips the first element in the sequence `s`.
    template <typename S,
                wheels::EnableIf<is_sequence<S>>...>
    void pop_front(S& s) {
        assert(!seq::empty(s));
        return detail::sequence_ops<S>::pop_front(s);
    }

    //! {function}
    //! *Requires*: `S` is a [concept:Sequence] [soft].
    //! *Returns*: a sequence with the elements of `whole` that are before the elements of `part`.
    template <typename S,
                wheels::EnableIf<is_sequence<S>>...>
    S before(S const& whole, S const& part) { return detail::sequence_ops<S>::before(whole, part); }
} // namespace seq

#endif // TAUSSIG_PRIMITIVE_HPP
