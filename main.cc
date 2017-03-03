#include "server/webserver.h"
#include "nginx-configparser/config_parser.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: ./webserver <path to config file>\n");
    return 1;
  }

  NginxConfig config;

  if(!ParseFile(argv[1], &config)) {
    printf("Syntax Error in NginxConfig %s", argv[1]);
    return 1;
  }

  WebServer server(&config);

  if(!server.Init()) {
    printf("Error initializing server");
    return 1;
  }
  if(!server.run_server()) {
    printf("Error running server.");
    return 1;
  }
  
  return 0;
}


