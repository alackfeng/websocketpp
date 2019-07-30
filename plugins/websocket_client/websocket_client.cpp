
#include "websocket_client.hpp"
#include <websocketpp/common/functional.hpp>

using namespace sdk;
using namespace plugins;

#ifdef WIN32
#define sleep(a) Sleep(a*1000)
#endif

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;


websocket_client::websocket_client()
:open_(false), done_(false) {
  std::cout << __FUNCTION__ << std::endl;
  set_logging();

  client_.init_asio();

  // using websocketpp::lib::placeholders::_1;
  // using websocketpp::lib::bind;
  client_.set_open_handler(bind(&websocket_client::on_open, this, _1));
  client_.set_close_handler(bind(&websocket_client::on_close, this, _1));
  client_.set_fail_handler(bind(&websocket_client::on_fail, this, _1));

  // init ssl
}
websocket_client::~websocket_client() {
  std::cout << __FUNCTION__ << std::endl;
  if(asio_thread_.joinable())
    asio_thread_.join();
  
  client_.get_alog().write(websocketpp::log::alevel::app, "~websocket_client asio_thread_ over.");
}

void websocket_client::run(const std::string& uri) {
  client_.get_alog().write(websocketpp::log::alevel::app, "websocket_client::run");

  websocketpp::lib::error_code ec;
  client::connection_ptr con = client_.get_connection(uri, ec);
  if(ec) {
    client_.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + ec.message());
  }

  hdl_ = con->get_handle();
  client_.connect(con);

  client_.get_alog().write(websocketpp::log::alevel::app, "Create asio_thread.");
  asio_thread_ = websocketpp::lib::thread(&client::run, &client_);

}
void websocket_client::on_open(connection_hdl hdl) {
  client_.get_alog().write(websocketpp::log::alevel::app, "Connection opend, starting client! ");

  scoped_lock guard(lock_);
  open_ = true;
}
void websocket_client::on_close(connection_hdl hdl) {
  client_.get_alog().write(websocketpp::log::alevel::app, "Connection closed, stopping client! ");

  scoped_lock guard(lock_);
  done_ = true;
}
void websocket_client::on_fail(connection_hdl hdl) {
  client_.get_alog().write(websocketpp::log::alevel::app, "Connection failed, stopping client! ");

  scoped_lock guard(lock_);
  done_ = true;
}

void websocket_client::loop_() {
  uint64_t count = 0;
  std::stringstream val;
  websocketpp::lib::error_code ec;

  while(1) {
  bool wait = false;
  client_.get_alog().write(websocketpp::log::alevel::app, "wait for. " + val.str());
  {
    scoped_lock guard(lock_);
    if (done_) {break;}
    if (!open_) {
      wait = true;
    }
  }

  if (wait) {
    sleep(1);
    continue;
  }

  val.str("");
  val << "count is " << count++;

  client_.get_alog().write(websocketpp::log::alevel::app, val.str());
  client_.send(hdl_,val.str(),websocketpp::frame::opcode::text,ec);


  if (ec) {
    client_.get_alog().write(websocketpp::log::alevel::app,
    "Send Error: "+ec.message());
    break;
  }

  sleep(1);
  }
}

void websocket_client::set_logging() {
  client_.clear_access_channels(websocketpp::log::alevel::all);
  client_.set_access_channels(websocketpp::log::alevel::connect);
  client_.set_access_channels(websocketpp::log::alevel::disconnect);
  client_.set_access_channels(websocketpp::log::alevel::app);
  client_.set_access_channels(websocketpp::log::alevel::frame_payload);
  client_.set_error_channels(websocketpp::log::elevel::all);
}


#ifdef SDK_WEBSOCKET_TLS


void websocket_client_tls::run(const std::string& uri) {
  client_.get_alog().write(websocketpp::log::alevel::app, "websocket_client_tls::run");

  websocketpp::lib::error_code ec;
  client::connection_ptr con = client_.get_connection(uri, ec);
  if(ec) {
    client_.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + ec.message());
  }

  hdl_ = con->get_handle();
  client_.connect(con);

  client_.get_alog().write(websocketpp::log::alevel::app, "Create asio_thread.");
  asio_thread_ = websocketpp::lib::thread(&client::run, &client_);

}
void websocket_client_tls::on_open(connection_hdl hdl) {
  client_.get_alog().write(websocketpp::log::alevel::app, "Connection opend, starting client! ");

  scoped_lock guard(lock_);
  open_ = true;
}
void websocket_client_tls::on_close(connection_hdl hdl) {
  client_.get_alog().write(websocketpp::log::alevel::app, "Connection closed, stopping client! ");

  scoped_lock guard(lock_);
  done_ = true;
}
void websocket_client_tls::on_fail(connection_hdl hdl) {
  client_.get_alog().write(websocketpp::log::alevel::app, "Connection failed, stopping client! ");

  scoped_lock guard(lock_);
  done_ = true;
}

