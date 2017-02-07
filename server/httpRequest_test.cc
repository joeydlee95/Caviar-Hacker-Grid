#include "gtest/gtest.h"
#include "httpRequest.h"
#include <string>

TEST(HttpRequestTest, SimpleTest) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";

  bool success = parser.Parse(request);

  EXPECT_TRUE(success);
}

TEST(HttpRequestTest, HttpMethod) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";

  parser.Parse(request);

  EXPECT_EQ(parser.getMethod(),HttpRequest::GET);
}

TEST(HttpRequestTest, ResourceUri) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";

  parser.Parse(request);

  EXPECT_EQ(parser.getResourcePath(),"http://www.w3.org/pub/WWW/TheProject.html");
}

TEST(HttpRequestTest, MessageBody) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";

  parser.Parse(request);

  EXPECT_EQ(parser.getMessageBody(),"Host: www.w3.org");
}

TEST(HttpRequestTest, MessageBodyWithHeaders) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\nContent-Length: length\r\nContent-Length: length\r\n\r\nHost: www.w3.org";

  parser.Parse(request);

  EXPECT_EQ(parser.getMessageBody(),"Host: www.w3.org");
}


TEST(HttpRequestTest, EmptyHeaderParse) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\n";

  parser.Parse(request);

  EXPECT_EQ(parser.header_fields_.size(),0);
}

TEST(HttpRequestTest, HeaderParse) { 
  HttpRequest parser;

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\nAccept-Languages: en-us\r\nAccept-Encoding: gzip, deflate\r\nContent-Length: length\r\n\r\n";

  parser.Parse(request);

  EXPECT_EQ(parser.header_fields_["Content-Length"],"length");
  EXPECT_EQ(parser.header_fields_["Accept-Languages"],"en-us");
  EXPECT_EQ(parser.header_fields_["Accept-Encoding"],"gzip, deflate");

}

