#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <vector>
#include <map>
#include <istream>
#include <boost/asio/streambuf.hpp>

namespace http {

  class status_code {
    public:
      bool set(int code);
      int status_code_;
  };

  class reason_phrase {
    public:
      void setDefault(int code);
      std::string reason_phrase_;
  };

  class mime_type {
    public:
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
      enum ContentType {
        CONTENT_TYPE_TEXT_PLAIN = 0, /* Default for unknown text documents */
        CONTENT_TYPE_TEXT_HTML,
        CONTENT_TYPE_TEXT_CSS,
        CONTENT_TYPE_TEXT_JS,
        CONTENT_TYPE_TEXT_WC, /* Wildcard */

        CONTENT_TYPE_IMAGE_JPEG,
        CONTENT_TYPE_IMAGE_PNG,
        CONTENT_TYPE_IMAGE_GIF,
        CONTENT_TYPE_IMAGE_SVG_XML,
        CONTENT_TYPE_IMAGE_BMP,
        CONTENT_TYPE_IMAGE_WEBP,
        CONTENT_TYPE_IMAGE_WC, 

        CONTENT_TYPE_AUDIO_MPEG,
        CONTENT_TYPE_AUDIO_OGG,
        CONTENT_TYPE_AUDIO_WEBM,
        CONTENT_TYPE_AUDIO_MIDI,
        CONTENT_TYPE_AUDIO_WAV,
        CONTENT_TYPE_AUDIO_WC, 

        CONTENT_TYPE_VIDEO_MP4,
        CONTENT_TYPE_VIDEO_WEBM,
        CONTENT_TYPE_VIDEO_OGG,
        CONTENT_TYPE_VIDEO_WC,

        CONTENT_TYPE_APP_OCTET_STREAM, /* Default for unknown binary data */
        CONTENT_TYPE_APP_PKCS12,
        CONTENT_TYPE_APP_VND_mspowerpoint,
        CONTENT_TYPE_APP_XHTML_XML,
        CONTENT_TYPE_APP_XML,
        CONTENT_TYPE_APP_PDF,
      };

      static std::string ContentTypeAsString(ContentType type);
  };

  struct http_field {
    public:
      std::string field_name;
      std::string field_value;
  };


  class http_headers {
    public:
      std::map<std::string, std::string> fields_;
  };

  class HTTPResponse {
    public:
      HTTPResponse();
      std::string ToString();

      reason_phrase reason_phrase_;
      status_code status_code_;
      mime_type mime_type_;
      http_headers http_headers_;
      std::string http_version_ = "HTTP/1.1";

      // TODO: Make this a data stream
      std::string body_;
      
      const char* line_break = "\r\n";
      
  };


  class HTTPResponseBuilder {
    // API based off of: 
    // https://microsoft.github.io/cpprestsdk/classweb_1_1http_1_1http__response.html#a1272e1a1e855c1433abd31a76ef3be97
    public:       
      HTTPResponseBuilder(HTTPResponse* res) : response_(res) {

      };

      status_code& status_code() const;
      bool set_status_code(int code);

      reason_phrase& reason_phrase() const;
      void set_reason_phrase(std::string phrase);

      http_headers& headers();
      http_headers& headers() const;

      void set_length(std::size_t length);
      void set_content_type(mime_type::ContentType type);
      // Used to set arbitrary headers.
      void set_header(http_field fields);
      void set_header(std::string field_name, std::string field_value);
      void set_headers(std::vector<http_field> fields);

      // TODO: add UTF16 support, via Boost-Locale (?)

      void set_body(std::string body);
      /*
      // If the content-type header is not yet set, it will set it to 
      void set_body(std::string &&body_text);
      // If the content-type header is not yet set, it will set it to 
      void set_body(const std::string &body_text);
      // If the content-type header is not yet set, it will set it to 
      void set_body(std::vector<unsigned char> &&body_data);
      // If the content-type header is not yet set, it will set it to 
      void set_body(const std::vector<unsigned char> &body_data);
      // If the content-type header is not yet set, it will set it to 
      void set_body(std::istream &stream);
      // If the content-type header is not yet set, it will set it to 
      void set_body(std::istream &stream, std::size_t content_length);
      */

      
      // Returns a stream representing the request data.
      HTTPResponse* getResult() {
        return response_;
      };
      
    protected:
      HTTPResponse* response_;   
  };
}

#endif