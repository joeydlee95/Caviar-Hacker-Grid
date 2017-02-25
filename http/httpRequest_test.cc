#include "gtest/gtest.h"
#include "httpRequest.h"
#include <string>

TEST(HttpRequestTest, SimpleParse) { 
  
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";
  auto req_parsed = Request::Parse(request);

  EXPECT_NE(req_parsed,nullptr);
}

TEST(HttpRequestTest, SimpleParseFail) { 
  
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\nHost: www.w3.org";
  auto req_parsed = Request::Parse(request);

  EXPECT_EQ(req_parsed,nullptr);
}


TEST(HttpRequestTest, HttpMethod) { 
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";
  auto req_parsed = Request::Parse(request);

  EXPECT_EQ(req_parsed->method(),"GET");
}

TEST(HttpRequestTest, ResourceUri) { 
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";
  auto req_parsed = Request::Parse(request);

  EXPECT_EQ(req_parsed->uri(),"http://www.w3.org/pub/WWW/TheProject.html");
}


TEST(HttpRequestTest, MessageBody) { 
  

  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\nHost: www.w3.org";

  auto req_parsed = Request::Parse(request);

  EXPECT_EQ(req_parsed->body(),"Host: www.w3.org");
}

TEST(HttpRequestTest, MessageBodyWithHeaders) { 
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\nContent-Length: length\r\nContent-Length: length\r\n\r\nHost: www.w3.org";

  auto req_parsed = Request::Parse(request);
  EXPECT_EQ(req_parsed->body(),"Host: www.w3.org");
}


TEST(HttpRequestTest, EmptyHeaderParse) { 
  
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\n\r\n";

  auto req_parsed = Request::Parse(request);

  EXPECT_EQ(req_parsed->headers().size(),0);
}

bool FindAndMatchHeader(const std::string header, const std::string value, const Request::Headers headers_vec){
  bool found = false;
  for(const auto& a_pair : headers_vec){
    if(a_pair.first == header){ 
      found = true;
      EXPECT_EQ(a_pair.second,value);
    }
  }
  return found;
}

TEST(HttpRequestTest, HeaderParse) { 
  std::string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1\r\nAccept-Languages: en-us\r\nAccept-Encoding: gzip, deflate\r\nContent-Length: length\r\n\r\n";

  auto req_parsed = Request::Parse(request);
  auto headers_vec = req_parsed->headers();

  EXPECT_EQ(FindAndMatchHeader("Content-Length","length",headers_vec),true);
  EXPECT_EQ(FindAndMatchHeader("Accept-Languages","en-us",headers_vec),true);

  EXPECT_EQ(FindAndMatchHeader("Accept-Lang","en-us",headers_vec),false);

}






