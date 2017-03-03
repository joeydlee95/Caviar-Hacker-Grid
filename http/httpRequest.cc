#include "httpRequest.h"
#include <string.h>
#include <vector>
#include <map>
#include <utility>
#include <cstdio>
#include <memory>

std::unique_ptr<Request> Request::Parse(const std::string& raw_request){
  Request* req = new Request();

  if(req->ParseRequest(raw_request)){
    std::unique_ptr<Request> ret;
    ret.reset(req);
    return ret;
  }
  else{
    return std::unique_ptr<Request>(nullptr);
  }
}


bool Request::processRequestLine(const std::string& request){
  //get the first CRLF which comes right after the request line
  std::size_t end_request_ln_idx = request.find("\r\n");
  if(end_request_ln_idx==std::string::npos){
    printf("Invalid Request Line CRLF");
    return false;
  }
  std::string request_line = request.substr(0,end_request_ln_idx);
  this->request_line_ = request_line;

  //split the request line into tokens and push them onto a temp vector
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
  
  this->method_ = request_fields[0];
  this->request_uri_ = request_fields[1];
  this->version_ = request_fields[2];
  
  return true;
}

bool Request::processMessageBody(const std::string& request){
  std::size_t body_start_inx = request.find("\r\n\r\n");
  if(body_start_inx==std::string::npos){
    printf("Invalid end of request");
    return false;
  }
  this->message_body_ = request.substr(body_start_inx+4);
  return true;

}

bool Request::processHeaders(const std::string& request){
  //the CRLF at the end of the request line
  std::size_t start_idx = request.find("\r\n");
  //the CRLFs preceding the start of the optional message body 
  std::size_t end_idx = request.find("\r\n\r\n");
  if(start_idx == end_idx){
    //if there is no header at all
    return true;
  }
  else{
    start_idx = start_idx + 2;
    end_idx = end_idx -1;
    int length = end_idx + 1 - start_idx;
    std::string headers_str = request.substr(start_idx,length);
    std::size_t cut = 0;
    std::vector<std::string> temp_headers;
    std::string delimiter = "\r\n";

    //put each line in the header_string into a temp vector
    while ((cut = headers_str.find(delimiter)) != std::string::npos) {
        temp_headers.push_back(headers_str.substr(0, cut));
        headers_str.erase(0, cut + delimiter.length());
    }
    temp_headers.push_back(headers_str);

    //for each line of header: value, put it mapping into the map
    for(std::size_t i = 0; i < temp_headers.size(); i++){
      std::string cur_header_line = temp_headers[i];
      std::size_t header_value_cut = cur_header_line.find(":");
      if(header_value_cut==std::string::npos){
        printf("Invalid header");
        return false;
      }
      std::string cur_header = cur_header_line.substr(0,header_value_cut);
      //header_value_cut+2 because there is a white space right after the ':''
      std::string cur_value = cur_header_line.substr(header_value_cut+2);
      this->header_fields_[cur_header] = cur_value;
    }
  }

  return true;
}




bool Request::ParseRequest(const std::string& request){
  if(!processRequestLine(request)){
    return false;
  }
  if(!processMessageBody(request)){
    return false;
  }
  if(!processHeaders(request)){
    return false;
  }
  this->raw_request_ = request;

  for(const auto& a_mapping: this->header_fields_){
    auto temp_pair = std::make_pair(a_mapping.first,a_mapping.second);
    headers_.push_back(temp_pair);
  }

  return true;
}


