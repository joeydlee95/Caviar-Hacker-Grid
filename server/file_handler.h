#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include "../nginx-configparser/config_parser.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "request_handler.h"
#include "../filesystem/file_opener.h"

class StaticFileHandler: public RequestHandler{

public:
	//will only use the root path for now
	//it assumes that config is the block for this handler only, more specifically:
	//     path /static StaticFileHandler{
	//       root /path1;
	//     }
	//config should contains the "root /path1;"
	//uri_prefix in this case should be /static
	virtual Status Init(const std::string& uri_prefix, const NginxConfig& config);

	//error 500 not used yet
	
	//if the requested uri is : /static1/text_file.txt
	//the prefix is /static1
	//and the root path is /foo
	//it will look for the file: /foo/text_file.txt
	virtual Status HandleRequest(const Request& request, Response* response);

	std::string getPrefix(){
		return this->m_uri_prefix_;
	}
	std::string getRoot(){
		return this->m_root_path_;
	}
private:
	std::string m_uri_prefix_;
	std::string m_root_path_;

};

REGISTER_REQUEST_HANDLER(StaticFileHandler);


#endif

