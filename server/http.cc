#include "http.h"
#include <stringstream>

bool http::status_code::set(int code) {
  if(code > 99 && code < 600) {
    status_code_ = code;
    return true;
  }
  return false;
}

void http::reason_phrase::setDefault(int code) {
  // These are defined http://www.ietf.org/assignments/http-status-codes/http-status-codes.xml
  // This *should* work fine for HTTP/1.0, as they follow the following rules (without particulars):
  // 1xx: Informational - Not used, but reserved for future use
  // 2xx: Success - The action was successfully received, understood, and accepted.
  // 3xx: Redirection - Further action must be taken in order to complete the request
  // 4xx: Client Error - The request contains bad syntax or cannot be fulfilled
  // 5xx: Server Error - The server failed to fulfill an apparently valid request

  switch(code) {
    case 100: 
      reason_phrase_ = "Continue";
      break;
    case 101:
      reason_phrase_ = "Switching Protocols";
      break;
    case 102:
      reason_phrase_ = "Processing";
      break;

    case 200:
      reason_phrase_ = "OK";
      break;
    case 201:
      reason_phrase_ = "Created";
      break;
    case 202:
      reason_phrase_ = "Accepted";
      break;
    case 203:
      reason_phrase_ = "Non-Authoritative Information";
      break;
    case 204:
      reason_phrase_ = "No Content";
      break;
    case 205:
      reason_phrase_ = "Reset Content";
      break;
    case 206:
      reason_phrase_ = "Partial Content";
      break;
    case 207:
      reason_phrase_ = "Multi-Status";
      break;
    case 208:
      reason_phrase_ = "Already Reported";
      break;
    case 226: 
      reason_phrase_ = "IM Used";

    case 300:
      reason_phrase_ = "Multiple Choices";
      break;
    case 301:
      reason_phrase_ = "Moved Permanently";
      break;
    case 302:
      reason_phrase_ = "Found";
      break;
    case 303:
      reason_phrase_ = "See Other";
      break;
    case 304:
      reason_phrase_ = "Not Modified";
      break;
    case 305:
      reason_phrase_ = "Use Proxy";
      break;
    case 307:
      reason_phrase_ = "Temporary Redirect";
      break;
    case 308:
      reason_phrase_ = "Permanent Redirect";
      break;

    case 400:
      reason_phrase_ = "Bad Request";
      break;
    case 401:
      reason_phrase_ = "Unauthorized";
      break;
    case 402:
      reason_phrase_ = "Payment Required";
      break;
    case 403:
      reason_phrase_ = "Forbidden";
      break;
    case 404:
      reason_phrase_ = "Not Found";
      break;
    case 405:
      reason_phrase_ = "Method Not Allowed";
      break;
    case 406:
      reason_phrase_ = "Not Acceptable";
      break;
    case 407:
      reason_phrase_ = "Proxy Authentication Required";
      break;
    case 408:
      reason_phrase_ = "Request Time-out";
      break;
    case 409:
      reason_phrase_ = "Conflict";
      break;
    case 410:
      reason_phrase_ = "Gone";
      break;
    case 411:
      reason_phrase_ = "Length Required";
      break;
    case 412:
      reason_phrase_ = "Precondition Failed";
      break;
    case 413:
      reason_phrase_ = "Payload Too Large";
      break;
    case 414:
      reason_phrase_ = "URI Too Large";
      break;
    case 415:
      reason_phrase_ = "Unsupported Media Type";
      break;
    case 416:
      reason_phrase_ = "Range not satisfiable";
      break;
    case 417:
      reason_phrase_ = "Expectation Failed";
      break;
    case 421:
      reason_phrase_ = "Misdirected Request";
      break;
    case 422:
      reason_phrase_ = "Unprocessable Entity";
      break;
    case 423:
      reason_phrase_ = "Locked";
      break;
    case 424:
      reason_phrase_ = "Failed Dependency";
      break;
    case 426:
      reason_phrase_ = "Upgrade Required";
      break;    
    case 428:
      reason_phrase_ = "Precondition Required";
      break;
    case 429:
      reason_phrase_ = "Too Many Requests";
      break;
    case 431:
      reason_phrase_ = "Request Header Fields Too Large";
      break;
    case 431:
      reason_phrase_ = "Unavailable For Legal Reasons";
      break;

    case 500:
      reason_phrase_ = "Internal Server Error";
      break;
    case 501:
      reason_phrase_ = "Not Implemented";
      break;
    case 502:
      reason_phrase_ = "Bad Gateway";
      break;
    case 503:
      reason_phrase_ = "Service Unavailable";
      break;
    case 504:
      reason_phrase_ = "Gateway Timeout";
      break;
    case 505:
      reason_phrase_ = "HTTP Version not supported";
      break;
    case 506:
      reason_phrase_ = "Variant Also Negotiates";
      break;
    case 507:
      reason_phrase_ = "Insufficient Storage";
      break;
    case 508:
      reason_phrase_ = "Loop Detected";
      break;
    case 510:
      reason_phrase_ = "Not Extended";
      break;
    case 508:
      reason_phrase_ = "Network Authentication Required";
      break;
    default:
      reason_phrase_ = "Unassigned";
      break;

  }
}

std::ostream* http::HTTPResponse::to_stream() {
  std::ostream os;
  // TODO: Make these overload the '<<' operator instead.
  os << status_code_.status_code_;
  os << reason_phrase_.reason_phrase_;
  os << http_version_;
  os << "\r\n";
  os << http_headers;
  os << "\r\n\r\n";
  os << body_.rdbuf();
  return os;
}

const status_code& http::HTTPResponseBuilder::status_code() const {
  return response_.status_code_;
}

bool http::HTTPResponseBuilder::set_status_code(int code) const {
  return response_.status_code_.set(code);
}

const reason_phrase& http::HTTPResponseBuilder::reason_phrase() const {
  return response_.reason_phrase_;
}

void http::HTTPResponseBuilder::set_reason_phrase(std::string phrase) const {
  reason_phrase_.reason_phrase_ = phrase;
}

http_headers& http::HTTPResponseBuilder::headers() {
  return response_.http_headers_;
}

const http_headers& http::HTTPResponseBuilder::headers() const {
  return response_.http_headers_;
}
// R-value. See: http://stackoverflow.com/questions/4549151/c-double-address-operator
void http::HTTPResponseBuilder::set_body(std::string &&body_text) {
  response_.body_ = std::move(std::istringstream is(body_text));
  set_content_type(CONTEXT_TYPE_TEXT_PLAIN);
}

void http::HTTPResponseBuilder::set_body(const std::string &body_text) {
  response_.body_ = std::move(std::istringstream is(body_text));
  set_content_type(CONTEXT_TYPE_TEXT_PLAIN);
}
void http::HTTPResponseBuilder::set_body(std::vector<unsigned char> &&body_data) {
  response_.body_ = std::move(std::istringstream is(body_text));
  set_content_type(CONTEXT_TYPE_TEXT_PLAIN);
}
void http::HTTPResponseBuilder::set_body(const std::vector<unsigned char> &body_data) {
  std::stringstream ss;
  copy(body_data.begin(), body_data().end(), istream_iterator<unsigned char>(ss, ""));
  response_.body_ = std::move(ss);
  set_content_type(CONTEXT_TYPE_TEXT_PLAIN);
}

void http::HTTPResponseBuilder::set_body(std::istream &stream) {
  response_.body_ = std::move(stream);
}
void http::HTTPResponseBuilder::set_body(std::istream &stream, std::size_t content_length) {
  response_.body_ = std::move(stream);
  set_length(content_length);
}