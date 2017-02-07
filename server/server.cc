
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
// http://think-async.com/asio/boost_asio_1_5_3/doc/html/boost_asio/example/local/stream_server.cpp

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "../webserver.h"
#include <vector>
#include "server.h"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, std::map<std::string, WebserverOptions>* options)
  : socket_(std::move(socket)), options_(options) {
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

        HttpRequest* request = new HttpRequest();
        if(request->Parse(data_)){
          // do_write(len);
          printf("%s\n", request->getMethod().c_str());
          printf("%s\n", request->getResourcePath().c_str());
          for(const auto & option : *options_) {
          //   if(request->getResourcePath().compare(option.first) == 0) {
          //     // match
          //     std::string behavior = option.second.(*options_)[0]

          //     break;
          //   }
            printf("option: %s, val: %s\n", option.first.c_str(), option.second.ToString().c_str());
          }
        }
        else{
          printf("Invalid Request: Parse Error");
        }
        delete request;
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

  //session just ends after wrtie
}




Server::Server(boost::asio::io_service& io_service, int port, std::map<std::string, WebserverOptions>* options)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
  socket_(io_service) {
  do_accept(options);
}

void Server::do_accept(std::map<std::string, WebserverOptions>* options)
{
  acceptor_.async_accept(socket_,
  [this, options](boost::system::error_code ec) {
    if (!ec) {
        std::make_shared<Session>(std::move(socket_), options)->start();
    }

    do_accept(options);
  });
}
