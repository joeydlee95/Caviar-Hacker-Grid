#include "http_file.h"
#include "http.h"

http::HTTPResponseBuilderFile::HTTPResponseBuilderFile(HTTPResponse* res, std::string file_directory) : HTTPResponseBuilder(res) {
    set_status_code(200);
    set_content_type(http::mime_type::CONTENT_TYPE_TEXT_PLAIN);
    //BROKEN AF>
    
}