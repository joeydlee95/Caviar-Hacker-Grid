#include "gtest/gtest.h"
#include "http.h"
#include "http_404.h"

TEST(HTTPResponse404, ToString) {
  http::HTTPResponse* res = new http::HTTPResponse();
  http::HTTPResponseBuilder404 builder(res);
  
  EXPECT_EQ(builder.getResult()->ToString(), "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");

  delete res;

}

