#ifndef WEBSERVER_H

#define WEBSERVER_H

#include "nginx-configparser/config_parser.h"
#include "server/server.h"
#include "webserver_options.h"
#include <cstdlib>
#include <string>
#include <map>



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

};

#endif