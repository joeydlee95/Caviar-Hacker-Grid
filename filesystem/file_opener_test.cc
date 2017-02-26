#include "gtest/gtest.h"
#include "file_opener.h"
#include <string>
TEST(FileOpenerTest, FileExist) { 
 
  EXPECT_TRUE(FileIO::FileExists("main.cc"));
}

TEST(FileOpenerTest, FileExtensionLocation) { 
 
  EXPECT_EQ(FileIO::FileExtensionLocation("main.cc"),4);
}

TEST(FileOpenerTest, FileToString) { 
  std::string content = "";
  FileIO::FileToString("filesystem/README_test.md",content);
  EXPECT_EQ(content,"# Caviar-Hacker-Grid\nCS130 Winter 2017 Webserver Project\n");
}