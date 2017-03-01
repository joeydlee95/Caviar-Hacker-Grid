#include "httpMutableRequest.h"
#include <iostream>
#include <sstream>

void MutableRequest::SetURI(const std::string& uri) {
	request_uri_ = uri;
}
void MutableRequest::SetHeader(const std::string& header_name, const std::string& header_value) {
	for (auto& header : headers_) {
		if (header.first == header_name) {
			header.second = header_value;
			return;
		}
	}
	// insert if it didn't exist
	headers_.push_back({header_name, header_value});
}

std::string MutableRequest::ToString() {
	std::string CRLF = "\r\n";

	std::stringstream request_stream;
	request_stream << method() << " " << uri() << " " + version() << CRLF;
	for (auto& header : headers_) {
		request_stream << header.first << ": " << header.second << CRLF;
	}
	request_stream << CRLF;
	request_stream << body();

	return request_stream.str();
}

MutableRequest::MutableRequest(const Request& other)
	: Request(other)
{}
