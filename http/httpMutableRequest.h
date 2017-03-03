#ifndef HTTPMUTABLEREQUEST_H
#define HTTPMUTABLEREQUEST_H

#include <string>
#include "httpRequest.h"

// an httpRequet that we allow to be modified
class MutableRequest : public Request {
 public:
	MutableRequest(const Request& other);
	void SetURI(const std::string& uri);
	void SetHeader(const std::string& header_name, const std::string& header_value);
	std::string ToString();
};

#endif

