#include "gtest/gtest.h"
#include "http.h"
#include "http_file.h"

TEST(HTTPResponseFile, ToString) {
  http::HTTPResponse* res = new http::HTTPResponse();
  http::HTTPResponse* res2 = new http::HTTPResponse();
  http::HTTPResponseBuilderFile builder(res, "googletest/travis.sh");
  
  
  EXPECT_EQ(builder.getResult()->ToString(), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n#!/usr/bin/env sh\nset -evx\nenv | sort\n\nmkdir build || true\nmkdir build/$GTEST_TARGET || true\ncd build/$GTEST_TARGET\ncmake -Dgtest_build_samples=ON \\\n      -Dgmock_build_samples=ON \\\n      -Dgtest_build_tests=ON \\\n      -Dgmock_build_tests=ON \\\n      -DCMAKE_CXX_FLAGS=$CXX_FLAGS \\\n      ../../$GTEST_TARGET\nmake\nCTEST_OUTPUT_ON_FAILURE=1 make test\n");
  http::HTTPResponseBuilderFile builder2(res2, "cheese");
  EXPECT_EQ(builder2.getResult()->ToString(), "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
  delete res;

}

