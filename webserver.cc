#include "webserver.h"
#include "webserver_options.h"
#include <utility>
#include <map>
#include <numeric>
#include <boost/asio.hpp>

std::string Webserver::ToString() const{
  std::string webserver_string;
  webserver_string.append("port: " + std::to_string(port_) + " \n");
  for(const auto& option : options_) {
    webserver_string.append("key: ");
    webserver_string.append(option.first);
    webserver_string.append(", value: ");
    webserver_string.append(option.second.ToString());
    webserver_string.append("\n");
  }
  return webserver_string;
}

boost::system::error_code Webserver::port_valid() {
  // Based off of: 
  // http://stackoverflow.com/questions/33358321/using-c-and-boost-or-not-to-check-if-a-specific-port-is-being-used
  using namespace boost::asio;
  using ip::tcp;

  io_service svc;
  tcp::acceptor a(svc);

  boost::system::error_code ec;
  a.open(tcp::v4(), ec) || a.bind({ tcp::v4(), (unsigned short)port_ }, ec);

  return ec;
}

bool Webserver::Init() {
  std::vector<std::string> portTokens = config_->find("port");
  if (portTokens.size() != 2) {
    printf("Config does not specify a port\n");
    return false;
  }

  // port should be in the format of port ______;
  port_ = std::atoi(portTokens[1].c_str());

  std::vector<std::shared_ptr<Nginx::NginxConfig> > statements = 
    config_->findAll("path");

  for (const auto& statement : statements) {
    if(statement->tokens_.size() != 3) {
      // lazy way to stringify the vector of strings
      printf("Invalid path block %s\n", std::accumulate(statement->tokens_.begin(), statement->tokens_.end(), std::string("")).c_str());
      return false;
    }

    // TODO: Replace this with the request handler. 
    std::map<std::string, std::vector<std::string> >* options = new std::map<std::string, std::vector<std::string> >;
    //token[0] = path, token[1] = <URL>, token[3] = <handler type>, childblock = additional options
    WebserverOptions opt(statement, options);
    printf("Options registered: %s\n", opt.ToString().c_str());
    //pair: /static -> root nginx-configparser;
    
    options_.insert(std::make_pair(statement->tokens_[1], opt));
  }

  std::vector<std::string> defaultTokens = config_->find("default");  
  if (portTokens.size() != 2) {
    printf("Config does not specify a port\n");
    return false;
  }

  boost::system::error_code ec = port_valid();
  if(ec.value() != boost::system::errc::success) {
    printf("Unable to bind port %d because %s\n", port_, ec.message().c_str());
    return false;
  }

  return true;
}

bool Webserver::run_server() {
  try {  
    boost::asio::io_service io_service;
    Server s(io_service, port_, &options_);
    printf("Running server on port %d...\n", port_);
    io_service.run();
  } 
  catch (std::exception& e) {
    printf("Exception %s\n", e.what());
    return false;
  }
  return true;
}
