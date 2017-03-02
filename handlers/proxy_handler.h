#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include <string>
#include "../nginx-configparser/config_parser.h"
#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "request_handler.h"

class ProxyHandler : public RequestHandler {
 public: 
  	Status Init(const std::string& uri_prefix, const NginxConfig& config);
  	Status HandleRequest(const Request& request, Response* response);

  	Status SendRequestToServer(const std::string& host, const std::string& port, const Request& req, Response* resp, int depth=10);
	void ParseRedirectLocation(std::string location, std::string* new_path, std::string* new_host);

 private:
    std::string m_uri_prefix_;
	std::string m_host_path_;
	std::string m_port_path_;
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

#endif
