#include "proxy_handler.h"

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../http/http.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/trim.hpp>
using boost::asio::ip::tcp;

RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
  this->m_uri_prefix_ = uri_prefix;
  // Assume the config argument has something like:
  // host www.example.com
  // port #####. (optional)
  std::vector<std::string> host_config_tokens = config.find("host");
  if (host_config_tokens.size() != 2){
    printf("ProxyHandler.Init: Invalid config:\n%s", config.ToString().c_str() );
    return INVALID_CONFIG;
  }
  this->m_host_path_ = host_config_tokens[1];

  std::vector<std::string> port_config_tokens = config.find("port");
  if (port_config_tokens.size() == 0) {
    this->m_port_path_ = "80"; // default http port
  }
  else if (port_config_tokens.size() != 2){
    printf("ProxyHandler.Init: Invalid config:\n%s", config.ToString().c_str() );
    return INVALID_CONFIG;
  }
  else {
    if (std::stoi(port_config_tokens[1]) > 65535) {
      return INVALID_CONFIG;
    }
    this->m_port_path_ = port_config_tokens[1];
  }

  return OK;
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
  return SendRequestToServer(m_host_path_, m_port_path_, request, response);
}

// convert a local uri like "/proxy/some/path" to remote uri "/some/path"
std::string ProxyHandler::ExtractNonProxyUri(const std::string& prefix, const std::string& uri) {
  if (uri.size() < prefix.size()) {
    printf("ProxyHandler: warning: request uri is shorter than uri prefix!\n");
    return uri;
  }

  std::string modified_uri = uri.substr(prefix.size());
  if (modified_uri == "") {
    modified_uri = "/";
  }
  if (modified_uri[0] != '/') {
    modified_uri = "/" + modified_uri;
  }
  return modified_uri;
}

void ProxyHandler::ModifyRequestForProxy(const Request& request, MutableRequest* modified_request) {
  std::string modified_uri = ExtractNonProxyUri(m_uri_prefix_, request.uri());
  printf("ProxyHandler: requesting '%s' ('%s')\n", modified_uri.c_str(), request.uri().c_str());

  modified_request->SetURI(modified_uri);
  modified_request->SetHeader("Connection", "close");
  modified_request->SetHeader("Host", m_host_path_ + ":" + m_port_path_);
}

// given a location string like "http://www.ucla.edu/some/path", extract "www.ucla.edu" and "/some/path"
void ProxyHandler::ParseRedirectLocation(std::string location, std::string* new_path, std::string* new_host) {
  location = boost::algorithm::trim_copy(location);

  // strip "http://", if present
  const size_t http_size = 7; // "http://"
  if (location.size() > http_size && location.substr(0, http_size) == "http://") {
    location = location.substr(http_size);
  }

  const size_t https_size = 8; // "https://"
  if (location.size() > https_size && location.substr(0, https_size) == "https://") {
    printf("ProxyHandler: warning: redirect specified https, which is unsupported.\n");
    location = location.substr(https_size);
  }

  // Location will be in form `newhost/new/path`, we want to split into 'newhost' + '/new/path'
  auto pathPos = location.find('/');
  if (pathPos != std::string::npos) {
    *new_host = location.substr(0, pathPos);
    *new_path = location.substr(pathPos);
  }
  else {
    *new_host = location;
    *new_path = "/"; // no path given
  }
}

void ProxyHandler::ConnectSocketToEndpoint(tcp::socket* socket, std::string host, std::string port) {
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(host, port);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  boost::asio::connect(*socket, endpoint_iterator);
}

void ProxyHandler::WriteToSocket(tcp::socket* socket, const std::string& requestStr) {
  boost::asio::streambuf reqBuf;
  std::ostream request_stream(&reqBuf);
  request_stream << requestStr;
  boost::asio::write(*socket, reqBuf);
}

boost::system::error_code ProxyHandler::SocketReadToEOF(tcp::socket* socket, boost::asio::streambuf* buf, std::string* data) {
  boost::system::error_code error;
  while (true) {
    boost::asio::read(*socket, *buf, boost::asio::transfer_at_least(1), error);
    if (buf->size() == 0) {
      break;
    }
    auto bufs = buf->data();
    std::string bodyChunk(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + buf->size());
    *data += bodyChunk;
    buf->consume(buf->size());
  }
  return error;
}

