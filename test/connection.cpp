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

TEST(ConnectionTest, DestroyBeforeEventLoopStart) {
  asio::io_context io;

  {
    dbus::connection bus(io, dbus::bus::system);

    dbus::filter f(bus, [] (dbus::message& m) {
        return true;
        });
    f.async_dispatch([] (asio::error_code, dbus::message) {});
  }

  asio::steady_timer t(io, 10ms);
  t.async_wait([&io] (const asio::error_code& /*e*/) { io.stop(); });
  io.run();
}

TEST(ConnectionTest, CallWithExpiredTimeoutAndReplyCallback) {
  constexpr auto srv_name = "com.test.server";
  constexpr auto srv_method = "m1";

  for (auto i = 0u; i < 1000; ++i)
  {
    asio::io_context io;
    dbus::connection server(io, dbus::bus::session);
    dbus::connection client(io, dbus::bus::session);

    server.request_name(srv_name);

    dbus::filter f(server, [] (dbus::message& m) { return m.get_member() == srv_method; });
    f.async_dispatch([&io, &server] (asio::error_code ec, dbus::message m)
        {
          if (ec) { FAIL(); io.stop(); return; }

          EXPECT_EQ(m.get_signature(), "i");
          int arg1;
          m.unpack(arg1);

          EXPECT_EQ(arg1, 42);

          asio::steady_timer t(io, 100ms);
          t.async_wait([&io, &server, m] (const asio::error_code& e) mutable {
            if (e && e != asio::error::operation_aborted) { FAIL(); io.stop(); return; }
            auto r = server.reply(m);
            server.async_send(r, [] (asio::error_code const&, dbus::message const&) {});
          });
        });

    bool test_result = false;
    dbus::message m = dbus::message::new_call({srv_name, "/", srv_name, srv_method});
    m.pack(42);
    client.async_send(m, [&io, &test_result] (asio::error_code ec, dbus::message& r)
        {
          test_result = true;
          io.stop();
        }, 10);

    io.run_for(100ms);

    EXPECT_TRUE(test_result);
    if (!test_result)
      break;
  }
}
