#include "httpRequest.h"
#include <string.h>
#include <vector>
#include <cstdio>
const std::string HttpRequest::GET = "GET";

bool HttpRequest::processRequestLine(const std::string request){
	int end_request_ln_idx = request.find("\r\n");
	if(end_request_ln_idx==std::string::npos){
		printf("Invalid Request Line CRLF");
		return false;
	}
	std::string request_line = request.substr(0,end_request_ln_idx);
	this->request_line = request_line;

	char* token = strtok(&request_line[0], " ");
	std::vector<std::string> request_fields;
	while(token!=NULL){
		std::string temp(token);
		request_fields.push_back(temp);
		token = strtok(NULL," ");
	}
	if(request_fields.size()!=3){
		printf("Invalid Request Line fields");
		return false;
	}
	
	this->method = request_fields[0];
	this->request_uri = request_fields[1];
	this->version = request_fields[2];
	
	return true;
}

bool HttpRequest::processMessageBody(const std::string request){
	int body_start_inx = request.find("\r\n\r\n");
	if(body_start_inx==std::string::npos){
		printf("Invalid end of request");
		return false;
	}
	this->message_body = request.substr(body_start_inx+4);
	return true;

}

bool HttpRequest::processHeaders(const std::string request){
	int start_idx = request.find("\r\n");
	int end_idx = request.find("\r\n\r\n");
	if(start_idx == end_idx){
		return true;
	}
	else{
		start_idx = start_idx + 2;
		end_idx = end_idx -1;
		int length = end_idx + 1 - start_idx;
		std::string headers_str = request.substr(start_idx,length);
		int cut = 0;
		std::vector<std::string> temp_headers;
		std::string delimiter = "\r\n";

		while ((cut = headers_str.find(delimiter)) != std::string::npos) {
		    temp_headers.push_back(headers_str.substr(0, cut));
		    headers_str.erase(0, cut + delimiter.length());
		}
		temp_headers.push_back(headers_str);

		for(int i = 0; i < temp_headers.size(); i++){
			std::string cur_header_line = temp_headers[i];
			int header_value_cut = cur_header_line.find(":");
			if(header_value_cut==std::string::npos){
				printf("Invalid header");
				return false;
			}
			std::string cur_header = cur_header_line.substr(0,header_value_cut);
			std::string cur_value = cur_header_line.substr(header_value_cut+1);
			this->header_fields[cur_header] = cur_value;
		}
	}

	return true;
}


std::string HttpRequest::getMethod(){
	return this->method;
}
std::string HttpRequest::getResourcePath(){
	return this->request_uri;
}
std::string HttpRequest::getVersion(){
	return this->version;
}

std::string HttpRequest::getMessageBody(){
	return this->message_body;
}

bool HttpRequest::Parse(const std::string request){
	if(!processRequestLine(request)){
		return false;
	}
	if(!processMessageBody(request)){
		return false;
	}
	if(!processHeaders(request)){
		return false;
	}
	return true;
}

// int main(int argc, char* argv[]) {
// 	return 0;
// }
// 
