#ifndef WEBSERVER_H

#define WEBSERVER_H

#include "nginx-configparser/config_parser.h"
#include "server/server.h"
#include <cstdlib>
#include <string>



class Webserver{
public:
	Webserver(NginxConfigParser* parser, NginxConfig* config):parser_(parser),config_(config){}
	
	bool run_server(const char* file_name);
private:

	NginxConfigParser* parser_;
	NginxConfig* config_;
};

#endif