#include "gtest/gtest.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "not_found_handler.h"
#include "../nginx-configparser/config_parser.h"
#include <vector>

TEST(EchoHandlerTest, BasicTest) {
  std::string raw_request= "GET /3 HTTP/1.1\r\nHost: localhost:3000\r\n\r\n";
  auto req = Request::Parse(raw_request);
  Response res;
  NotFoundHandler h;
  std::string expected_string = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n";
  auto handler_status = h.HandleRequest(*req, &res);  
  EXPECT_EQ(handler_status, RequestHandler::FILE_NOT_FOUND);
  EXPECT_EQ(res.ToString(), expected_string);
}

