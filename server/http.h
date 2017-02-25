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
      static std::string getDefault(int code);
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
      static http::mime_type::ContentType GetMimeType(std::string extension);
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
 
}

#endif