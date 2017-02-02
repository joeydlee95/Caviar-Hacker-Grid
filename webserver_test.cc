#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "webserver.h"
#include "nginx-configparser/config_parser.h"
#include <string>
#include <boost/asio.hpp>

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;


class MockConfigParser: public NginxConfigParser{
public:
	MOCK_METHOD2(Parse,bool(const char* file_name, NginxConfig* config));
	
};

class MockNginxConfig: public NginxConfig{
public:
	MOCK_METHOD1(find,bool(std::string& s));
};

TEST(WebserverTest, ConfigurationFail) {
  MockConfigParser mock_parser;
  MockNginxConfig mock_config;

  Webserver server(&mock_parser,&mock_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_parser, Parse(dummy_file.c_str(), _))
    .WillOnce(
     
      Return(false));
  EXPECT_CALL(mock_config, find(_)).Times(0);
  EXPECT_FALSE(server.run_server(dummy_file.c_str()));
}

TEST(WebserverTest, FindPortFail) {
  
  MockConfigParser mock_parser;
  MockNginxConfig mock_config;

  Webserver server(&mock_parser,&mock_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_parser, Parse(dummy_file.c_str(), _))
    .WillOnce(
     
      Return(true));
  EXPECT_CALL(mock_config, find(_)).WillOnce(
  	  Return(false)	);
  EXPECT_FALSE(server.run_server(dummy_file.c_str()));
}


