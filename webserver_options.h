#ifndef WEBSERVER_OPTIONS_H
#define WEBSERVER_OPTIONS_H
#include <cstdlib>
#include <string>
#include <map> 
#include "nginx-configparser/config_parser.h"
class WebserverOptions {
public:
	WebserverOptions(std::unique_ptr<NginxConfig> const &statement, std::vector<std::map<std::string, std::vector<std::string> > >* options);
	std::vector<std::map<std::string, std::vector<std::string> > >* options_;
	std::string ToString() const;
};

#endif