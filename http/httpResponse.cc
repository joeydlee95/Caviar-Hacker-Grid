#include "httpResponse.h"
#include <map>
void Response::SetStatus(const ResponseCode response_code){
	this->status_code_ = response_code;
	this->reason_phrase_ = http::reason_phrase::getDefault(static_cast<int>(response_code));
}
int Response::GetStatus() {
	return static_cast<int>(this->status_code_);
}

void Response::AddHeader(const std::string& header_name, const std::string& header_value){
	this->http_headers_.fields_[header_name] = header_value;
}

void Response::SetBody(const std::string& body){
	this->body_ = body;
}

std::string Response::ToString(){
	std::string serialized_resp;
  	serialized_resp.append(this->http_version_ + " " + std::to_string(static_cast<int>(this->status_code_)) + " " + this->reason_phrase_);
  	serialized_resp.append(this->line_break);
  	for(const auto & header : this->http_headers_.fields_) {
	    serialized_resp.append(header.first);
	    serialized_resp.append(": ");
	    serialized_resp.append(header.second);
	    serialized_resp.append(line_break);;
	}

	serialized_resp.append(this->line_break);
	serialized_resp.append(this->body_);
	return serialized_resp;
}
