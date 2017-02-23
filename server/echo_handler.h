#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include <string>
#include "../nginx-configparser/config_parser.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "request_handler.h"

class EchoHandler: public RequestHandler {
  
  Status Init(const std::string& uri_prefix,
	      const NginxConfig& config);
  
  Status HandleRequest(const Request& request,
		       Response* response);
};

#endif
