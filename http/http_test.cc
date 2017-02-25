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
