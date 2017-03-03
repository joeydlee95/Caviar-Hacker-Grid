#ifndef WEBSERVER_H

#define WEBSERVER_H

#include "../nginx-configparser/config_parser.h"

#include <cstdlib>
#include <string>
#include <map>

#include "server.h"
#include "../handlers/request_handler.h"





class WebServer {
public:
  WebServer(NginxConfig* config) : config_(config) {
  }

  virtual bool Init();
  virtual bool run_server();
  virtual boost::system::error_code port_valid();

  std::string ToString() const;

  NginxConfig* config_;
  int port_ = 0;
private:
  ServerStatus status_;
  HandlerConfiguration HandlerMapping_;
  bool AddHandler(std::string path, std::string HandlerName, NginxConfig* const config);

};

#endif