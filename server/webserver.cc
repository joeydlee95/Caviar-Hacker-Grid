#include "webserver.h"

#include <utility>
#include <map>
#include <numeric>
#include <boost/asio.hpp>

#include "../handlers/request_handler.h"
#include "../handlers/status_handler.h"


std::string WebServer::ToString() const {
  std::string webserver_string;
  webserver_string.append("port: " + std::to_string(port_) + " \n");
  for(auto const& handler : *HandlerMapping_.RequestHandlers) {
    webserver_string.append("Handler registered: " + handler.first + "\n");
  }
  return webserver_string;
}

bool WebServer::AddHandler(std::string path, std::string HandlerName, NginxConfig* config) {
  auto handler = RequestHandler::CreateByName(HandlerName.c_str());
  if(handler == nullptr) {
    printf("Invalid Handler %s\n", HandlerName.c_str());
    return false;
  }
  if(HandlerName == "StatusHandler") {
    StatusHandler* statusHandler = dynamic_cast<StatusHandler*>(handler);
    statusHandler->InitStatus(&status_);
    // handler = f;
  }

  RequestHandler::Status s = handler->Init(path, *config);
  
  if(s == RequestHandler::INVALID_CONFIG) {
    printf("Error initializing Handler %s due to invalid config %s\n", HandlerName.c_str(), config->ToString().c_str());
    return false;
  }

  printf("Registered Handler %s to path %s\n", HandlerName.c_str(), path.c_str());
  auto res = HandlerMapping_.RequestHandlers->insert(std::make_pair(path, handler));
  if(res.second == false) {
    printf("Failed to insert %s with Handler %s because path already exists\n", path.c_str(), HandlerName.c_str());
    return false;
  }
  status_.AddHandler(path, HandlerName);
  return true;

}

boost::system::error_code WebServer::port_valid() {
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

bool WebServer::Init() {
  std::vector<std::string> portTokens = config_->find("port");
  if (portTokens.size() != 2) {
    printf("Config does not specify a port\n");
    return false;
  }

  // port should be in the format of port ______;
  port_ = std::atoi(portTokens[1].c_str());
  if(port_ > 65535) {
    printf("Port number is greater than range. Valid port numbers: 0-65535, where 0-1024 require root access\n");
    return false;
  }
  HandlerMapping_.RequestHandlers = new HandlerMap;

  std::vector<std::shared_ptr<NginxConfig> > statements = 
    config_->findAll("path");

  for (const auto& statement : statements) {
    if(statement->tokens_.size() != 3) {
      // lazy way to stringify the vector of strings
      printf("Invalid path block %s\n", std::accumulate(statement->tokens_.begin(), statement->tokens_.end(), std::string("")).c_str());
      return false;
    }
    AddHandler(statement->tokens_[1], statement->tokens_[2], statement.get());
  }

  std::vector<std::string> defaultTokens = config_->find("default");  
  if (defaultTokens.size() != 2) {
    printf("Config does not specify a default handler\n");
    return false;
  }


  HandlerMapping_.DefaultHandler = RequestHandler::CreateByName(defaultTokens[1].c_str());
  if(HandlerMapping_.DefaultHandler == nullptr) {
    printf("Default Handler Invalid: %s specified, not found\n", defaultTokens[1].c_str());
    return false;
  }
  printf("Registered Default Handler %s\n", defaultTokens[1].c_str());
  
  status_.SetDefaultHandler(defaultTokens[1]);

  boost::system::error_code ec = port_valid();
  if(ec.value() != boost::system::errc::success) {
    printf("Unable to bind port %d because %s\n", port_, ec.message().c_str());
    return false;
  }

  return true;
}

bool WebServer::run_server() {
  try {  
    boost::asio::io_service io_service;
    Server s(io_service, port_, &HandlerMapping_, &status_);
    printf("Running server on port %d...\n", port_);
    io_service.run();
  } 
  catch (std::exception& e) {
    printf("Exception %s\n", e.what());
    return false;
  }
  return true;
}
