// Represents an HTTP response.
//
// Usage:
//   Response r;
//   r.SetStatus(RESPONSE_200);
//   r.SetBody(...);
//   return r.ToString();
//
// Constructed by the RequestHandler, after which the server should call ToString
// to serialize.
#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <string>
#include "http.h"
class Response {
 public:
  enum ResponseCode {
    // Define your HTTP response codes here.
    OK = 200,
    NOT_FOUND = 404,
    BAD_REQUEST = 400
  };
  
  void SetStatus(const ResponseCode response_code);
  void AddHeader(const std::string& header_name, const std::string& header_value);
  void SetBody(const std::string& body);
  
  //for the moment it only has HTTP/1.1
  std::string ToString();

private:
	std::string reason_phrase_;
    ResponseCode status_code_;
    
    http::http_headers http_headers_;
    std::string http_version_ = "HTTP/1.1";

      
    std::string body_;
      
    const char* line_break = "\r\n";

};

#endif