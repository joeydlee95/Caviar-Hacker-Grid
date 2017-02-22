#ifndef WEBSERVER_OPTIONS_H
#define WEBSERVER_OPTIONS_H
#include <cstdlib>
#include <string>
#include <map> 
#include "nginx-configparser/config_parser.h"

// options is a bit of a misnomer. option would be more correct. 
class WebserverOptions {
public:
	WebserverOptions(std::shared_ptr<NginxConfig> const &statement, std::map<std::string, std::vector<std::string> >* options);
	std::map<std::string, std::vector<std::string> >* options_;
	std::string ToString() const;
    // std::string 
};

#endif