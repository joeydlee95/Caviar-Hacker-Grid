#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
class HttpRequest{
public:
	bool Parse(const std::string request);

	//get the message body at the end if there is any
	std::string getMessageBody();
	
	std::string getMethod();
	std::string getResourcePath();
	std::string getVersion();
	
	//string constants for HTTP request Methods, for now it only has GET
	static const std::string GET;

	//for example, we have "Accept-Languages: en-us" in the headers
	//just do header_fields[Accept-Languages]
	//notice that it will return " en-us" with the space for now
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