void websocket_client_tls::loop_() {
  uint64_t count = 0;
  std::stringstream val;
  websocketpp::lib::error_code ec;

  while(1) {
  bool wait = false;
  client_.get_alog().write(websocketpp::log::alevel::app, "wait for. " + std::to_string(open_));
  {
    scoped_lock guard(lock_);
    if (done_) {break;}
    if (!open_) {
      wait = true;
    }
  }

  if (wait) {
    sleep(1);
    continue;
  }

  val.str("");
  val << "count is " << count++;

  client_.get_alog().write(websocketpp::log::alevel::app, val.str());
  client_.send(hdl_,val.str(),websocketpp::frame::opcode::text,ec);


  if (ec) {
    client_.get_alog().write(websocketpp::log::alevel::app,
    "Send Error: "+ec.message());
    break;
  }

  sleep(1);
  }
}


void websocket_client_tls::set_logging() {
  client_.clear_access_channels(websocketpp::log::alevel::all);
  client_.set_access_channels(websocketpp::log::alevel::connect);
  client_.set_access_channels(websocketpp::log::alevel::disconnect);
  client_.set_access_channels(websocketpp::log::alevel::app);
  client_.set_access_channels(websocketpp::log::alevel::frame_payload);
  client_.set_error_channels(websocketpp::log::elevel::all);
}


websocket_client_tls::websocket_client_tls()
: open_(false), done_(false) {
  std::cout << __FUNCTION__ << std::endl;
  set_logging();

  client_.init_asio();

  // using websocketpp::lib::placeholders::_1;
  // using websocketpp::lib::bind;
  client_.set_open_handler(bind(&websocket_client_tls::on_open, this, _1));
  client_.set_close_handler(bind(&websocket_client_tls::on_close, this, _1));
  client_.set_fail_handler(bind(&websocket_client_tls::on_fail, this, _1));

  // init ssl
}
websocket_client_tls::~websocket_client_tls() {
  std::cout << __FUNCTION__ << std::endl;
  if(asio_thread_.joinable())
    asio_thread_.join();
  
  client_.get_alog().write(websocketpp::log::alevel::app, "~websocket_client_tls asio_thread_ over.");

}
void websocket_client_tls::init_tls(const std::string& hostname) {
  std::cout << __FUNCTION__ << std::endl;
  // using websocketpp::lib::placeholders::_1;
  client_.set_tls_init_handler(bind(&websocket_client_tls::on_tls_init, this, hostname.c_str(), _1));
}


bool verify_certificate(const char * hostname, bool preverified, boost::asio::ssl::verify_context& ctx) {
  std::cout << "verify_certificate ok" << std::endl;
  return true;
}

websocket_client_tls::context_ptr websocket_client_tls::on_tls_init(const char * hostname, connection_hdl) {

  std::cout << "on_tls_init call: " << hostname << std::endl;

  context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
  try {
    ctx->set_options(
      boost::asio::ssl::context::default_workarounds |
      boost::asio::ssl::context::no_sslv2 |
      boost::asio::ssl::context::no_sslv3 |
      boost::asio::ssl::context::single_dh_use);

    ctx->set_verify_mode(boost::asio::ssl::verify_peer);
    ctx->set_verify_callback(bind(&verify_certificate, hostname, _1, _2));
    ctx->load_verify_file("ca-chain.cert.pem");

  } catch(std::exception& e) {
    std::cout << "init_ssl exception: " << e.what() << std::endl;
  }

  return ctx;
}

#endif


int main(int argc, char* argv[]) {

try {
  websocket_client_tls client_ssl;
  websocket_client client;

  std::string hostname = "localhost";
  std::string port = "9002";
  std::string uri = "ws://localhost:9002";
  bool ssl = false;

  if(argc == 2) {
    uri = argv[1];
  }

  if(argc == 4) {
    hostname = argv[1];
    port = argv[2];
    ssl = static_cast<bool>(atoi(argv[3]));
  } else {
    std::cout << "Usage: websocket_client <hostname> <port> <ssl>" << std::endl;
    return 1;
  }

  uri = ssl ? "wss://" : "ws://";
  uri +=  hostname;
  uri += ":";
  uri += port;

  if(ssl) {
    std::cout << "start ssl client : " << ssl << ", " << uri << std::endl;
    
    client_ssl.init_tls(hostname);
    client_ssl.run(uri);

    std::cout << "Create local_thread." << std::endl;
    websocketpp::lib::thread local_thread(&websocket_client_tls::loop_, &client_ssl);

    std::cout << "local_thread::run wait." << std::endl;
    local_thread.join();
  } else {
    client.run(uri);
    std::cout << "Create local_thread." << std::endl;
    websocketpp::lib::thread local_thread(&websocket_client::loop_, &client);

    std::cout << "local_thread::run wait." << std::endl;
    local_thread.join();
  }

  

} catch(std::exception& e) {
  std::cout << "main std::exception..." << e.what() << std::endl;
} catch(...) {
  std::cout << "main fatal::exception..." << std::endl;
}
  std::cout << "main over." << std::endl;
  return 0;
}
