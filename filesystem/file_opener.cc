#include "file_opener.h"
#include <string>
#include <sstream>
#include <fstream>
bool FileIO::FileExists(std::string fname) {
  std::ifstream f(fname.c_str());
  return f.is_open();
}

// http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
void FileIO::FileToString(std::string fname, std::string& str) {
  std::ifstream t(fname);
  std::stringstream buffer;
  buffer << t.rdbuf();
  str = buffer.str();
}

void FileIO::FileToStream(std::string fname, std::ifstream& stream) {
  stream.open(fname.c_str());
}