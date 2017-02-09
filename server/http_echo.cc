#include "http_echo.h"
#include "http.h"

http::HTTPResponseBuilderEcho::HTTPResponseBuilderEcho(HTTPResponse* res, std::string buf) : HTTPResponseBuilder(res) {
    set_status_code(200);
    set_content_type(http::mime_type::CONTENT_TYPE_TEXT_PLAIN);
    set_body(buf);

}