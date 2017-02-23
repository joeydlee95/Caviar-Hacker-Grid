
// based off of: http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp


#ifndef SERVER_H

#define SERVER_H

#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "httpRequest.h"
#include <map>
#include "../webserver_options.h"
#include "http.h"

using boost::asio::ip::tcp;


class Session
  : public std::enable_shared_from_this<Session>
{
public:
  Session(tcp::socket socket);
  void start();

private:
  void do_read();

  void do_write();

  tcp::socket socket_;
  enum { max_length = 1024 };
  std::string data_;
};

class Server 
{
public:
  Server(boost::asio::io_service& io_service, int port);

private:
  void do_accept();
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

#endif // If Guard
