#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>
#include <utility>
#include <memory>
#include <vector>

// For the Request and Response classes, you need to implement the methods
// and add private data as appropriate. You may also need to modify or extend
// the API when implementing the reverse proxy. Use your good judgment.

// Represents an HTTP Request.
//
// Usage:
//   auto request = Request::Parse(raw_request);
class Request {
 public:
   //return nullptr if nnot successful
   static std::unique_ptr<Request> Parse(const std::string& raw_request);

   std::string raw_request() const{
    return this->raw_request_;
   }
  std::string method() const{
      return this->method_;
    }
    std::string uri() const{
      return this->request_uri_;
    }
    std::string version() const{
      return this->version_;
    }

  using Headers = typename std::vector<std::pair<std::string, std::string>>;
  std::string body() const{
      return this->message_body_;
    }
  
  Headers headers() const{
    return this->headers_;
  }

private:
  bool ParseRequest(const std::string& request);
  //should only be called by ParseRequest
  bool processRequestLine(const std::string& request);
  bool processMessageBody(const std::string& request);
  bool processHeaders(const std::string& request);

  //for example, we have "Accept-Languages: en-us" in the headers
  //just do header_fields[Accept-Languages]
  //notice that it will return "en-us"
  std::map<std::string,std::string> header_fields_;

  Headers headers_;

  std::string raw_request_;
  std::string request_line_;
  std::string message_body_;
  std::string method_;
  std::string version_;
  std::string request_uri_;  
};





#endif
