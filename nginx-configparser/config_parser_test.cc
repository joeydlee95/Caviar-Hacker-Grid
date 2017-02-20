#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "config_parser.h"
#include <boost/algorithm/string/join.hpp>

TEST(NginxConfigParserTest, SimpleFileConfig) { //first is test case bracket, second is the test cases name
  Nginx::NginxConfig out_config;

  bool success = Nginx::ParseFile("example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST(NginxConfigParserTest, NonexistentFileConfig) { //first is test case bracket, second is the test cases name
  Nginx::NginxConfig out_config;

  bool success = Nginx::ParseFile("nonexistent_config", &out_config);

  EXPECT_FALSE(success);
}


TEST(NginxConfigTest, ToString) {
  Nginx::NginxConfig statement;
  statement.tokens_.push_back("foo");
  statement.tokens_.push_back("bar");

  EXPECT_EQ(statement.ToStringSubBlock(0), "foo bar;\n"); // note that the 0 contains the indentation level
}

class NginxStringConfigTest : public ::testing::Test { //fixture, lets you define helper methods
protected:
  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return Nginx::ParseConfig(&config_stream, &out_config_);
  }

  bool FindSubConfig(const std::string key) {
    return out_config_.find(key, out_config_);
  }

  bool FindAllConfig(const std::string key) {
    out_configs_ = out_config_.findAll(key);
    return out_configs_.size() != 0;
  }

  bool FindString(const std::string key) {
    out_tokens_ =  out_config_.find(key);
    return out_tokens_.size() != 0;
  }

  std::vector<std::string> out_tokens_;
  Nginx::NginxConfig out_config_;
  std::vector<std::shared_ptr<Nginx::NginxConfig> > out_configs_;
};



TEST_F(NginxStringConfigTest, ToString) {
  ASSERT_TRUE(ParseString("port 8000;")); 
  EXPECT_EQ(out_config_.ToString(0), "port 8000;\n");
}

TEST_F(NginxStringConfigTest, EmbeddedToString) {
  ASSERT_TRUE(ParseString("server { port 8000; }")); 
  EXPECT_EQ(out_config_.ToString(0), "server {\n  port 8000;\n}\n");
  EXPECT_EQ(out_config_.ToString(1), "  server {\n    port 8000;\n  }\n");
}

TEST_F(NginxStringConfigTest, SimpleConfig) {
  
  EXPECT_TRUE(ParseString("foo bar;")); // note that we can treat the expects as straems
  EXPECT_EQ(1, out_config_.children_.size()) 
    << "Config has one statements";
  EXPECT_EQ(out_config_.children_[0]->tokens_[0], "foo");
  EXPECT_EQ(out_config_.children_[0]->tokens_[1], "bar");
}

TEST_F(NginxStringConfigTest, InvalidConfig) {
  EXPECT_FALSE(ParseString("foo bar"));
}


TEST_F(NginxStringConfigTest, NestedConfig) {
  EXPECT_TRUE(ParseString("server { listen 80; }"));
  // TODO: Test the contents of out_config_;
}

TEST_F(NginxStringConfigTest, UnbalancedBraces) {
  EXPECT_FALSE(ParseString("server { listen 80; "));
  EXPECT_FALSE(ParseString("server listen 80; } "));
}

TEST_F(NginxStringConfigTest, SemicolonIssues) {
  EXPECT_FALSE(ParseString("server { listen 80; }; "));
  EXPECT_FALSE(ParseString("server { listen 80; } \n \"test\" "));
}

TEST_F(NginxStringConfigTest, UnbalancedQuotations) {
  EXPECT_FALSE(ParseString("\"bar; "));
  EXPECT_FALSE(ParseString("\"bar;\";\" "));
}

TEST_F(NginxStringConfigTest, MultipleBlockConfig) {
  EXPECT_TRUE(ParseString("foo \"bar\"; \n server { listen 80;} server2 { listen 80; }"));
}

TEST_F(NginxStringConfigTest, EmbeddedBlockConfig) {
  EXPECT_TRUE(ParseString("server { server2 { listen 80; } }"));
}

TEST_F(NginxStringConfigTest, EmptyConfig) {
  EXPECT_TRUE(ParseString(""));
}

TEST_F(NginxStringConfigTest, SimpleCommentConfig) {
  EXPECT_TRUE(ParseString("# test"));
}

