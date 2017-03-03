#include "status_handler.h"

#include <string>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../http/http.h"


RequestHandler::Status StatusHandler::Init(const std::string& uri_prefix,
              const NginxConfig& config) {
  return OK;
}


RequestHandler::Status StatusHandler::HandleRequest(const Request& request,
                 Response* response) {
  if (status_ == nullptr) {
    return RequestHandler::MISCONFIGURED_HANDLER;
  }

  ServerStatus::Status status = status_->GetStatus();


  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_TEXT_HTML));
  response->SetBody(StatusToHtml(status));
  return OK;
}


RequestHandler::Status StatusHandler::InitStatus(ServerStatus* status) {
  if (status == nullptr) {
    return RequestHandler::MISCONFIGURED_HANDLER;
  }
  status_ = status;
  return OK;
}



std::string StatusHandler::StatusToHtml(const ServerStatus::Status& status) {
  std::string html_string;

  html_string.append("<!DOCTYPE HTML>\n");
  html_string.append("<html>\n");
  html_string.append("<head>Status Page</head>");
  html_string.append("<body bgcolor=\"FFFFFF\">\n");
  html_string.append("<h1>Status Page</h1>\n");
  html_string.append("<h2>Requests Received</h2>\n");
  html_string.append("<p> Total Requests Received: " + std::to_string(status.requests_) + "</p>");
  html_string.append("<p> Requests broken down by URL:");
  html_string.append("<ul>\n");
  for(auto &pair : status.RequestCountByURL_) {
    html_string += "<li>"+ pair.first + " : " + std::to_string(pair.second) + "</li>\n";
  }
  html_string.append("</ul></p>\n");
  html_string.append("<p> Requests broken down by Response Code: ");
  html_string.append("<ul>\n");
  for(auto &pair : status.ResponseCountByCode_) {
    html_string += "<li>"+ std::to_string(pair.first) + " : " + std::to_string(pair.second) + "</li>\n";
  }
  html_string.append("</ul></p>\n");

  html_string.append("<h2>Handlers</h2>\n");
  
  html_string.append("<p><ul>\n");
  for(auto &pair : status.RequestHandlers_) {
    html_string += "<li> "+ pair.first + " : " + pair.second + "</li>\n";
  }
  html_string.append("</ul></p>\n");

  html_string.append("<p> Default handler: " + status.defaultHandler_ + "</p>");

  html_string.append("</body>\n");
  html_string.append("</html>\n");

  return html_string;
}
