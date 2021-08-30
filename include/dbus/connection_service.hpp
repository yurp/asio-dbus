// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DBUS_CONNECTION_SERVICE_HPP
#define DBUS_CONNECTION_SERVICE_HPP

#include <asio.hpp>
#include <asio/io_context.hpp>

#include <dbus/detail/async_send_op.hpp>
#include <dbus/element.hpp>
#include <dbus/error.hpp>
#include <dbus/message.hpp>

#include <dbus/impl/connection.ipp>

namespace dbus {
namespace bus {
static const int session = DBUS_BUS_SESSION;
static const int system = DBUS_BUS_SYSTEM;
static const int starter = DBUS_BUS_STARTER;
}  // namespace bus

class filter;
class match;
class connection;

class connection_service
    : public asio::detail::service_base<connection_service> {
 public:
  typedef impl::connection implementation_type;

  inline explicit connection_service(asio::io_context& io)
      : asio::detail::service_base<connection_service>(io) {}

  inline void construct(implementation_type& impl) {}

  inline void destroy(implementation_type& impl) {}

  inline void shutdown_service() {
    // TODO is there anything that needs shutting down?
  }

  inline void open(implementation_type& impl, const string& address) {
    asio::io_context& io = this->get_io_context();

    impl.open(io, address);
  }

  inline void open(implementation_type& impl, const int bus = bus::system) {
    asio::io_context& io = this->get_io_context();

    impl.open(io, bus);
  }

  inline message send(implementation_type& impl, message& m) {
    return impl.send_with_reply_and_block(m);
  }

  template <typename Duration>
  inline message send(implementation_type& impl, message& m,
                      const Duration& timeout) {
    if (timeout == Duration::zero()) {
      // TODO this can return false if it failed
      impl.send(m);
      // TODO(ed) rework API to seperate blocking and non blocking sends
      return message(nullptr);
    } else {
      return impl.send_with_reply_and_block(
          m, std::chrono::milliseconds(timeout).count());
    }
  }

  template <typename MessageHandler>
  inline ASIO_INITFN_RESULT_TYPE(MessageHandler,
                                 void(asio::error_code, message))
      async_send(implementation_type& impl, message& m,
                 ASIO_MOVE_ARG(MessageHandler) h, int timeout_ms) {

    auto i = asio::bind_executor(this->get_io_context().get_executor(), [this, &impl, &m, timeout_ms](auto&& handler)
    {
      // begin asynchronous operation
      impl.start(this->get_io_context());

      using handler_type = std::decay_t<decltype(handler)>;
      detail::async_send_op<handler_type>(this->get_io_context(), std::forward<handler_type>(handler)) (impl, m, timeout_ms);
    });

    return asio::async_initiate<MessageHandler, void(asio::error_code, message)>(std::move(i), h);
  }

 private:
  friend connection;
  inline void new_match(implementation_type& impl, match& m);

  inline void delete_match(implementation_type& impl, match& m);

  inline void new_filter(implementation_type& impl, filter& f);

  inline void delete_filter(implementation_type& impl, filter& f);
};

}  // namespace dbus

#endif  // DBUS_CONNECTION_SERVICE_HPP
