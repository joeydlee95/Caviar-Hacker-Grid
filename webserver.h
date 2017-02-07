#ifndef WEBSERVER_H

#define WEBSERVER_H

#include "nginx-configparser/config_parser.h"
#include "server/server.h"
#include <cstdlib>
#include <string>
#include <map>


class WebserverOptions {
public:
	WebserverOptions(std::unique_ptr<NginxConfig> const &statement);
	std::vector<std::map<std::string, std::vector<std::string> > > options_;
	std::string ToString();
};

class Webserver {
public:
	Webserver(NginxConfigParser* parser, NginxConfig* config) : parser_(parser), config_(config) {
	}
	virtual bool configure_server(const char* file_name);
	virtual bool run_server(const char* file_name);
	virtual boost::system::error_code port_valid();
	NginxConfigParser* parser_;
	NginxConfig* config_;
	
	std::map<std::string, WebserverOptions> options_;

	int port_;

};

#endif