#include "webserver.h"
#include "nginx-configparser/config_parser.h"
int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: ./webserver <path to config file>\n");
    return 1;
  }

  NginxConfigParser parser;
  NginxConfig config;

  Webserver server(&parser,&config);

  if(!server.run_server(argv[1])){
  	return 1;
  }
  
  return 0;
}