#include "not_found_handler.h"

#include <string>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../http/http.h"


RequestHandler::Status NotFoundHandler::Init(const std::string& uri_prefix,
				      const NginxConfig& config) {
  return OK;
}


RequestHandler::Status NotFoundHandler::HandleRequest(const Request& request,
					       Response* response) {
  response->SetStatus(Response::NOT_FOUND);
  response->AddHeader("Content-Type", http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_TEXT_PLAIN));
  return FILE_NOT_FOUND;
}
