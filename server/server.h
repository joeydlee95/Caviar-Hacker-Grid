
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp


#ifndef SERVER_H

#define SERVER_H

#include <vector>
#include <string>
#include <map>

#include <boost/asio.hpp>

#include "../http/httpRequest.h"
#include "../http/http.h"
#include "../handlers/request_handler.h"

using boost::asio::ip::tcp;

class Session
  : public std::enable_shared_from_this<Session>
{
public:
  Session(tcp::socket socket, HandlerConfiguration* handler);
  void start();

private:
  void do_read();

  void do_write(const std::string resp);

  tcp::socket socket_;
  std::string data_;
  HandlerConfiguration* handler_;
};

class Server 
{
public:
  Server(boost::asio::io_service& io_service, int port, HandlerConfiguration* handler);

private:
  void do_accept(HandlerConfiguration* handler);
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

#endif // If Guard
