#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "httpRequest.h"
#include "httpMutableRequest.h"
#include <string>

using ::testing::HasSubstr;
using ::testing::Not;

TEST(HttpMutableRequestTest, CopyConstructToString) {
	std::string requestStr = (
			"GET /uri/path HTTP/1.1\r\n"
			"Accept-Languages: en-us\r\n"
			"\r\n"
			);
	auto request = Request::Parse(requestStr);
	ASSERT_NE(request, nullptr);

	MutableRequest mut_request(*request);

	std::string result = mut_request.ToString();
	EXPECT_THAT(result, HasSubstr("GET /uri/path HTTP/1.1\r\n"));
	EXPECT_THAT(result, HasSubstr("Accept-Languages: en-us\r\n"));
	EXPECT_THAT(result, HasSubstr("\r\n\r\n"));
}

TEST(HttpMutableRequestTest, SetUri) {
	std::string requestStr = (
			"GET /uri/path HTTP/1.1\r\n"
			"Accept-Languages: en-us\r\n"
			"\r\n"
			);
	auto request = Request::Parse(requestStr);
	ASSERT_NE(request, nullptr);

	MutableRequest mut_request(*request);

	mut_request.SetURI("/new/uri/thing");

	std::string result = mut_request.ToString();
	EXPECT_THAT(result, HasSubstr("GET /new/uri/thing HTTP/1.1\r\n"));
}

TEST(HttpMutableRequestTest, SetHeader_new) {
	std::string requestStr = (
			"GET /uri/path HTTP/1.1\r\n"
			"Accept-Languages: en-us\r\n"
			"\r\n"
			);
	auto request = Request::Parse(requestStr);
	ASSERT_NE(request, nullptr);

	MutableRequest mut_request(*request);

	mut_request.SetHeader("Connection", "close");

	std::string result = mut_request.ToString();
	EXPECT_THAT(result, HasSubstr("GET /uri/path HTTP/1.1\r\n"));
	EXPECT_THAT(result, HasSubstr("Accept-Languages: en-us\r\n"));
	EXPECT_THAT(result, HasSubstr("Connection: close\r\n"));
	EXPECT_THAT(result, HasSubstr("\r\n\r\n"));
}

TEST(HttpMutableRequestTest, SetHeader_overwrite) {
	std::string requestStr = (
			"GET /uri/path HTTP/1.1\r\n"
			"Accept-Languages: en-us\r\n"
			"Connection: keep-alive\r\n"
			"\r\n"
			);
	auto request = Request::Parse(requestStr);
	ASSERT_NE(request, nullptr);

	MutableRequest mut_request(*request);

	mut_request.SetHeader("Connection", "close");

	std::string result = mut_request.ToString();
	EXPECT_THAT(result, HasSubstr("GET /uri/path HTTP/1.1\r\n"));
	EXPECT_THAT(result, HasSubstr("Accept-Languages: en-us\r\n"));
	EXPECT_THAT(result, HasSubstr("Connection: close\r\n"));
	EXPECT_THAT(result, Not(HasSubstr("keep-alive")));
	EXPECT_THAT(result, HasSubstr("\r\n\r\n"));
}