TEST_F(NginxStringConfigTest, ComplexCommentConfig) {
  EXPECT_TRUE(ParseString("server\n { server2 \n{ # test \ntestlisten 80;# test \n} \n}"));
}

TEST_F(NginxStringConfigTest, BlankBlockConfig) {
  EXPECT_TRUE(ParseString("server {}"));
}

TEST_F(NginxStringConfigTest, SingleQuoteTest) {
  EXPECT_TRUE(ParseString("port \'8000\';"));
  EXPECT_EQ(out_config_.children_[0]->tokens_[0], "port");
  EXPECT_EQ(out_config_.children_[0]->tokens_[1], "\'8000\'");
}

TEST_F(NginxStringConfigTest, FindTestSingleStatement) {
  ASSERT_TRUE(ParseString("port 8000;")); 
  EXPECT_TRUE(FindString("port"));
  EXPECT_TRUE(out_tokens_.size() == 2);
  EXPECT_EQ(out_tokens_[1], "8000");
  EXPECT_FALSE(FindString("8000")); 
}

TEST_F(NginxStringConfigTest, FindTestEmbeddedBlockInvalid) {
  ASSERT_TRUE(ParseString("server {\nport 8000;\n}")); 
  EXPECT_FALSE(FindString("port")); 
}

TEST_F(NginxStringConfigTest, FindSubConfig) {
  ASSERT_TRUE(ParseString("port 3000;\necho {/URL; }"));
  EXPECT_TRUE(FindString("port"));
  EXPECT_EQ(out_tokens_[1], "3000");
  
  EXPECT_TRUE(FindSubConfig("echo"));
  EXPECT_EQ(out_config_.ToString(), "/URL;\n"); 
}

TEST_F(NginxStringConfigTest, FindDuplicatePath) {
  ASSERT_TRUE(ParseString("port 3000;\necho {/URL; }\necho {/URL2; }"));
  EXPECT_TRUE(FindString("port"));
  EXPECT_EQ(out_tokens_[1], "3000");
  
  EXPECT_TRUE(FindAllConfig("echo"));
  EXPECT_EQ(out_configs_[0]->ToString(), "/URL;\n"); 
  EXPECT_EQ(out_configs_[1]->ToString(), "/URL2;\n"); 
}


TEST_F(NginxStringConfigTest, FindSubSubConfig) {
  ASSERT_TRUE(ParseString("port 3000;\necho { level2 { test; } }"));
  EXPECT_TRUE(FindString("port"));
  EXPECT_EQ(out_tokens_[1], "3000");
  
  ASSERT_TRUE(FindSubConfig("echo"));
  EXPECT_EQ(out_config_.ToString(), "level2 {\n  test;\n}\n");
  EXPECT_FALSE(FindString("port"));
  EXPECT_TRUE(FindString("level2"));

  ASSERT_TRUE(FindSubConfig("level2"));
  EXPECT_FALSE(FindString("port"));
  EXPECT_TRUE(FindString("test"));
  EXPECT_EQ(out_config_.ToString(), "test;\n");
  EXPECT_TRUE(FindSubConfig("test"));
  EXPECT_EQ(out_tokens_[0], "test");
  EXPECT_EQ(out_tokens_.size(), 1);
}

TEST_F(NginxStringConfigTest, RealConfigExample) {
  ASSERT_TRUE(ParseString("port 2020;\n\npath / StaticHandler {\n  root /foo/bar;\n}\n\npath /echo EchoHandler {}\n\ndefault NotFoundHandler {}"));
  printf("%s\n", out_config_.ToString().c_str());
  EXPECT_TRUE(FindString("port"));
  EXPECT_EQ(out_tokens_[1], "2020");
  EXPECT_EQ(out_tokens_.size(), 2);
  
  
  EXPECT_TRUE(FindAllConfig("path"));
  EXPECT_EQ(out_configs_.size(), 2);
  std::string joined_string = boost::algorithm::join(out_configs_[0]->tokens_, " ");
  EXPECT_EQ(joined_string, "path / StaticHandler");
  joined_string = boost::algorithm::join(out_configs_[1]->tokens_, " ");
  EXPECT_EQ(joined_string, "path /echo EchoHandler");

  EXPECT_TRUE(FindString("default"));
  EXPECT_EQ(out_tokens_[1], "NotFoundHandler");
}
