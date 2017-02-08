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
  bool find(const std::string& key, std::string& value, std::size_t depth = 1) {
    return mocked_find(key, value, depth);
  }
  bool find(const std::string& key, NginxConfig& value) {
    return mocked_find(key, value);
  }
	MOCK_METHOD3(mocked_find,bool(const std::string& key, std::string& value, std::size_t depth));  
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
  EXPECT_CALL(mock_config, mocked_find("port", An<std::string &>(), An<std::size_t>())).Times(0);
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
  EXPECT_CALL(mock_config, mocked_find("port", An<std::string &>(), An<std::size_t>()))
    .WillOnce(
  	  Return(false)	
    );
  EXPECT_FALSE(server.run_server(dummy_file.c_str()));
}

TEST(WebserverOptionsTest, ToStringTest) {
  std::unique_ptr<NginxConfig> conf(new MockNginxConfig);
  std::map<std::string, std::vector<std::string> >* options = new std::map<std::string, std::vector<std::string> >;
  std::vector<std::string> to_insert;
  to_insert.push_back("is");
  to_insert.push_back("tasty");
  options->insert(std::make_pair("cheese", to_insert));
  WebserverOptions opt(conf, options);
  EXPECT_EQ(opt.ToString(), "cheese: is tasty \n");
}