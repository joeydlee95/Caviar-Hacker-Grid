#include "http.h"
#include <string>
#include <boost/asio.hpp>

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
    case 451:
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
    case 511:
      reason_phrase_ = "Network Authentication Required";
      break;
    default:
      reason_phrase_ = "Unassigned";
      break;

  }
}

std::string http::mime_type::ContentTypeAsString(ContentType type) {
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
  // http://www.iana.org/assignments/media-types/media-types.xhtml#imag
  switch(type) {
    case CONTENT_TYPE_TEXT_PLAIN:
      return "text/plain";
    case CONTENT_TYPE_TEXT_HTML:
      return "text/html";
    case CONTENT_TYPE_TEXT_CSS:
      return "text/css";
    case CONTENT_TYPE_TEXT_JS:
      return "text/javascript";
    case CONTENT_TYPE_TEXT_WC:
      return "text/*";

    case CONTENT_TYPE_IMAGE_JPEG:
      return "image/jpeg";
    case CONTENT_TYPE_IMAGE_PNG:
      return "image/png";
    case CONTENT_TYPE_IMAGE_GIF:
      return "image/gif";
    case CONTENT_TYPE_IMAGE_SVG_XML:
      return "image/svg+xml";
    case CONTENT_TYPE_IMAGE_BMP:
      return "image/bmp";
    case CONTENT_TYPE_IMAGE_WEBP:
      return "image/webp";
    case CONTENT_TYPE_IMAGE_WC:
      return "image/*";

    case CONTENT_TYPE_AUDIO_MPEG:
      return "audio/mpeg";
    case CONTENT_TYPE_AUDIO_OGG:
      return "audio/ogg";
    case CONTENT_TYPE_AUDIO_WEBM:
      return "audio/webm";
    case CONTENT_TYPE_AUDIO_MIDI:
      return "audio/midi";
    case CONTENT_TYPE_AUDIO_WAV:
      return "audio/wav";
    case CONTENT_TYPE_AUDIO_WC:
      return "audio/*";

    case CONTENT_TYPE_VIDEO_MP4:
      return "video/mp4";
    case CONTENT_TYPE_VIDEO_WEBM:
      return "video/webm";
    case CONTENT_TYPE_VIDEO_OGG:
      return "video/ogg";
    case CONTENT_TYPE_VIDEO_WC:
      return "video/*";

    case CONTENT_TYPE_APP_PKCS12:
      return "application/pkcs12";
    case CONTENT_TYPE_APP_VND_mspowerpoint:
      return "application/vnd.mspowerpoint";
    case CONTENT_TYPE_APP_XHTML_XML:
      return "application/xhtml+xml";
    case CONTENT_TYPE_APP_XML:
      return "application/xml";
    case CONTENT_TYPE_APP_PDF:
      return "application/pdf";

    // Default for unspecified data types is binary octet stream
    case CONTENT_TYPE_APP_OCTET_STREAM:
    default:
      return "application/octet-stream";
  }
}

http::HTTPResponse::HTTPResponse() {

}

std::string http::HTTPResponse::ToString() {
  std::string serialized_resp;
  serialized_resp.append(http_version_ + " " + std::to_string(status_code_.status_code_) + " " + reason_phrase_.reason_phrase_);
  
  
  for(const auto & header : http_headers_.fields_) {
    serialized_resp.append(line_break);
    serialized_resp.append(header.first);
    serialized_resp.append(": ");
    serialized_resp.append(header.second);
  }

  serialized_resp.append(line_break);
  serialized_resp.append(line_break);
  serialized_resp.append(body_);
  return serialized_resp;
}

const http::status_code& http::HTTPResponseBuilder::status_code() const {
  return response_->status_code_;
}

bool http::HTTPResponseBuilder::set_status_code(int code) {
  if(response_->status_code_.set(code)) {
    if(response_->reason_phrase_.reason_phrase_.compare("") == 0) {//No reason phrase set yet.
    response_->reason_phrase_.setDefault(code);
    }
    return true;
  }
  return false;
}

const http::reason_phrase& http::HTTPResponseBuilder::reason_phrase() const {
  return response_->reason_phrase_;
}

void http::HTTPResponseBuilder::set_reason_phrase(std::string phrase) {
  response_->reason_phrase_.reason_phrase_ = phrase;
}

http::http_headers& http::HTTPResponseBuilder::headers() {
  return response_->http_headers_;
}

const http::http_headers& http::HTTPResponseBuilder::headers() const {
  return response_->http_headers_;
}

/*
// R-value. See: http://stackoverflow.com/questions/4549151/c-double-address-operator
void http::HTTPResponseBuilder::set_body(std::string &&body_text) {
  response_->body_ << std::move(std::istringstream is(body_text));
  set_content_type(CONTEXT_TYPE_TEXT_PLAIN);
}

void http::HTTPResponseBuilder::set_body(const std::string &body_text) {
  response_->body_ = std::move(std::istringstream is(body_text));
  set_content_type(CONTEXT_TYPE_TEXT_PLAIN);
}
void http::HTTPResponseBuilder::set_body(std::vector<unsigned char> &&body_data) {
  response_->body_ = std::move(std::istringstream is(body_text));
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
}*/



void http::HTTPResponseBuilder::set_length(std::size_t length) {
  set_header("Content-Length", std::to_string(length));
}

void http::HTTPResponseBuilder::set_content_type(http::mime_type::ContentType type) {
  set_header("Content-Type", http::mime_type::ContentTypeAsString(type));
}

void http::HTTPResponseBuilder::set_header(std::string field_name, std::string field_value) {
  response_->http_headers_.fields_[field_name] = field_value;
}

void http::HTTPResponseBuilder::set_header(http::http_field fields) {
  set_header(fields.field_name, fields.field_value);
}

void http::HTTPResponseBuilder::set_headers(std::vector<http_field> fields) {
  for(const auto& field : fields) {
    set_header(field);
  }
}

void http::HTTPResponseBuilder::set_body(std::string body) {
  response_->body_ = body;
}