// Copyright (c) Denis Sokolovsky <ganellon@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIO_DBUS_SUPPORT
#define ASIO_DBUS_SUPPORT

#include <variant>
#include <type_traits>

namespace dbus::variant
{
namespace detail
{

template <int N, typename V, typename F>
struct for_each
{
    static constexpr void exec(F f)
    {
        for_each<N-1, V, F>::exec(f);
        f(std::variant_alternative_t<N, V>{});
    }
};

template <typename V, typename F>
struct for_each<0, V, F>
{
    static constexpr void exec(F f)
    {
        f(std::variant_alternative_t<0, V>{});
    }
};
}

template <typename V, typename F>
constexpr void for_each(F f)
{
    detail::for_each<std::variant_size_v<V> - 1, V, F>::exec(f);
}

}

#endif // ASIO_DBUS_SUPPORT
