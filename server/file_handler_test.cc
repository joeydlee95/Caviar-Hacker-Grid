#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "file_handler.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "../nginx-configparser/config_parser.h"
#include <vector>
using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::An;

class MockNginxConfig : public NginxConfig{
public:
  std::vector<std::string> find(const std::string& key) const {
    return mocked_find(key);
  }
  MOCK_CONST_METHOD1(mocked_find,std::vector<std::string>(const std::string& key));
};


TEST(FileHandlerTest, InitTest){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	set_tokens.push_back("/static");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("dummy", mock_config);
    EXPECT_EQ(handler.getPrefix(),"dummy");
    EXPECT_EQ(handler.getRoot(),"/static");
    EXPECT_EQ(ret, RequestHandler::OK);
}

TEST(FileHandlerTest, InitFail){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("dummy", mock_config);
   
    EXPECT_EQ(ret, RequestHandler::INVALID_CONFIG);
}

TEST(FileHandlerTest, HandleRequestTexT){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	set_tokens.push_back("files_served");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("/static1", mock_config);
    
    EXPECT_EQ(ret, RequestHandler::OK);
    std::string raw_req = "GET /static1/text_file.txt HTTP/1.1\r\nAccept-Languages: en-us\r\nAccept-Encoding: gzip, deflate\r\nContent-Length: length\r\n\r\n";
    auto req = Request::Parse(raw_req);

    Response resp;
    auto handler_status = handler.HandleRequest(*req,&resp);

    EXPECT_EQ(handler_status,RequestHandler::OK);
    EXPECT_EQ(resp.ToString(),"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n# Caviar-Hacker-Grid\nCS130 Winter 2017 Webserver Project\n");
}

TEST(FileHandlerTest, HandleRequestJPG){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	set_tokens.push_back("files_served");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("/stat", mock_config);
    
    EXPECT_EQ(ret, RequestHandler::OK);
    std::string raw_req = "GET /stat/jpg_file.jpg HTTP/1.1\r\nAccept-Languages: en-us\r\nAccept-Encoding: gzip, deflate\r\nContent-Length: length\r\n\r\n";
    auto req = Request::Parse(raw_req);

    Response resp;
    auto handler_status = handler.HandleRequest(*req,&resp);

    EXPECT_EQ(handler_status,RequestHandler::OK);
    EXPECT_EQ(resp.ToString(),"HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n# Caviar-Hacker-Grid\nCS130 This is an image");
}

TEST(FileHandlerTest, WrongPrefixPos){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	set_tokens.push_back("files_served");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("/stat", mock_config);
    
    EXPECT_EQ(ret, RequestHandler::OK);
    std::string raw_req = "GET /jpg_file.jpg/stat HTTP/1.1\r\nContent-Length: length\r\n\r\n";
    auto req = Request::Parse(raw_req);

    Response resp;
    auto handler_status = handler.HandleRequest(*req,&resp);

    EXPECT_EQ(handler_status,RequestHandler::BAD_REQUEST);
}

TEST(FileHandlerTest, NoPrefix){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	set_tokens.push_back("files_served");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("/stat", mock_config);
    
    EXPECT_EQ(ret, RequestHandler::OK);
    std::string raw_req = "GET /jpg_file.jpg HTTP/1.1\r\nContent-Length: length\r\n\r\n";
    auto req = Request::Parse(raw_req);

    Response resp;
    auto handler_status = handler.HandleRequest(*req,&resp);

    EXPECT_EQ(handler_status,RequestHandler::BAD_REQUEST);

    EXPECT_EQ(resp.ToString(),"HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n");
}

TEST(FileHandlerTest, NotFound){
	MockNginxConfig mock_config;
	std::vector<std::string> set_tokens;
	set_tokens.push_back("root");
	set_tokens.push_back("files_served");
	EXPECT_CALL(mock_config, mocked_find("root"))
    .WillOnce(
  	  Return(set_tokens)	
    );	
    StaticFileHandler handler;
    RequestHandler::Status ret = handler.Init("/static1", mock_config);
    
    EXPECT_EQ(ret, RequestHandler::OK);
    std::string raw_req = "GET /static1/text_f.txt HTTP/1.1\r\nContent-Length: length\r\n\r\n";
    auto req = Request::Parse(raw_req);

    Response resp;
    auto handler_status = handler.HandleRequest(*req,&resp);

    EXPECT_EQ(handler_status,RequestHandler::FILE_NOT_FOUND);
    EXPECT_EQ(resp.ToString(),"HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
}
