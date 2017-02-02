#include "webserver.h"
#include <boost/asio.hpp>


bool Webserver::run_server(const char* file_name){

  
  if (!parser_->Parse(file_name, config_)) {
    printf("Invalid config file");
    return false;
  }


  std::string port_str = "port";
  
  if (!config_->find(port_str)) {
    printf("Config does not specify a port");
    return false;
  }
  
  int port = std::atoi(port_str.c_str());
  if(port < 1024 || port > 65535) {
    printf("Invalid port %d", port);
    return 1;
  }
  
  try {

    boost::asio::io_service io_service;
    Server s(io_service, port);
    printf("Running server on port %s...\n", port_str.c_str());
    io_service.run();
  } 
  catch (std::exception& e) {
    printf("Exception %s\n", e.what());
    return false;
  }
  return true;
}



