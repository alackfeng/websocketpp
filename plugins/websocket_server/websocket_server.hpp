#ifndef SDK_PLUGINS_WEBSOCKET_SERVER_H
#define SDK_PLUGINS_WEBSOCKET_SERVER_H

#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace sdk {
namespace plugins {

struct connection_data
{
  int sessionid;
  std::string name;
};

struct custom_config: public websocketpp::config::asio
{
  typedef websocketpp::config::asio core;
  typedef core::concurrency_type concurrency_type;
  typedef core::request_type request_type;
  typedef core::response_type response_type;
  typedef core::message_type message_type;
  typedef core::con_msg_manager_type con_msg_manager_type;
  typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;
  typedef core::alog_type alog_type;
  typedef core::elog_type elog_type;
  typedef core::rng_type rng_type;
  typedef core::transport_type transport_type;
  typedef core::endpoint_base endpoint_base;

  typedef connection_data connection_base;
};

typedef websocketpp::server<custom_config> server;
typedef server::connection_ptr connection_ptr;  

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class websocket_server
{
public:
  websocket_server();
  ~websocket_server();

  void on_open(connection_hdl hdl);
  void on_close(connection_hdl hdl);
  void on_message(connection_hdl hdl, server::message_ptr msg);
  void run(uint16_t port);

private:
  int next_sessionid_;
  server server_;
};


} /// namespace plugins
} /// namespace sdk

#endif