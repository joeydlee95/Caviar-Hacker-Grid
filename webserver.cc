#include "nginx-configparser/config_parser.h"
#include "server/server.h"
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: ./webserver <path to config file>\n");
    return 1;
  }

  NginxConfigParser config_parser;
  NginxConfig config;
  
  if (!config_parser.Parse(argv[1], &config)) {
    printf("Invalid config file");
    return 1;
  }

  std::string port_str = "";
  if (!config.find(port_str)) {
    printf("Config does not specify a port");
    return 1;
  }
  int port = atoi(port_str.c_str());
  printf("port: %d", port);
  //
  printf("hello");
  return 0;
}

