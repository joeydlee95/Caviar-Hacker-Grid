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


/*
class MockNginxConfig: public Nginx::NginxConfig{
public:
  bool find(const std::string& key, Nginx::NginxConfig& value) const {
    return mocked_find(key, value);
  }
  bool td::vector<std::string> find(const std::string& key) const {
    return mocked_find(key);
  }
	MOCK_METHOD2(mocked_find,bool(const std::string& key, Nginx::NginxConfig& value));  
  MOCK_METHOD1(mocked_find,std::vector<std::string>(const std::string& key));
};

TEST(WebserverTest, ConfigurationFail) {
  MockNginxConfig mock_config;

  Webserver server(&mock_parser,&mock_config);
  std::string dummy_file = "dummy";
  
  EXPECT_CALL(mock_config, mocked_find("port", mock_config)).Times(0);
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
}*/