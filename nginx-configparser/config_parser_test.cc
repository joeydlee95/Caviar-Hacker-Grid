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

  bool FindConfig(const std::string key) {
    return out_config_.find(key, out_config_);
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
  EXPECT_TRUE(out_config_.find("port", s));
  EXPECT_EQ(s, "8000");
  EXPECT_FALSE(out_config_.find("port", s, 2));
  EXPECT_FALSE(out_config_.find("8000", s)); 
}

TEST_F(NginxStringConfigTest, findTestEmbeddedBlock) {
  std::string s = "port";
  ASSERT_TRUE(ParseString("server {\nport 8000;\n}")); 
  EXPECT_FALSE(out_config_.find("port", s)); 
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

// TODO: ADD A CONFIG TEST WITH DUPLICATE BODIES.

TEST_F(NginxStringConfigTest, FindSubConfig) {
  NginxConfig config;
  std::string s = "";
  ASSERT_TRUE(ParseString("port 3000;\necho {/URL; }"));
  EXPECT_TRUE(out_config_.find("port", s));
  EXPECT_EQ(s, "3000");
  
  EXPECT_TRUE(out_config_.find("echo", config));
  EXPECT_EQ(config.ToString(), "/URL;\n");
}


TEST_F(NginxStringConfigTest, FindSubSubConfig) {
  NginxConfig config;
  NginxConfig sub_config;
  std::string s = "";
  ASSERT_TRUE(ParseString("port 3000;\necho { level2 { test; } }"));
  EXPECT_TRUE(out_config_.find("port", s));
  EXPECT_EQ(s, "3000");
  
  ASSERT_TRUE(out_config_.find("echo", config));
  EXPECT_EQ(config.ToString(), "level2 {\n  test;\n}\n");
  EXPECT_FALSE(config.find("port", s, 0));
  EXPECT_TRUE(config.find("level2", s, 0));
  EXPECT_FALSE(config.find("level2", s));

  ASSERT_TRUE(config.find("level2", sub_config));
  EXPECT_FALSE(sub_config.find("port", s));
  EXPECT_FALSE(sub_config.find("test", s));
  EXPECT_TRUE(sub_config.find("test", s, 0));
  EXPECT_EQ(s, "test");
}


TEST_F(NginxStringConfigTest, RealConfigExample) {
  NginxConfig config;
  std::string s = "";
  ASSERT_TRUE(ParseString("port 3000;\n\nlocation /URL {\n    root /path/to/file;\n}\n\nlocation /echo {\n    echo;\n}\n"));
  printf("port 3000;\n\nlocation /URL {\n    root /path/to/file;\n}\n\nlocation /echo {\n    echo;\n}\n");
  EXPECT_TRUE(out_config_.find("port", s));
  EXPECT_EQ(s, "3000");
  

  std::vector<std::shared_ptr<NginxConfigStatement> > statements = out_config_.findAll("location");
  ASSERT_EQ(statements.size(), 2);
  NginxConfigStatement* s0 = statements[0].get();
  NginxConfigStatement* s1 = statements[1].get();
  ASSERT_NE(s0, nullptr);
  ASSERT_NE(s1, nullptr);
  ASSERT_EQ(s0->ToString(0), "location /URL {\n  root /path/to/file;\n}\n");
  ASSERT_EQ(s1->ToString(0), "location /echo {\n  echo;\n}\n");

  std::string path = "";
  EXPECT_TRUE(s0->child_block_.get()->find("root", path));
  EXPECT_EQ(path, "/path/to/file");

  EXPECT_FALSE(s0->child_block_.get()->find("echo", path, 0));
  EXPECT_TRUE(s1->child_block_.get()->find("echo", path, 0));
  EXPECT_EQ(path, "echo");
}
