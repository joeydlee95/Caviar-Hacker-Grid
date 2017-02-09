#ifndef HTTP_404_H
#define HTTP_404_H

#include "http.h"

namespace http {
  class HTTPResponseBuilder404 : public http::HTTPResponseBuilder {
  public:
    HTTPResponseBuilder404(HTTPResponse* res);
  };
}


#endif // If Guard