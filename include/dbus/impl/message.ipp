// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DBUS_MESSAGE_IPP
#define DBUS_MESSAGE_IPP

namespace dbus {

message message::new_call(
    const endpoint& destination,
    const string& method_name)
{
  return dbus_message_new_method_call(
      destination.get_process_name().c_str(),
      destination.get_path().c_str(),
      destination.get_interface().c_str(),
      method_name.c_str());
}

message message::new_return(
    message& call)
{
  return dbus_message_new_method_return(call);
}

message message::new_error(
    message& call,
    const string& error_name,
    const string& error_message)
{
  return dbus_message_new_error(call,
    error_name.c_str(),
    error_message.c_str());
}

message message::new_signal(
    const endpoint& origin,
    const string& signal_name)
{
  return dbus_message_new_signal(
      origin.get_path().c_str(),
      origin.get_interface().c_str(),
      signal_name.c_str());
}

template<typename Element>
message::packer message::pack(const Element& e)
{
  return packer(*this).pack(e);
}

template<typename Element>
message::unpacker message::unpack(Element& e)
{
  return unpacker(*this).unpack(e);
}

} // namespace dbus

#endif // DBUS_MESSAGE_IPP
