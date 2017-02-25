#include "gtest/gtest.h"
#include "status_handler.h"

#include <vector>

#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "../nginx-configparser/config_parser.h"


TEST(StatusHandlerTest, BasicTest) {
  std::string raw_request= "GET /echo HTTP/1.1\r\nHost: localhost:3000\r\n\r\n";
  auto req = Request::Parse(raw_request);
  Response res;
  StatusHandler h;
  std::string expected_string = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\nHost: localhost:3000\r\n\r\n";
  auto handler_status = h.HandleRequest(*req, &res);  
  EXPECT_EQ(handler_status, RequestHandler::OK);
  EXPECT_EQ(res.ToString(), expected_string);
}

