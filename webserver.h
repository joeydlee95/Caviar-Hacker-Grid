#ifndef WEBSERVER_H

#define WEBSERVER_H

#include "nginx-configparser/config_parser.h"
#include "server/server.h"
#include <cstdlib>
#include <string>
#include <map>
#include "server/request_handler.h"



class Webserver {
public:
	Webserver(NginxConfig* config) : config_(config) {
	}

	virtual bool Init();
	virtual bool run_server();
	virtual boost::system::error_code port_valid();

	std::string ToString() const;

	NginxConfig* config_;
	int port_ = 0;
private:
	// std::map<std::string, RequestHandler*> RequestHandlers_;
	std::map<std::string,RequestHandler*> RequestHandlers_;

	RequestHandler* DefaultHandler_;
	bool AddHandler(std::string path, std::string HandlerName);

};

#endif