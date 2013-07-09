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

// Sequence generate() algorithm

#ifndef TAUSSIG_ALGORITHMS_GENERATE_HPP
#define TAUSSIG_ALGORITHMS_GENERATE_HPP

#include <wheels/adl/get.h++>
#include <wheels/fun/result_of.h++>
#include <wheels/meta/decay.h++>
#include <wheels/meta/is_related.h++>
//#include <wheels/tuple.h++>

#include <taussig/traits/true_sequence.h++>
#include <taussig/traits/value_type.h++>
#include <taussig/traits/is_true_sequence.h++>

#include <tuple> // tuple_element
#include <utility> // forward

namespace seq {
    template <typename Fun>
    struct generate_sequence : true_sequence {
    private:
        using fun_type = wheels::meta::Decay<Fun>;
        using result_type = wheels::fun::ResultOf<fun_type&()>;

    public:
        template <typename FunF,
                  wheels::meta::DisableIfRelated<FunF, generate_sequence<Fun>>...>
        explicit generate_sequence(FunF&& fun)
        : fun(std::forward<FunF>(fun)), result(wheels::fun::invoke(this->fun)) {}

        using reference = ValueType<result_type>;
        using value_type = wheels::meta::Decay<reference>;

        bool empty() const {
            return !result;
        }
        reference front() const {
            return *result;
        }
        void pop_front() {
            result = wheels::fun::invoke(fun);
        }

    private:
        fun_type fun;
        result_type result;
    };
    static_assert(is_true_sequence<generate_sequence<wheels::optional<int>()>>(), "generate_sequence is a sequence");

    namespace result_of {
        template <typename Fun>
        using generate = generate_sequence<wheels::meta::Decay<Fun>>;
    } // namespace result_of

    template <typename Fun>
    result_of::generate<Fun> generate(Fun&& fun) {
        return result_of::generate<Fun>(std::forward<Fun>(fun));
    }
} // namespace seq

#endif // TAUSSIG_ALGORITHMS_GENERATE_HPP

