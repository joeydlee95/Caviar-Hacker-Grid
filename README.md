[![Build Status](https://travis-ci.org/UCLA-CS130/Caviar-Hacker-Grid.svg?branch=master)](https://travis-ci.org/UCLA-CS130/Caviar-Hacker-Grid)

# Caviar-Hacker-Grid
CS130 Winter 2017 Webserver Project

Commands are to be run from the base Caviar-Hacker-Grid directory.
# Installation
You must clone the repository recursively.
``` 
git clone --recursive https://github.com/UCLA-CS130/Caviar-Hacker-Grid.git 
```

To run the server, cd into the directory and run 
```
make && ./webserver test_config
```

# Commands
To run tests:
  ```
  make test
  ```
  
To run integration test:
  ```
  make integration
  ```

To clean all binaries:
  ```
  make clean
  ```
 
  
# Config File Format
Conforms to the standard API:
```
port <port number>;

path /path HandlerName {
  HandlerOptions;
}

default defaultHandler {}
```

port and default are mandatory tags. There can be as many path handlers as you want.

port must have a valid unused port that can be bound, otherwise the server will not begin. 

Path handlers are longest prefixed matched, so you can have subpaths. Be careful to ensure that the paths have a slash prepended: otherwise, they won't ever be matched.

The HandlerName is the name of the handler you want to use. If the handler is not found, the block is ignored.


Currently, the supported handlers are:
* NotFoundHandler
  * No options
* EchoHandler
  * No options
* StaticHandler
  * Mandatory: 
      * root <path_to_files_to_serve>
        * Note that the path is relative to the location served from.
        * This means you can take advantage of relative directories. 


# Layout
## Existing Code
* `main.cc` : Gets and parses command lines arguments, passed to webserver class
* `webserver.cc` : Parses and initializes the config for the server, including validation. This initializes the request handlers.
* `nginx-configparser/config_parser.cc` : Parses configs, and provides an API for extracting blocks. find and findall functions return a NginxConfig and a vector of NginxConfig's respectively, which can be recurisvely traversed. 
* `filesystem/file_opener.cc` : Handles reading files into and out of strings. 
* `server/server.cc` : Creates the connection, and handles the polymorphic handler dispatch.
* `server/http.cc` : Helper classes for http requests and responses, providing an abstraction over mimetypes, reason phrases, response codes, and headers. 
* `server/httpRequest.cc` : Common API http requests. Allows parsing of HTTP Requests , and provides an API for easily extracting fields from it.
* `server/httpResponse.cc` : Common API http responses. Allows a simple way to set the status, add headers, and set the body of the http response. Provides a simple method to create a string representing the HTTP response, by setting headers, status, and body via an API.
* `server/request_handler.h` : The request handler API, base abstract class for handlers
* `server/echo_handler.cc` : A request handler dispatching handlers with config EchoHandler. Returns the original HTTP request in plaintext.
* `server/file_hander.cc` : A request handler dispatching files from the server's local file system. In the config, requires a root /path/to/files which is the path from which it will server the files. If the file isn't found it returns 404.
* `server/not_found_handler.cc` : A request handler that dispatches 404 not found.

## Adding Code
1. Modify the Makefile to include the path to your new handler. Do this by adding the path to your `(dependency_name)_test` in the `$CLASSES` variable, and creating a target for the headers.

    The custom target should look like:
  ```
  dependency_name: dependency_name.h
  ```
2. Create the files (`dependency_name.cc`, `dependency_name.h`, `dependency_name_test.cc`)

## Adding Request handlers
After following the above methods to add code, in the header file:
* Add the following header dependencies: 
```
#include <string>
#include "../nginx-configparser/config_parser.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "request_handler.h"
```
* Extend the base Request Handler Class. This requires you to define two functions: Init() and HandleRequest()
```
class ExampleHandler : public RequestHandler {
  public:
    Status Init(const std::string& uri_prefix,
              const NginxConfig& config);
    
    Status HandleRequest(const Request& request, 
                  Response* response);
};
```
* Register the handler so the rest of the code is aware of your handler. The parameter to this macro should be your handler's class name.
```
REGISTER_REQUEST_HANDLER(ExampleHandler)
```