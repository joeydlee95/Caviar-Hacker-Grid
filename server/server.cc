
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
// http://think-async.com/asio/boost_asio_1_5_3/doc/html/boost_asio/example/local/stream_server.cpp

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <numeric>
#include <vector>

#include <boost/asio.hpp>
#include "../webserver.h"
#include "server.h"
#include "http.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "request_handler.h"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, HandlerConfiguration* handler)
  : socket_(std::move(socket)),
  handler_(handler) {
}

void Session::start() {
  do_read();
}

void Session::do_read() {
  auto self(shared_from_this());
  //trying to the size of incoming messege
  std::size_t buffer_size;
  //blocks the session until it gets a non-empty incoming string
  while((buffer_size = socket_.available()) == 0) {
    continue;
  }

  data_ = std::string(buffer_size, 0);
  socket_.async_read_some(boost::asio::buffer(&data_[0], data_.size()),
    [this, self](boost::system::error_code ec, std::size_t len) {
      if (!ec) {
        printf("Incoming Data length %lu:\n", len);
        // std::unique_ptr<Request> req = Request::Parse(data_);

        HandlerConfiguration h = *handler_;
        do_write();
      }
  });
}

void Session::do_write() {
  auto self(shared_from_this());
  std::string builder_string = "Temporary";
  boost::asio::async_write(socket_, boost::asio::buffer(&builder_string[0], builder_string.length()),
    [this, self](boost::system::error_code ec, std::size_t len) {
  });

  //session just ends after wrtie
}




Server::Server(boost::asio::io_service& io_service, int port, HandlerConfiguration* handler)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
  socket_(io_service){
  do_accept(handler);
}

void Server::do_accept(HandlerConfiguration* handler)
{
  acceptor_.async_accept(socket_,
  [this, handler](boost::system::error_code ec) {
    if (!ec) {
        std::make_shared<Session>(std::move(socket_), handler)->start();
    }

    // Continue accepting new connections
    do_accept(handler);
  });
}
