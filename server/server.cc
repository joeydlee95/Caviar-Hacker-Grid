
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
// http://think-async.com/asio/boost_asio_1_5_3/doc/html/boost_asio/example/local/stream_server.cpp

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <vector>
#include "server.h"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket)
  : socket_(std::move(socket)) {
}

void Session::start() {
  do_read();
}

void Session::do_read() {


  auto self(shared_from_this());
  data_ = std::string(socket_.available(), 0);
  socket_.async_read_some(boost::asio::buffer(&data_[0], data_.size()),
    [this, self](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        printf("Incoming Data length %lu:\n", length);

        for (std::size_t i = 0; i < length; i++) {
            printf("%c", data_[i]);
        }

        do_write(length);
      }
  });
}

void Session::do_write(std::size_t length) {
  auto self(shared_from_this());
  std::string header_string = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  data_.insert(0, header_string);
  boost::asio::async_write(socket_, boost::asio::buffer(&data_[0], length + header_string.length()),
    [this, self](boost::system::error_code ec, std::size_t len) {
      if (!ec) {
        printf("Outgoing Data:\n");
        for (std::size_t i = 0; i < len; i++) {
            printf("%c", data_[i]);
        }
      }
  });
}




Server::Server(boost::asio::io_service& io_service, int port)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
  socket_(io_service) {
  do_accept();
}

void Server::do_accept()
{
  acceptor_.async_accept(socket_,
  [this](boost::system::error_code ec) {
    if (!ec) {
        std::make_shared<Session>(std::move(socket_))->start();
    }

    do_accept();
  });
}
