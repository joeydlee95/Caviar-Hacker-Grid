#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H


#include "request_handler.h"

#include <string>

#include "../nginx-configparser/config_parser.h"
#include "../http/httpRequest.h"
#include "../http/httpResponse.h"


class NotFoundHandler: public RequestHandler {
 public: 
  Status Init(const std::string& uri_prefix,
        const NginxConfig& config);
  
  Status HandleRequest(const Request& request,
           Response* response);
};

REGISTER_REQUEST_HANDLER(NotFoundHandler);


#endif
