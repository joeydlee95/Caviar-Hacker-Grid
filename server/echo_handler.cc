#include "echo_handler.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "http.h"
#include <string>

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
