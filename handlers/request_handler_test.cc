#include "gtest/gtest.h"

#include <string>
#include <map>

#include "request_handler.h"

// Insert tests for Registerer and CreateByName here
TEST(RequestHandlerKeyMatchTest, NormalCase) { //first is test case bracket, second is the test cases name
  HandlerConfiguration config;
  config.RequestHandlers = new HandlerMap;
  config.RequestHandlers->insert(std::make_pair("/", nullptr));
  config.RequestHandlers->insert(std::make_pair("/echo", nullptr));
  config.RequestHandlers->insert(std::make_pair("/serve", nullptr));
  config.RequestHandlers->insert(std::make_pair("/status", nullptr));
  

  EXPECT_EQ(config.FindLongestHandlerKey("/"), "/");
  EXPECT_EQ(config.FindLongestHandlerKey("/echo"), "/echo");
  EXPECT_EQ(config.FindLongestHandlerKey("/serve"), "/serve");
  EXPECT_EQ(config.FindLongestHandlerKey("/status"), "/status");

  config.RequestHandlers->insert(std::make_pair("/status/test", nullptr));
  EXPECT_EQ(config.FindLongestHandlerKey("/status/test"), "/status/test");


}
