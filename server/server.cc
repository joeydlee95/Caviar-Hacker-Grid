
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        printf("Incoming Data:\n");
        for (std::size_t i = 0; i < length; i++) {
            printf("%c", data_[i]);
        }

        do_write(length);
      }
  });
}

void Session::do_write(std::size_t length) {
  auto self(shared_from_this());

  boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
    [this, self](boost::system::error_code ec, std::size_t len) {
      if (!ec) {
        printf("Outgoing Data:\n");
        for (std::size_t i = 0; i < len; i++) {
            printf("%c", data_[i]);
        }

        do_read();
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
