#ifndef HTTP_ECHO_H
#define HTTP_ECHO_H

#include "http.h"

namespace http {
  class HTTPResponseBuilderEcho : public http::HTTPResponseBuilder {
  public:
    HTTPResponseBuilderEcho(HTTPResponse* res, std::string buf);
  };
}


#endif // If Guard