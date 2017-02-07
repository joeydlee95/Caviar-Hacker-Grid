#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
class HttpRequest{
public:
	//should call this method before calling other member functions
	bool Parse(const std::string request);
	//clear every memebr variable, should be called before processing a new request
	void Clear();
	std::string getMessageBody();	
	std::string getMethod();
	std::string getResourcePath();
	std::string getVersion();
	std::string getRawRequest();
	// HTTP Keywords
	static const std::string GET;

	//for example, we have "Accept-Languages: en-us" in the headers
	//just do header_fields[Accept-Languages]
	//notice that it will return "en-us"
	std::map<std::string,std::string> header_fields_;

private:
	//should only be called by Parse
	bool processRequestLine(const std::string request);
	bool processMessageBody(const std::string request);
	bool processHeaders(const std::string request);

	std::string raw_request_;
	std::string request_line_;
	std::string message_body_;
	std::string method_;
	std::string version_;
	std::string request_uri_;	
};


#endif
