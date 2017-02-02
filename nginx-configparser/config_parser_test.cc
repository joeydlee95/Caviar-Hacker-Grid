#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) { //first is test case bracket, second is the test cases name
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("test_config", &out_config);

  EXPECT_TRUE(success);
}

TEST(NginxConfigParserTest, NonexistentConfig) { //first is test case bracket, second is the test cases name
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("dead_config", &out_config);

  EXPECT_FALSE(success);
}


TEST(NginxConfigTest, ToString) {
  NginxConfigStatement statement;
  statement.tokens_.push_back("foo");
  statement.tokens_.push_back("bar");

  EXPECT_EQ(statement.ToString(0), "foo bar;\n"); // note that the 0 contains the indentation level
}

class NginxStringConfigTest : public ::testing::Test { //fixture, lets you define helper methods
protected:
  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &out_config_);
  }
  NginxConfigParser parser_;
  NginxConfig out_config_;
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


TEST_F(NginxStringConfigTest, findTest) {
  std::string s = "port";
  ASSERT_TRUE(ParseString("port 8000;")); 
  EXPECT_TRUE(out_config_.find(s));
  EXPECT_EQ(s, "8000");

  EXPECT_FALSE(out_config_.find(s)); 
}

TEST_F(NginxStringConfigTest, findTestEmbeddedBlock) {
  std::string s = "port";
  ASSERT_TRUE(ParseString("server {\nport 8000;\n}")); 
  EXPECT_FALSE(out_config_.find(s)); 
}



TEST_F(NginxStringConfigTest, SimpleConfig) {
  
  EXPECT_TRUE(ParseString("foo bar;")); // note that we can treat the expects as straems
  EXPECT_EQ(1, out_config_.statements_.size()) 
    << "Config has one statements";
  EXPECT_EQ(out_config_.statements_[0]->tokens_[0], "foo");
  EXPECT_EQ(out_config_.statements_[0]->tokens_[1], "bar");
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


TEST_F(NginxStringConfigTest, SingleQuoteTest) {
  EXPECT_TRUE(ParseString("port \'8000\';"));
  EXPECT_EQ(out_config_.statements_[0]->tokens_[0], "port");
  EXPECT_EQ(out_config_.statements_[0]->tokens_[1], "\'8000\'");
}