#include <string>
#include "gtest/gtest.h"
#include "webserver_options.h"
#include <map>
#include <string>
#include <vector>
#include <sstream>

TEST(WebServerOptionTest, TestInit) { 
  NginxConfigParser parser;
  NginxConfig out_config;

  std::stringstream config_stream("location /echo {\n    root nginx-configparser;\n}\n");
  parser.Parse(&config_stream, &out_config);
  
  auto statement = out_config.findAll("location")[0];

  // EXPECT_EQ(statement->tokens_[1],"/echo");

  std::map<std::string, std::vector<std::string> >* options = new std::map<std::string, std::vector<std::string> >;
  
  WebserverOptions opt(statement->child_block_, options);

  
  EXPECT_EQ(opt.ToString(),"root: nginx-configparser \n");
}

TEST(WebServerOptionTest, TestInit2) { 
  NginxConfigParser parser;
  NginxConfig out_config;

  std::stringstream config_stream("port 3000;\n\nlocation /URL {\n    root /path/to/file;\n}\n\nlocation /echo {\n    echo;\n}\n");
  parser.Parse(&config_stream, &out_config);
  
  //get the second childblock with the key "location"
  auto statement = out_config.findAll("location")[1];

  std::map<std::string, std::vector<std::string> >* options = new std::map<std::string, std::vector<std::string> >;
  
  WebserverOptions opt(statement->child_block_, options);

  //The Fornatting of ToString is strange with this input
  EXPECT_EQ(opt.ToString(),"echo: \n");
}