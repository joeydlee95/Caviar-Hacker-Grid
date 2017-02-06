#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
class HttpRequest{
public:
	bool Parse(const std::string request);
	std::string getMessageBody();
	std::string getMethod();
	std::string getResourcePath();
	std::string getVersion();
	std::string getField(std::string header);

	static const std::string GET;
	std::map<std::string,std::string> header_fields;

private:
	bool processRequestLine(const std::string request);
	bool processMessageBody(const std::string request);
	bool processHeaders(const std::string request);
	std::string request_line;
	std::string message_body;
	

	std::string method;
	std::string version;
	std::string request_uri;	
};


#endif
