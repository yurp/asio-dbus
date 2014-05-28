// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DBUS_MESSAGE_IPP
#define DBUS_MESSAGE_IPP

namespace dbus {

message message::new_call(
    const string& process_name,
    const string& object_path,
    const string& interface_name,
    const string& method_name)
{
  return dbus_message_new_method_call(
      process_name.c_str(),
      object_path.c_str(),
      interface_name.c_str(),
      method_name.c_str());
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
