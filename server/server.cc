
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
#include "http_echo.h"
#include "http_404.h"
#include "http_file.h"
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
        bool match = false;
        printf("Incoming Data length %lu:\n", len);

        // Todo: convert this to a unique_ptr
        HttpRequest* request = new HttpRequest();
        if(!request->Parse(data_)) {
          printf("Invalid Request: Parse Error");
          // return an error code. low priority bug (400)
          builder = new http::HTTPResponseBuilder404(new http::HTTPResponse());
          builder->set_status_code(400);
          builder->set_reason_phrase(http::reason_phrase::getDefault(400));
          printf("404: \n%s\n", builder->getResult()->ToString().c_str());
        }
        else {
          //Todo: refactor this.
          printf("%s\n", request->getMethod().c_str());
          printf("%s\n", request->getResourcePath().c_str());
          std::string resource_path = request->getResourcePath();
          for(const auto & option : *options_) {
            
            // TODO: fix bug with subdirectories. This code doesn't find those properly, instead it finds the first matching prefix on the config
            auto res = std::mismatch(option.first.begin(), option.first.end(), resource_path.begin());
            if(res.first == option.first.end()) {
              printf("prefix config %s: http request %s\n", option.first.c_str(), resource_path.c_str());
              match = true;
              
              std::map<std::string, std::vector<std::string> >::iterator i;
              if((i = option.second.options_->find("echo")) != option.second.options_->end()) {
                //return an echo response
                
                builder = new http::HTTPResponseBuilderEcho(new http::HTTPResponse(), data_);
                printf("echoing: \n%s\n", builder->getResult()->ToString().c_str());
              }
              else if((i = option.second.options_->find("root")) != option.second.options_->end()) {
                std::string tail = resource_path.substr(option.first.size());
                printf("you should serve files from %s\n", (std::accumulate(i->second.begin(), i->second.end(), std::string(""))+tail).c_str());
                builder = new http::HTTPResponseBuilderFile(new http::HTTPResponse(), tail);
              }
              else {
                // invalid config
                printf("Unexpected lack of serving options\n"); 
                builder = new http::HTTPResponseBuilder404(new http::HTTPResponse());
                builder->set_status_code(500);
                builder->set_reason_phrase(http::reason_phrase::getDefault(500));
                printf("500: \n%s\n", builder->getResult()->ToString().c_str());
                //return 500
              }
              break;

            }
          }

          if(!match) {
            //send 404
            printf("path not found in config!\n");
            builder = new http::HTTPResponseBuilder404(new http::HTTPResponse());
            printf("404: \n%s\n", builder->getResult()->ToString().c_str());
          }


        }
        do_write(builder);
        delete request;
      }
  });
}

void Session::do_write(http::HTTPResponseBuilder* builder) {
  auto self(shared_from_this());
  std::string builder_string = builder->getResult()->ToString();
  boost::asio::async_write(socket_, boost::asio::buffer(&builder_string[0], builder_string.length()),
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
