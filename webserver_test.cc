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

TEST(WebserverTest, Configuration) {
  MockConfigParser mock_parser;
  NginxConfig injected_config;

  Webserver server(&mock_parser,&injected_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_parser, Parse(dummy_file.c_str(), _))
    .WillOnce(
     
      Return(true));
  EXPECT_TRUE(server.parse_config(dummy_file.c_str()));
}

TEST(WebserverTest, ConfigurationFail) {
  //Test for error handling
  MockConfigParser mock_parser;
  NginxConfig injected_config;

  Webserver server(&mock_parser,&injected_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_parser, Parse(dummy_file.c_str(), _))
    .WillOnce(
     
      Return(false));
  EXPECT_FALSE(server.parse_config(dummy_file.c_str()));
}


TEST(WebserverTest, GetPort){
	NginxConfigParser injected_parser;
	MockNginxConfig mock_config;

	Webserver server(&injected_parser,&mock_config);
  	std::string port_str = "dummy";

  	EXPECT_CALL(mock_config, find(port_str))
    .WillOnce(DoAll(
    	SetArgReferee<0>("3000"),
    	Return(true)));

    EXPECT_TRUE(server.get_port_from_config(port_str));
    EXPECT_EQ(port_str,"3000");
}

TEST(WebserverTest, GetPortFail){
	NginxConfigParser injected_parser;
	MockNginxConfig mock_config;

	Webserver server(&injected_parser,&mock_config);
  	std::string port_str = "dummy";

  	EXPECT_CALL(mock_config, find(port_str))
    .WillOnce(Return(false));

    EXPECT_FALSE(server.get_port_from_config(port_str));
    
}