#ifndef SDK_PLUGINS_WEBSOCKET_CLIENT_H
#define SDK_PLUGINS_WEBSOCKET_CLIENT_H

#ifndef SDK_WEBSOCKET_TLS
#define SDK_WEBSOCKET_TLS 1
#endif

#ifdef SDK_WEBSOCKET_TLS
#include <websocketpp/config/asio_client.hpp>
#endif
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>


namespace sdk {
namespace plugins {

using websocketpp::connection_hdl;



class websocket_client
{
public:
  typedef websocketpp::client<websocketpp::config::asio_client> client;
  typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;

  websocket_client();
  ~websocket_client();

  void run(const std::string& uri);
  void on_open(connection_hdl hdl);
  void on_close(connection_hdl hdl);
  void on_fail(connection_hdl hdl);


public:
  void loop_();

protected:
  void set_logging();

protected:
  client client_;
  connection_hdl hdl_;
  websocketpp::lib::mutex lock_;
  bool open_;
  bool done_;

  websocketpp::lib::thread asio_thread_;
};

#ifdef SDK_WEBSOCKET_TLS
class websocket_client_tls
{
public: 

  typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
  typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;
  typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

  websocket_client_tls();
  ~websocket_client_tls();

  void run(const std::string& uri);
  void on_open(connection_hdl hdl);
  void on_close(connection_hdl hdl);
  void on_fail(connection_hdl hdl);

  void init_tls(const std::string& hostname);
  context_ptr on_tls_init(const char * hostname, connection_hdl);

public:
  void loop_();

protected:
  void set_logging();

protected:
  client client_;
  connection_hdl hdl_;
  websocketpp::lib::mutex lock_;
  bool open_;
  bool done_;

  websocketpp::lib::thread asio_thread_;
};

#endif

} /// namespace plugins
} /// namespace sdk

#endif