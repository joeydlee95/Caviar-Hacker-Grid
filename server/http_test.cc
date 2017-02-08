#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "http.h"

TEST(StatusCode, StatusCodeValidation) {
  http::status_code s;
  for(int i = -700; i < 700; i++) {
    if(i < 100 || i > 599) {
      EXPECT_FALSE(s.set(i));
    }
    else {
      EXPECT_TRUE(s.set(i));
      EXPECT_EQ(s.status_code_, i);
    }
  }
}

TEST(HTTPResponse, ToString) {
  http::HTTPResponse* res = new http::HTTPResponse();
  http::HTTPResponseBuilder builder(res);
  
  EXPECT_TRUE(builder.set_status_code(200));
  EXPECT_EQ(builder.reason_phrase().reason_phrase_, "OK");

  builder.set_length(10);
  builder.set_content_type(http::mime_type::CONTENT_TYPE_TEXT_PLAIN);
  builder.set_body("Cheesy");

  EXPECT_EQ(builder.getResult()->ToString(), "HTTP/1.1 200 OK\r\nContent-Length: 10\r\nContent-Type: text/plain\r\n\r\nCheesy");

  delete res;

}

