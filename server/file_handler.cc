#include "file_handler.h"
#include <vector>
#include <string.h>
#include <cstdio>
#include "../http/http.h"
#include "not_found_handler.h"

RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config){
	this->m_uri_prefix_ = uri_prefix;
	//assume the config argument has something like
	// root /path1
	//check if such a line exists
	std::vector<std::string> root_config_tokens = config.find("root");
	if (root_config_tokens.size() < 2){
		printf("StaticHandler.Init: Invalid config:\n%s", config.ToString().c_str() );
		return INVALID_CONFIG;
	}

	this->m_root_path_ = root_config_tokens[1];
	return OK;
}

RequestHandler::Status StaticHandler::HandleRequest(const Request& request, Response* response){
	std::string req_uri = request.uri();
	std::size_t prefix_pos = req_uri.find(this->m_uri_prefix_);

	//the server should check for this first, this is just for safety
	if (prefix_pos == std::string::npos || prefix_pos != 0){
		printf("StaticHandler.HandleRequest: Bad request");
		response->SetStatus(Response::BAD_REQUEST);
		response->AddHeader("Content-Type",http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_TEXT_HTML));
		return BAD_REQUEST;
	}

	std::string uri_no_prefix = req_uri.substr(this->m_uri_prefix_.size());
	std::string actual_uri = this->m_root_path_ + uri_no_prefix;

	if (!FileIO::FileExists(actual_uri)){
		printf("StaticHandler.HandleRequest: File not found:%s\n",actual_uri.c_str());
		return NotFoundHandler_.HandleRequest(request, response);
	}

	response->SetStatus(Response::OK);

	// find the file extension, set the content type based on this.
    std::size_t index_of_extension = FileIO::FileExtensionLocation(actual_uri);
    if (index_of_extension == std::string::npos){
    	response->AddHeader("Content-Type",http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_APP_OCTET_STREAM));
    }
    else {
      std::string file_extension = actual_uri.substr(index_of_extension + 1);
      response->AddHeader("Content-Type",http::mime_type::ContentTypeAsString(http::mime_type::GetMimeType(file_extension)));
    }
    
    std::string body;
    FileIO::FileToString(actual_uri, body);
    response->SetBody(body);

    return OK;
}