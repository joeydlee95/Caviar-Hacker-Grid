#include "blocking_handler.h"

#include <string>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../http/http.h"

RequestHandler::Status BlockingHandler::Init(const std::string& uri_prefix,
				      const NginxConfig& config) {
  return OK;
}

RequestHandler::Status BlockingHandler::HandleRequest(const Request& request,
					       Response* response) {
  for(;;) {
    continue;
  }
  return OK;
}
