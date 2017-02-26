#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include <string>
#include "../nginx-configparser/config_parser.h"
#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "request_handler.h"
#include "../server/server.h"

class StatusHandler : public RequestHandler {
 public: 
  Status Init(const std::string& uri_prefix,
              const NginxConfig& config);
  
  Status InitStatus(ServerStatus* status);

  Status HandleRequest(const Request& request,
                       Response* response);
  std::string StatusToHtml(const ServerStatus::Status& status);
  private:
    ServerStatus* status_;
};

REGISTER_REQUEST_HANDLER(StatusHandler);


#endif
