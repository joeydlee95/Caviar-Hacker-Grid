#include "file_opener.h"
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
bool FileIO::FileExists(std::string fname) {
  struct stat s;
  if(stat(fname.c_str(), &s) == 0 && s.st_mode & S_IFREG) {
    return true;
  }
  else {
    return false;
  }
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

std::size_t FileIO::FileExtensionLocation(std::string fname) {
  return fname.find_last_of(".");
}