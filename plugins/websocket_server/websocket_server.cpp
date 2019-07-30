#include "websocket_server.hpp"


using namespace sdk;
using namespace plugins;

websocket_server::websocket_server(): next_sessionid_(1) {
  std::cout << __FUNCTION__ << std::endl;
  server_.init_asio();
  server_.set_open_handler(bind(&websocket_server::on_open, this, ::_1));
  server_.set_close_handler(bind(&websocket_server::on_close, this, ::_1));
  server_.set_message_handler(bind(&websocket_server::on_message, this, ::_1, ::_2));
}
websocket_server::~websocket_server() {
  std::cout << __FUNCTION__ << std::endl;
}

void websocket_server::on_open(connection_hdl hdl) {
  std::cout << __FUNCTION__ << std::endl;

  connection_ptr con = server_.get_con_from_hdl(hdl);
  con->sessionid = next_sessionid_++; // custom data
}
void websocket_server::on_close(connection_hdl hdl) {
  std::cout << __FUNCTION__ << std::endl;

  connection_ptr con = server_.get_con_from_hdl(hdl);
  std::cout << "Closing connection " << con->name
            << " with sessionid " << con->sessionid << std::endl;
}
void websocket_server::on_message(connection_hdl hdl, server::message_ptr msg) {
  std::cout << __FUNCTION__ << std::endl;
  connection_ptr con = server_.get_con_from_hdl(hdl);
  
  if (con->name.empty()) {
    con->name = msg->get_payload();
    std::cout << "Setting name of connection with sessionid " 
    << con->sessionid << " to " << con->name << std::endl;
  } else {
    std::cout << "Got a message from connection " << con->name 
    << " with sessionid " << con->sessionid << std::endl;
  }
}
void websocket_server::run(uint16_t port) {
  std::cout << __FUNCTION__ << std::endl;
  server_.listen(port);
  server_.start_accept();
  server_.run();
}

#ifndef TEST_MAIN
int main() {
  websocket_server server;
  server.run(9002);
}
#endif