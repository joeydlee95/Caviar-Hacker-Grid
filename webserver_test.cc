#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "webserver.h"
#include "nginx-configparser/config_parser.h"
#include <string>
#include <boost/asio.hpp>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::An;


class MockConfigParser: public NginxConfigParser{
public:
	MOCK_METHOD2(Parse,bool(const char* file_name, NginxConfig* config));
	
};

class MockNginxConfig: public NginxConfig{
public:
  bool find(const std::string& key, std::string& value, int depth = 1) {
    return mocked_find(key, value, depth);
  }
  bool find(const std::string& key, NginxConfig& value) {
    return mocked_find(key, value);
  }
	MOCK_METHOD3(mocked_find,bool(const std::string& key, std::string& value, int depth));  
  MOCK_METHOD2(mocked_find,bool(const std::string& key, NginxConfig& value));
};

TEST(WebserverTest, ConfigurationFail) {
  MockConfigParser mock_parser;
  MockNginxConfig mock_config;

  Webserver server(&mock_parser,&mock_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_parser, Parse(dummy_file.c_str(), _))
    .WillOnce( 
      Return(false)
    );
  EXPECT_CALL(mock_config, mocked_find("port", An<std::string &>(), An<int>())).Times(0);
  EXPECT_FALSE(server.run_server(dummy_file.c_str()));
}

TEST(WebserverTest, FindPortFail) {
  
  MockConfigParser mock_parser;
  MockNginxConfig mock_config;

  Webserver server(&mock_parser,&mock_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_parser, Parse(dummy_file.c_str(), _))
    .WillOnce(
      Return(true)
    );
  EXPECT_CALL(mock_config, mocked_find("port", An<std::string &>(), An<int>()))
    .WillOnce(
  	  Return(false)	
    );
  EXPECT_FALSE(server.run_server(dummy_file.c_str()));
}


