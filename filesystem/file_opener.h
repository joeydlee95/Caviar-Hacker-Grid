#ifndef FILE_OPENER_H
#define FILE_OPENER_H

#include <string>
namespace FileIO {
  bool FileExists(std::string fname);
  void FileToString(std::string fname, std::string& str);
  void FileToStream(std::string fname, std::ifstream& stream);
  std::size_t FileExtensionLocation(std::string fname);
}

#endif // If Guard