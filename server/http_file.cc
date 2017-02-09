#include "http_file.h"
#include "http.h"
#include "../filesystem/file_opener.h"

http::HTTPResponseBuilderFile::HTTPResponseBuilderFile(HTTPResponse* res, std::string file_directory) : HTTPResponseBuilder(res) {
  printf("file: %s\n", file_directory.c_str());
  if(FileIO::FileExists(file_directory)) {
    printf("File exists\n");
    set_status_code(200);
    // find the file extension, set the content type based on this.
    std::size_t index_of_substr = FileIO::FileExtensionLocation(file_directory);
    if(index_of_substr == std::string::npos) {
      set_content_type(http::mime_type::CONTENT_TYPE_APP_OCTET_STREAM);
    }
    else {
      std::string file_extension = file_directory.substr(index_of_substr + 1);
      printf("File extension: %s\n", file_extension.c_str());
      set_content_type(http::mime_type::GetMimeType(file_extension));
    }
    
    std::string body;
    FileIO::FileToString(file_directory, body);
    set_body(body);
  }
  else {
    printf("File doesn't exist\n");
    set_status_code(404);
    set_content_type(http::mime_type::CONTENT_TYPE_TEXT_HTML);
  }

}