// Reference from: http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp11/allocation/server.cpp

#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class Server {
  public:
    Server(boost::asio::io_service& io_service, int port)
     : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      socket_(io_service) {
        accept();
      }
  private:
    void accept();

};
