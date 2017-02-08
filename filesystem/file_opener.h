#ifndef FILE_OPENER_H
#define FILE_OPENER_H

#include <string>
namespace FileIO {
  bool FileExists(std::string fname);
  void FileToString(std::string fname, std::string& str);
  void FileToStream(std::string fname, std::ifstream& stream);
}

#endif // If Guard