#include "webserver.h"
#include <boost/asio.hpp>
bool Webserver::parse_config(const char* file_name){
  if (!parser_->Parse(file_name, config_)) {
    printf("Invalid config file");
    return false;
  }
  return true;

}

bool Webserver::get_port_from_config(std::string& s){
  if (!config_->find(s)) {
    printf("Config does not specify a port");
    return false;
  }
  return true;
}

bool Webserver::run_server(const char* file_name){

  if(!this->parse_config(file_name)){
    return false;
  }
  std::string port_str;
  if(!this->get_port_from_config(port_str)){
    return false;
  }
  try {
    boost::asio::io_service io_service;
    Server s(io_service, std::atoi(port_str.c_str()));
    printf("Running server on port %s...\n", port_str.c_str());
    io_service.run();
  } 
  catch (std::exception& e) {
    printf("Exception %s\n", e.what());
    return false;
  }
  return true;
}



