#include "gtest/gtest.h"
#include "../http/httpRequest.h"
#include "../http/httpResponse.h"
#include "echo_handler.h"
#include "../nginx-configparser/config_parser.h"
#include <vector>

TEST(EchoHandlerTest, BasicTest) {
  std::string raw_request= "GET /echo HTTP/1.1\r\nHost: localhost:3000\r\n\r\n";
  auto req = Request::Parse(raw_request);
  Response res;
  EchoHandler eh;
  std::string expected_string = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\nHost: localhost:3000\r\n\r\n";
  auto handler_status = eh.HandleRequest(*req, &res);  
  EXPECT_EQ(handler_status, RequestHandler::OK);
  EXPECT_EQ(res.ToString(), expected_string);
}

