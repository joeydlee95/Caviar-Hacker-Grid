#include "gtest/gtest.h"
#include "httpResponse.h"
#include "http.h"
#include <string>

TEST(ResponseTest, CompleteTest){
	Response res;
	res.SetStatus(Response::OK);
	res.AddHeader("Content-Type",http::mime_type::ContentTypeAsString(http::mime_type::CONTENT_TYPE_TEXT_HTML));
	res.SetBody("yo!");
	res.AddHeader("Accept-Languages","en-us");
	EXPECT_EQ(res.ToString(),"HTTP/1.1 200 OK\r\nAccept-Languages: en-us\r\nContent-Type: text/html\r\n\r\nyo!");
}
