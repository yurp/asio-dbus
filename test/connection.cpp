// Copyright (c) Denis Sokolovsky <ganellon@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <dbus/connection.hpp>
#include <dbus/endpoint.hpp>
#include <dbus/filter.hpp>
#include <dbus/message.hpp>
#include <functional>
#include <chrono>

#include <unistd.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std::literals;

TEST(RegressionTest, DestroyBeforeEventLoopStart) {
  asio::io_service io;

  {
    dbus::connection bus(io, dbus::bus::system);

    dbus::filter f(bus, [](dbus::message& m) {
        return true;
        });
    f.async_dispatch([](asio::error_code, dbus::message) {});
  }

  asio::steady_timer t(io, 10ms);
  t.async_wait([&](const asio::error_code& /*e*/) { io.stop(); });
  io.run();
}
