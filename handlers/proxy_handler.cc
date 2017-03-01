#include "proxy_handler.h"

#include <string>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../http/http.h"

const int MAX_PORTNUM = 65535;

RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
  	this->m_uri_prefix_ = uri_prefix;
	// Assume the config argument has something like:
	// host www.example.com
	// port #####.
	std::vector<std::string> host_config_tokens = config.find("host");
	if (host_config_tokens.size() != 2){
	  	printf("ProxyHandler.Init: Invalid config:\n%s", config.ToString().c_str() );
	  	return INVALID_CONFIG;
	}
	this->m_host_path_ = host_config_tokens[1];

	std::vector<std::string> port_config_tokens = config.find("port");
	if (port_config_tokens.size() != 2){
		printf("ProxyHandler.Init: Invalid config:\n%s", config.ToString().c_str() );
		return INVALID_CONFIG;
	}
	this->m_port_path_ = port_config_tokens[1];
	
	return OK;
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_TEXT_PLAIN));
  response->SetBody((request.raw_request()).c_str());
  return OK;
}