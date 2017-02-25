#include "http.h"
#include <string>
#include <locale>
#include <boost/asio.hpp>

bool http::status_code::set(int code) {
  if(code > 99 && code < 600) {
    status_code_ = code;
    return true;
  }
  return false;
}

void http::reason_phrase::setDefault(int code) {
  reason_phrase_ = getDefault(code);
}
std::string http::reason_phrase::getDefault(int code) {

  // These are defined http://www.ietf.org/assignments/http-status-codes/http-status-codes.xml
  // This *should* work fine for HTTP/1.0, as they follow the following rules (without particulars):
  // 1xx: Informational - Not used, but reserved for future use
  // 2xx: Success - The action was successfully received, understood, and accepted.
  // 3xx: Redirection - Further action must be taken in order to complete the request
  // 4xx: Client Error - The request contains bad syntax or cannot be fulfilled
  // 5xx: Server Error - The server failed to fulfill an apparently valid request

  switch(code) {
    case 100: 
      return "Continue";
      break;
    case 101:
      return "Switching Protocols";
      break;
    case 102:
      return "Processing";
      break;

    case 200:
      return "OK";
      break;
    case 201:
      return "Created";
      break;
    case 202:
      return "Accepted";
      break;
    case 203:
      return "Non-Authoritative Information";
      break;
    case 204:
      return "No Content";
      break;
    case 205:
      return "Reset Content";
      break;
    case 206:
      return "Partial Content";
      break;
    case 207:
      return "Multi-Status";
      break;
    case 208:
      return "Already Reported";
      break;
    case 226: 
      return "IM Used";

    case 300:
      return "Multiple Choices";
      break;
    case 301:
      return "Moved Permanently";
      break;
    case 302:
      return "Found";
      break;
    case 303:
      return "See Other";
      break;
    case 304:
      return "Not Modified";
      break;
    case 305:
      return "Use Proxy";
      break;
    case 307:
      return "Temporary Redirect";
      break;
    case 308:
      return "Permanent Redirect";
      break;

    case 400:
      return "Bad Request";
      break;
    case 401:
      return "Unauthorized";
      break;
    case 402:
      return "Payment Required";
      break;
    case 403:
      return "Forbidden";
      break;
    case 404:
      return "Not Found";
      break;
    case 405:
      return "Method Not Allowed";
      break;
    case 406:
      return "Not Acceptable";
      break;
    case 407:
      return "Proxy Authentication Required";
      break;
    case 408:
      return "Request Time-out";
      break;
    case 409:
      return "Conflict";
      break;
    case 410:
      return "Gone";
      break;
    case 411:
      return "Length Required";
      break;
    case 412:
      return "Precondition Failed";
      break;
    case 413:
      return "Payload Too Large";
      break;
    case 414:
      return "URI Too Large";
      break;
    case 415:
      return "Unsupported Media Type";
      break;
    case 416:
      return "Range not satisfiable";
      break;
    case 417:
      return "Expectation Failed";
      break;
    case 421:
      return "Misdirected Request";
      break;
    case 422:
      return "Unprocessable Entity";
      break;
    case 423:
      return "Locked";
      break;
    case 424:
      return "Failed Dependency";
      break;
    case 426:
      return "Upgrade Required";
      break;    
    case 428:
      return "Precondition Required";
      break;
    case 429:
      return "Too Many Requests";
      break;
    case 431:
      return "Request Header Fields Too Large";
      break;
    case 451:
      return "Unavailable For Legal Reasons";
      break;

    case 500:
      return "Internal Server Error";
      break;
    case 501:
      return "Not Implemented";
      break;
    case 502:
      return "Bad Gateway";
      break;
    case 503:
      return "Service Unavailable";
      break;
    case 504:
      return "Gateway Timeout";
      break;
    case 505:
      return "HTTP Version not supported";
      break;
    case 506:
      return "Variant Also Negotiates";
      break;
    case 507:
      return "Insufficient Storage";
      break;
    case 508:
      return "Loop Detected";
      break;
    case 510:
      return "Not Extended";
      break;
    case 511:
      return "Network Authentication Required";
      break;
    default:
      return "Unassigned";
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

http::mime_type::ContentType http::mime_type::GetMimeType(std::string extension) {
  // Turn this into a switch statement with hashing macros
  // https://dev.krzaq.cc/post/switch-on-strings-with-c11/

  // this should probably be a map.
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  if(extension.compare("txt") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_PLAIN;
  }
  else if(extension.compare("sh") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_PLAIN;
  }
  else if(extension.compare("cc") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_PLAIN;
  }
  else if(extension.compare("h") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_PLAIN;
  }
  else if(extension.compare("html") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_HTML;
  }
  else if(extension.compare("js") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_JS;
  }
  else if(extension.compare("css") == 0) {
    return http::mime_type::CONTENT_TYPE_TEXT_CSS;
  }
  else if(extension.compare("jpg") == 0) {
    return http::mime_type::CONTENT_TYPE_IMAGE_JPEG;
  }
  else if(extension.compare("jpeg") == 0) {
    return http::mime_type::CONTENT_TYPE_IMAGE_JPEG;
  }
  else if(extension.compare("png") == 0) {
    return http::mime_type::CONTENT_TYPE_IMAGE_PNG;
  }
  else if(extension.compare("gif") == 0) {
    return http::mime_type::CONTENT_TYPE_IMAGE_GIF;
  }
  else if(extension.compare("svg") == 0) {
    return http::mime_type::CONTENT_TYPE_IMAGE_SVG_XML;
  }
  else if(extension.compare("webp") == 0) {
    return http::mime_type::CONTENT_TYPE_IMAGE_WEBP;
  }
  else if(extension.compare("mpeg") == 0) {
    return http::mime_type::CONTENT_TYPE_AUDIO_MPEG;
  }
  else if(extension.compare("mp3") == 0) {
    return http::mime_type::CONTENT_TYPE_AUDIO_MPEG;
  }
  else if(extension.compare("ogg") == 0) {
    return http::mime_type::CONTENT_TYPE_AUDIO_OGG;
  }
  else if(extension.compare("midi") == 0) {
    return http::mime_type::CONTENT_TYPE_AUDIO_MIDI;
  }
  else if(extension.compare("wav") == 0) {
    return http::mime_type::CONTENT_TYPE_AUDIO_WAV;
  }
  else if(extension.compare("mp4") == 0) {
    return http::mime_type::CONTENT_TYPE_VIDEO_MP4;
  }
  else if(extension.compare("webm") == 0) {
    return http::mime_type::CONTENT_TYPE_VIDEO_WEBM;
  }
  else if(extension.compare("ppt") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_VND_mspowerpoint;
  }
  else if(extension.compare("pptx") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_VND_mspowerpoint;
  }
  else if(extension.compare("p12") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_PKCS12;
  }
  else if(extension.compare("pfx") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_PKCS12;
  }
  else if(extension.compare("xhtml") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_XHTML_XML;
  }
  else if(extension.compare("xml") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_XML;
  }
  else if(extension.compare("pdf") == 0) {
    return http::mime_type::CONTENT_TYPE_APP_PDF;
  }
  return http::mime_type::CONTENT_TYPE_APP_OCTET_STREAM;
}
