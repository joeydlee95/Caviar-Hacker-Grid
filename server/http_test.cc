#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "server/http.h"

TEST(HTTPResponseTest, StatusCodeValidation) {
  http::status_code s();
  for(int i = -700; i < 700; i++) {
    if(i < 100 || i > 599) {
      EXPECT_FALSE(s.set(i));
    }
    else {
      EXPECT_TRUE(s.set(i));
      EXPECT_EQ(s.status_code_ == i);
    }
  }
}


class NginxStringConfigTest : public ::testing::Test { //fixture, lets you define helper methods
protected:
  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &out_config_);
  }

  bool FindConfig(const std::string key) {
    return out_config_.find(key, out_config_);
  }
  NginxConfigParser parser_;
  NginxConfig out_config_;
};