#include "proxy_handler.h"

#include <iostream>
#include <sstream>
#include <string>

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
		this->m_port_path_ = 80; // default http port
	} else if (port_config_tokens.size() != 2){
		printf("ProxyHandler.Init: Invalid config:\n%s", config.ToString().c_str() );
		return INVALID_CONFIG;
	} else {
		this->m_port_path_ = port_config_tokens[1];
	}

	return OK;
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
	return SendRequestToServer(m_host_path_, m_port_path_, request, response);
}

RequestHandler::Status ProxyHandler::SendRequestToServer(
		const std::string& host, const std::string& port,
		const Request& request, Response* response, int depth) {

	if (depth <= 0) {
		printf("ProxyHandler: Request redirect depth limit reached. Aborting.\n");
		response->SetStatus(Response::NOT_FOUND);
		return OK;
	}

	printf("ProxyHandler: trying to connect to %s (port %s)\n", host.c_str(), port.c_str());
	try {
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(host, port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);
		printf("ProxyHandler: connected\n");

		boost::asio::streambuf reqBuf;
		std::ostream request_stream(&reqBuf);

		if (request.uri().size() < m_uri_prefix_.size()) {
			printf("ProxyHandler: request uri is shorter than uri prefix!\n");
		}
		std::string modified_uri = request.uri().substr(m_uri_prefix_.size());
		if (modified_uri == "") modified_uri = "/";
		if (modified_uri[0] != '/') modified_uri = "/" + modified_uri;
		printf("ProxyHandler: requesting '%s' ('%s')\n", modified_uri.c_str(), request.uri().c_str());

		std::string CRLF = "\r\n";
		request_stream << request.method() + " " + modified_uri + " " + request.version() << CRLF;
		for (auto header : request.headers()) {
			std::string& key = header.first;
			std::string value = header.second;
			if (key == "Connection") {
				// make sure connection closes
				value = "close";
			}
			if (key == "Host") {
				// append :port if we're not using the default port
				value = m_host_path_ + (m_port_path_ == "80" ? "" : ":" + m_port_path_);
			}
			request_stream << key << ": " << value << CRLF;
		}
		request_stream << CRLF;
		request_stream << request.body();

		boost::asio::write(socket, reqBuf);
		printf("ProxyHandler: sent request to server\n");

		const size_t maxResponseLineSize = 1024;
		boost::asio::streambuf response_buf(maxResponseLineSize);
		boost::asio::read_until(socket, response_buf, "\r\n");

		std::istream response_stream(&response_buf);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if ( ! response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			printf("ProxyHandler: received invalid response\n");
			return BAD_REQUEST;
		}
		printf("ProxyHandler: response %d -- %s\n", status_code, status_message.c_str());

		response->SetStatus((Response::ResponseCode)status_code);


		boost::asio::read_until(socket, response_buf, "\r\n\r\n");

		std::string header;
		while (std::getline(response_stream, header) && header != "\r") {
			auto colonPos = header.find(':');
			if (colonPos == std::string::npos) {
				printf("ProxyHandler: malformed header '%s'\n", header.c_str());
				continue;
			}
			std::string header_key = header.substr(0, colonPos);
			std::string header_value = boost::algorithm::trim_copy(header.substr(colonPos+1));

			if (header_key == "Connection") {
				if (header_value != "close") {
					printf("ProxyHandler: connection not set to close!! '%s'\n", header_value.c_str());
				}
			}

			if (status_code == 302) {
				if (header_key == "Location") {
					std::string new_host = boost::algorithm::trim_copy(header_value);
					printf("ProxyHandler: got 302! new Location: '%s'\n", new_host.c_str());

					size_t http_size = 7; // "http://"
					if (new_host.size() > http_size && new_host.substr(0, http_size) == "http://") {
						new_host = new_host.substr(http_size);
					}

					std::string newpath = request.uri();
					std::unique_ptr<Request> newreq(new Request(request));
					// Location will be in form `newhost/new/path`, we want to split into 'newhost' + '/new/path'
					auto pathPos = new_host.find('/');
					if (pathPos != std::string::npos) {
						newpath = new_host.substr(pathPos);
						new_host = new_host.substr(0, pathPos);
						printf("ProxyHandler: newpath = '%s'\n", newpath.c_str());
						if (newpath != request.uri()) {
							printf("ProxyHandler: !! new path not equal to old path !! '%s' vs '%s'\n",
									newpath.c_str(), request.uri().c_str());
						}
					}

					printf("ProxyHandler: redirecting to '%s'\n", new_host.c_str());
					socket.close();

					return SendRequestToServer(new_host, m_port_path_, *newreq, response, depth-1);
				}
			}

			printf("ProxyHandler: header '%s' := '%s'\n", header_key.c_str(), header_value.c_str());
			response->AddHeader(header_key, header_value);
		}

		printf("ProxyHandler: reading until eof\n");
		boost::system::error_code error;
		std::string body;
		while (true) {
			printf("ProxyHandler: reading...\n");
			boost::asio::read(socket, response_buf, boost::asio::transfer_at_least(1), error);
			if (response_buf.size() == 0) {
				break;
			}
			auto bufs = response_buf.data();
			std::string bodyChunk(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + response_buf.size());
			body += bodyChunk;
			printf("ProxyHandler: read chunk length %zu\n", bodyChunk.size());
			response_buf.consume(response_buf.size());
		}
		printf("ProxyHandler: read body (length %zu)\n", body.size());

		response->SetBody(body);

		if (error && error != boost::asio::error::eof) {
			printf("ProxyHandler: error! %s\n", error.message().c_str());
		}

	} catch (std::exception& e) {
		// could not reach server
		printf("ProxyHandler: %s\n", e.what());
		return BAD_REQUEST;
	}

	printf("ProxyHandler: done\n");

	return OK;
}
