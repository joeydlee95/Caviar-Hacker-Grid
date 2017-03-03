#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include <string>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "../nginx-configparser/config_parser.h"
#include "../http/httpRequest.h"
#include "../http/httpMutableRequest.h"
#include "../http/httpResponse.h"
#include "request_handler.h"

class ProxyHandler : public RequestHandler {
  public:
    Status Init(const std::string& uri_prefix, const NginxConfig& config);
    Status HandleRequest(const Request& request, Response* response);

    std::string getPrefix() {
      return this->m_uri_prefix_;
    }
    std::string getProxyHost() {
      return this->m_host_path_;
    }
    std::string getProxyPort() {
      return this->m_port_path_;
    }

    std::string ExtractNonProxyUri(const std::string& prefix, const std::string& uri);
    Status SendRequestToServer(const std::string& host, const std::string& port, const Request& req, Response* resp, int depth=10);
    void ParseRedirectLocation(std::string location, std::string* new_path, std::string* new_host);
    void ModifyRequestForProxy(const Request& request, MutableRequest* modified_request);
  protected:
    virtual void ConnectSocketToEndpoint(boost::asio::ip::tcp::socket* socket, std::string host, std::string port);
    virtual boost::system::error_code SocketReadUntil(boost::asio::ip::tcp::socket* socket, boost::asio::streambuf* buf, const std::string& sep);
    virtual boost::system::error_code SocketReadToEOF(boost::asio::ip::tcp::socket* socket, boost::asio::streambuf* buf, std::string* data);
    virtual void WriteToSocket(boost::asio::ip::tcp::socket* socket, const std::string& requestString);
    virtual bool ReadNextHeader(std::istream* response_stream, std::string* header_key, std::string* header_value);

    std::string m_uri_prefix_;
    std::string m_host_path_;
    std::string m_port_path_;
    boost::asio::io_service io_service_;
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

#endif