boost::system::error_code ProxyHandler::SocketReadUntil(
    tcp::socket* socket, boost::asio::streambuf* buf, const std::string& sep) {
  boost::system::error_code error;
  boost::asio::read_until(*socket, *buf, sep, error);
  if (error && error != boost::asio::error::eof) {
    printf("ProxyHandler: error! %s\n", error.message().c_str());
  }
  return error;
}

bool ProxyHandler::ReadNextHeader(std::istream* response_stream, std::string* header_key, std::string* header_value) {
  std::string header;
  if ( ! std::getline(*response_stream, header)) {
    return false;
  }
  if (header == "\r") {
    return false; // blank line = end of headers
  }
  auto colonPos = header.find(':');
  if (colonPos == std::string::npos) {
    printf("ProxyHandler: warning: malformed header '%s'\n", header.c_str());
    *header_key = "";
    *header_value = "";
    return true; // possibly a transient error, try to get further headers.
  }
  *header_key = header.substr(0, colonPos);
  *header_value = boost::algorithm::trim_copy(header.substr(colonPos+1));
  return true;
}

RequestHandler::Status ProxyHandler::SendRequestToServer(
    const std::string& host, const std::string& port,
    const Request& request, Response* response, int depth) {

  if (depth <= 0) {
    printf("ProxyHandler: Request redirect depth limit reached. Aborting.\n");
    response->SetStatus(Response::NOT_FOUND);
    return OK;
  }

  printf("ProxyHandler: connecting to %s (port %s)\n", host.c_str(), port.c_str());
  try {
    tcp::socket socket(io_service_);

    ConnectSocketToEndpoint(&socket, host, port);

    boost::asio::streambuf reqBuf;
    std::ostream request_stream(&reqBuf);

    MutableRequest modified_request(request);
    ModifyRequestForProxy(request, &modified_request);

    WriteToSocket(&socket, modified_request.ToString());

    const size_t bufSize = 8192; // boost will only read this much, buffer won't be overrun.
    boost::asio::streambuf response_buf(bufSize);
    std::istream response_stream(&response_buf);

    SocketReadUntil(&socket, &response_buf, "\r\n");

    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message, '\r');
    response_stream.ignore(1); // \n

    printf("ProxyHandler: read status: %u %s\n", status_code, status_message.c_str());
    response->SetStatus((Response::ResponseCode)status_code);

    SocketReadUntil(&socket, &response_buf, "\r\n\r\n");

    std::string header_key;
    std::string header_value;
    while (ReadNextHeader(&response_stream, &header_key, &header_value)) {
      if (header_key == "Connection") {
        if (header_value != "close") {
          printf("ProxyHandler: warning: connection not set to close: '%s'\n", header_value.c_str());
        }
      }

      if (status_code == 302) {
        if (header_key == "Location") {
          printf("ProxyHandler: got 302, new Location: '%s'\n", header_value.c_str());

          std::string new_path;
          std::string new_host;
          ParseRedirectLocation(header_value, &new_path, &new_host);

          std::unique_ptr<MutableRequest> newreq(new MutableRequest(request));
          newreq->SetURI(new_path);
          newreq->SetHeader("Host", new_host);

          printf("ProxyHandler: redirecting to '%s'\n", new_host.c_str());
          socket.close();

          return SendRequestToServer(new_host, m_port_path_, (Request)*newreq, response, depth-1);
        }
      }

      response->AddHeader(header_key, header_value);
    }

    std::string body;
    auto error = SocketReadToEOF(&socket, &response_buf, &body);

    if (error && error != boost::asio::error::eof) {
      printf("ProxyHandler: error! %s\n", error.message().c_str());
    }
    else {
      response->SetBody(body);
    }

  } catch (std::exception& e) {
    // could not reach server
    printf("ProxyHandler: exception: %s\n", e.what());
    return BAD_REQUEST;
  }

  return OK;
}
