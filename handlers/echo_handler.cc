#include "echo_handler.h"

#include <string>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../http/http.h"


RequestHandler::Status EchoHandler::Init(const std::string& uri_prefix,
              const NginxConfig& config) {
  return OK;
}

RequestHandler::Status EchoHandler::HandleRequest(const Request& request,
               Response* response) {
  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_TEXT_PLAIN));
  response->SetBody((request.raw_request()).c_str());
  return OK;
}
