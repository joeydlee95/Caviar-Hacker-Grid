#include "http_file.h"
#include "http.h"
#include "../filesystem/file_opener.h"

http::HTTPResponseBuilderFile::HTTPResponseBuilderFile(HTTPResponse* res, std::string file_directory) : HTTPResponseBuilder(res) {
  //op
  printf("file: %s", file_directory.c_str());
  if(FileIO::FileExists(file_directory)) {
    printf("File exists\n");
    set_status_code(200);
    // find the file extension, set the content type based on this.
    set_content_type(http::mime_type::CONTENT_TYPE_TEXT_PLAIN);
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