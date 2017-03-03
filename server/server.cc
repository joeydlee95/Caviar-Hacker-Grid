
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
// http://think-async.com/asio/boost_asio_1_5_3/doc/html/boost_asio/example/local/stream_server.cpp


#include "server.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <numeric>
#include <vector>

#include <boost/asio.hpp>

#include "webserver.h"
#include "../http/http.h"
#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../handlers/request_handler.h"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, HandlerConfiguration* handler, ServerStatus* status)
  : socket_(std::move(socket)),
  handler_(handler),
  status_(status) {
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
        std::unique_ptr<Request> req = Request::Parse(data_);
        Response* response = new Response;
        std::string response_string;
        if(req.get()) {
          // response valid 
          printf("request key: %s\n", req->uri().c_str());
          
          std::string best_key = handler_->FindLongestHandlerKey(req->uri());
          RequestHandler::Status RequestStatus;

          if(best_key == "") {
            // use the default handler, as no key was found for the URI
            printf("no key found, 404ing\n");
            RequestStatus = handler_->DefaultHandler->HandleRequest(*req, response);
          }
          else {
            printf("key %s found\n", best_key.c_str());
            printf("key in config: %s\n", handler_->RequestHandlers->find(best_key)->first.c_str());
            // do_write(handler_->DefaultHandler);
            RequestStatus = handler_->RequestHandlers->find(best_key)->second->HandleRequest(*req, response);
          }
          

          if(RequestStatus == RequestHandler::BAD_REQUEST) {
            response_string = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
            status_->LogIncomingRequest(req->uri(), 500);
          }
          else {
            response_string = response->ToString();
            status_->LogIncomingRequest(req->uri(), response->GetStatus());
          }
        } else {
          // response invalid, return a 400
          response_string = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n";
          status_->LogIncomingRequest(req->uri(), 400);
        }

        do_write(response_string);
        delete response;
      }
  });
}

void Session::do_write(const std::string resp) {
  auto self(shared_from_this());
  boost::asio::async_write(socket_, boost::asio::buffer(&resp[0], resp.length()),
    [this, self](boost::system::error_code ec, std::size_t len) {
  });

  //session just ends after wrtie
}


Server::Server(boost::asio::io_service& io_service, int port, HandlerConfiguration* handler, ServerStatus* status)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
  socket_(io_service){
  do_accept(handler, status);
}

void Server::do_accept(HandlerConfiguration* handler, ServerStatus* status)
{
  acceptor_.async_accept(socket_,
  [this, handler, status](boost::system::error_code ec) {
    if (!ec) {
        std::make_shared<Session>(std::move(socket_), handler, status)->start();
    }

    // Continue accepting new connections
    do_accept(handler, status);
  });
}

void ServerStatus::AddHandler(std::string path, std::string handler) {
  Status_.RequestHandlers_.insert(std::make_pair(path, handler));
}

ServerStatus::Status ServerStatus::GetStatus() {
  return Status_;
}

void ServerStatus::LogIncomingRequest(std::string url, int RespCode) {
  auto RespCodeCount = Status_.ResponseCountByCode_.insert(std::make_pair(RespCode, 1));
	if (RespCodeCount.second == false) { // The particular response code is already in the map
		RespCodeCount.first->second++;
	}

  auto URLCodeCount = Status_.RequestCountByURL_.insert(std::make_pair(url, 1));
	if (URLCodeCount.second == false) { // The particular url is already in the map
		URLCodeCount.first->second++;
	}

	Status_.requests_++;
}

void ServerStatus::SetDefaultHandler(std::string handler) {
  Status_.defaultHandler_ = handler;
}
