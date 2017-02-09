#include "http_404.h"
#include "http.h"

http::HTTPResponseBuilder404::HTTPResponseBuilder404(HTTPResponse* res) : HTTPResponseBuilder(res) {
    set_status_code(404);
    set_content_type(http::mime_type::CONTENT_TYPE_TEXT_HTML);

}