#include "webserver.h"
#include <utility>
#include <map>
#include <boost/asio.hpp>


std::string WebserverOptions::ToString() {
  std::string options_string;
  for(auto& option : options_) {
    // for(auto& vals : option.second) {
    //   options_string.append(vals.first);
    //   options_string.append(": ");
    //   options_string.append(vals.second);
    //   options_string.append("\n");
    // }
    printf("option: %s", option.first.c_str());
  }
  return options_string;
}

WebserverOptions::WebserverOptions(NginxConfigStatement* statement) {
    if(statement->child_block_.get() != nullptr) {
      for(const auto& s : statement->child_block_.get()->statements_) {
        options_.insert(
            std::make_pair(
                s->tokens_[0], 
                std::vector<std::string>(
                        s->tokens_.begin() + 1, 
                        s->tokens_.end()
                                        )
                          )
        );
      }
    }

}


boost::system::error_code Webserver::port_valid() {
  // Based off of: 
  // http://stackoverflow.com/questions/33358321/using-c-and-boost-or-not-to-check-if-a-specific-port-is-being-used
  using namespace boost::asio;
  using ip::tcp;

  io_service svc;
  tcp::acceptor a(svc);

  boost::system::error_code ec;
  a.open(tcp::v4(), ec) || a.bind({ tcp::v4(), (short)port_ }, ec);

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

  // Note that this currently doesn't validate the input.
  for(auto& statement: statements) {
    NginxConfigStatement* st = statement.get();
    // if(st->tokens_.size() != 2)
      // printf("Invalid config %s", std::string(st->tokens_.begin(), st->tokens_.end()).c_str());
      // return false;
    // }
    WebserverOptions opt = WebserverOptions(st->child_block_);
    options_.insert(st->tokens_[1], opt);
  }
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



