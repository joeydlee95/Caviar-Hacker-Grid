#include "webserver.h"
#include <utility>
#include <map>
#include <numeric>
#include <boost/asio.hpp>


std::string WebserverOptions::ToString() const{
  std::string options_string;
  for(auto& option : *options_) {
    for(auto& vals : option) {
      options_string.append(vals.first);
      options_string.append(": ");
      for(auto& token : vals.second) {
        options_string.append(token);
        options_string.append(" ");
      }
      options_string.append("\n");
    }
    
  }
  // printf("%s", options_string.c_str());
  return options_string;
}

WebserverOptions::WebserverOptions(std::unique_ptr<NginxConfig> const &statement, std::vector<std::map<std::string, std::vector<std::string> > >* options) : options_(options) {
    if(statement.get() != nullptr) {
      for(const auto& s : statement->statements_) {
        std::string key = s->tokens_[0];
        std::vector<std::string> vals = std::vector<std::string>(s->tokens_.begin() + 1, s->tokens_.end());
        std::map<std::string, std::vector<std::string> > map;
        map.insert(std::make_pair(key, vals));
        options_->push_back(map);
      }
    }
}


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

bool Webserver::configure_server(const char* file_name) {
  if (!parser_->Parse(file_name, config_)) {
    printf("Invalid config file");
    return false;
  } 

  std::string port_str = "";

  if (!config_->find("port", port_str)) {
    printf("Config does not specify a port\n");
    return false;
  }

  port_ = std::atoi(port_str.c_str());

  boost::system::error_code ec = port_valid();
  if(ec.value() != boost::system::errc::success) {
    printf("Unable to bind port %d because %s\n", port_, ec.message().c_str());
    return false;
  }

  std::vector<std::shared_ptr<NginxConfigStatement> > statements = 
    config_->findAll("location");

  for (const auto& statement : statements) {
    // printf("%s", statement->ToString(0).c_str());
    if(statement->tokens_.size() != 2) {
      // lazy way to stringify the vector of strings
      printf("Invalid config %s", std::accumulate(statement->tokens_.begin(), statement->tokens_.end(), std::string("")).c_str());
      return false;
    }
    std::vector<std::map<std::string, std::vector<std::string> > >* options = new std::vector<std::map<std::string, std::vector<std::string> > >;
    WebserverOptions opt(statement->child_block_, options);
    printf("%s\n", opt.ToString().c_str());
    options_.insert(std::make_pair(statement->tokens_[1], opt));
  }
  return true;
}

bool Webserver::run_server(const char* file_name) {
  if(!configure_server(file_name))  {
    // Server cannot run
    return false;
  }
  try {  
    boost::asio::io_service io_service;
    Server s(io_service, port_);
    printf("Running server on port %d...\n", port_);
    io_service.run();
  } 
  catch (std::exception& e) {
    printf("Exception %s\n", e.what());
    return false;
  }
  return true;
}



