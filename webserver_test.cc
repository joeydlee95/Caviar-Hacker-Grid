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



class MockNginxConfig : public Nginx::NginxConfig{
public:
  bool find(const std::string& key, Nginx::NginxConfig& value) const {
    return mocked_find(key, value);
  }
  std::vector<std::string> find(const std::string& key) const {
    return mocked_find(key);
  }
	MOCK_CONST_METHOD2(mocked_find,bool(const std::string& key, Nginx::NginxConfig& value));  
  MOCK_CONST_METHOD1(mocked_find,std::vector<std::string>(const std::string& key));
};

/*
TEST(WebserverTest, FindPortFail) {
  MockNginxConfig mock_config;
  mock_config.children_.emplace_back(new MockNginxConfig);
  mock_config.children_[0]->tokens_.push_back("dummy");

  Webserver server(&mock_config);
  EXPECT_CALL(mock_config, mocked_find("port"))
    .WillOnce(
  	  Return(false)	
    );

  EXPECT_FALSE(server.Init());
}
*/

/*
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